///
/// \file      pipeline.cpp
/// \author    Joachim Danmayr
/// \date      2023-06-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "pipeline.hpp"
#include <algorithm>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "../helper/helper.hpp"
#include "../image_processing/channel_processor.hpp"
#include "../logger/console_logger.hpp"
#include "backend/helper/system_resources.hpp"
#include "backend/helper/thread_pool.hpp"
#include "backend/image_reader/image_reader.hpp"
#include "backend/settings/channel_settings.hpp"
#include "cell_approximation/cell_approximation.hpp"
#include <opencv2/imgcodecs.hpp>

namespace joda::pipeline {

using namespace std;
using namespace std::filesystem;

Pipeline::Pipeline(const joda::settings::json::AnalyzeSettings &settings,
                   joda::helper::ImageFileContainer *imageFileContainer, const std::string &inputFolder,
                   const ThreadingSettings &threadingSettings) :
    mInputFolder(inputFolder),
    mAnalyzeSettings(settings), mImageFileContainer(imageFileContainer), mThreadingSettings(threadingSettings)
{
  mMainThread = std::make_shared<std::thread>(&Pipeline::runJob, this);
}

///
/// \brief      Runs the pipeline.
///             Iterates over all found images and executes
///             the pipeline for each image by calling the
///             pure virtual function >execute< which must
///             be defined in one of the derived classes.
/// \author     Joachim Danmayr
/// \return
///
void Pipeline::runJob()
{
  try {
    mState = State::RUNNING;
    // Prepare
    mOutputFolder = prepareOutputFolder(mInputFolder);

    // Store configuration
    static const std::string separator(1, std::filesystem::path::preferred_separator);
    mAnalyzeSettings.storeConfigToFile(mOutputFolder + separator + "settings.json");

    // Look for images in the input folder
    mImageFileContainer->setWorkingDirectory(mInputFolder);
    mImageFileContainer->waitForFinished();
    mProgress.total.total = mImageFileContainer->getNrOfFiles();

    joda::reporting::Table alloverReport;

    int threadPoolImage   = mThreadingSettings.cores[ThreadingSettings::IMAGES];
    int threadPoolTile    = mThreadingSettings.cores[ThreadingSettings::TILES];
    int threadPoolChannel = mThreadingSettings.cores[ThreadingSettings::CHANNELS];
    BS::thread_pool imageThreadPool(threadPoolImage);
    BS::thread_pool tileThreadPool(threadPoolTile);
    BS::thread_pool channelThreadPool(threadPoolChannel);
    auto idStart = DurationCount::start("start");
    //
    // Iterate over each image to do detection
    //
    for(const auto &imagePath : mImageFileContainer->getFilesList()) {
      imageThreadPool.push_task([&] {
        std::string imageName   = helper::getFileNameFromPath(imagePath);
        auto detailOutputFolder = mOutputFolder + separator + imageName;
        std::filesystem::create_directories(detailOutputFolder);

        //
        // Execute for each tile
        //
        ImageProperties props;
        if(imagePath.ends_with(".jpg")) {
          props = JpgLoader::getImageProperties(imagePath);
        } else {
          props = TiffLoader::getImageProperties(imagePath, 0);
        }
        int64_t runs = 1;
        if(props.imageSize > joda::algo::MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE) {
          runs = props.nrOfTiles / joda::algo::TILES_TO_LOAD_PER_RUN;
        }
        mProgress.image.total = runs;

        //
        // Iterate over each tile
        //
        joda::reporting::Table detailReports;
        std::mutex writeDetailReportMutex;

        for(uint32_t tileIdx = 0; tileIdx < runs; tileIdx++) {
          tileThreadPool.push_task(
              [this, &detailReports, &channelThreadPool, &imagePath, &detailOutputFolder,
               &threadPoolChannel](int tileIdx) {
                auto idChannels = DurationCount::start("channels");
                //
                // Execute for each channel of the selected tile
                //
                std::map<int32_t, joda::func::DetectionResponse> detectionResults;

                //
                // Iterate over each channel
                //
                for(int chIdx = 0; chIdx < mAnalyzeSettings.getChannelsVector().size(); chIdx++) {
                  channelThreadPool.push_task(
                      [this, &detailReports, &detectionResults, &imagePath, &detailOutputFolder](int chIdx,
                                                                                                 int tileIdx) {
                        auto channelSettings = this->mAnalyzeSettings.getChannelsVector().at(chIdx);
                        analyszeChannel(detailReports, detectionResults, channelSettings, imagePath, detailOutputFolder,
                                        chIdx, tileIdx);
                      },
                      chIdx, tileIdx);
                  while(channelThreadPool.get_tasks_total() > (threadPoolChannel + THREAD_POOL_BUFFER)) {
                    std::this_thread::sleep_for(100us);
                  }
                  if(mStop) {
                    break;
                  }
                }
                channelThreadPool.wait_for_tasks();

                DurationCount::stop(idChannels);

                //
                // Execute pipeline steps
                //
                auto idColoc       = DurationCount::start("coloc");
                int tempChannelIdx = mAnalyzeSettings.getChannelsVector().size();
                for(const auto &pipelineStep : mAnalyzeSettings.getPipelineSteps()) {
                  auto [chSettings, response] =
                      pipelineStep.execute(mAnalyzeSettings, detectionResults, detailOutputFolder);
                  if(chSettings.index != settings::json::PipelineStepSettings::PipelineStepIndex::NONE) {
                    detectionResults.emplace(static_cast<int32_t>(chSettings.index), response);
                    setDetailReportHeader(detailReports, chSettings.name, tempChannelIdx);
                    appendToDetailReport(response, detailReports, detailOutputFolder, tempChannelIdx, tileIdx);
                    tempChannelIdx++;
                  }
                }
                DurationCount::stop(idColoc);

                {
                  std::lock_guard<std::mutex> lock(mWriteLock);
                  mProgress.image.finished++;
                }
              },
              tileIdx);
          //
          // Free memory
          //

          while(tileThreadPool.get_tasks_total() > (threadPoolTile + THREAD_POOL_BUFFER)) {
            std::this_thread::sleep_for(100us);
          }
          if(mStop) {
            break;
          }
        }
        tileThreadPool.wait_for_tasks();
        channelThreadPool.wait_for_tasks();

        //
        // Write report
        //
        detailReports.flushReportToFile(detailOutputFolder + separator + "detail.csv");

        auto nrOfChannels = mAnalyzeSettings.getChannelsVector().size() + mAnalyzeSettings.getPipelineSteps().size();
        appendToAllOverReport(alloverReport, detailReports, imageName, nrOfChannels);

        {
          std::lock_guard<std::mutex> lock(mWriteLock);
          mProgress.total.finished++;
        }
      });

      while(imageThreadPool.get_tasks_total() > (threadPoolImage + THREAD_POOL_BUFFER)) {
        std::this_thread::sleep_for(100us);
      }
      if(mStop) {
        break;
      }
    }

    imageThreadPool.wait_for_tasks();
    tileThreadPool.wait_for_tasks();
    channelThreadPool.wait_for_tasks();
    DurationCount::stop(idStart);

    std::string resultsFile = mOutputFolder + separator + "results.csv";
    alloverReport.flushReportToFile(resultsFile);
    mState = State::FINISHED;
  } catch(const std::exception &ex) {
    // setStateError(ex.what());
    mState            = State::ERROR_;
    mLastErrorMessage = ex.what();
  }
  while(!mStop) {
    sleep(1);
  }
}

///
/// \brief      Analyze channel
/// \author     Joachim Danmayr
///
void Pipeline::analyszeChannel(joda::reporting::Table &detailReports,
                               std::map<int32_t, joda::func::DetectionResponse> &detectionResults,
                               joda::settings::json::ChannelSettings &channelSettings, std::string imagePath,
                               std::string detailOutputFolder, int chIdx, int tileIdx)
{
  int32_t channelIndex = channelSettings.getChannelInfo().getChannelIndex();

  setDetailReportHeader(detailReports, channelSettings.getChannelInfo().getName(), chIdx);

  try {
    auto processingResult = joda::algo ::ChannelProcessor::processChannel(channelSettings, imagePath, tileIdx);
    //
    // Add processing result to the detection result map
    //
    auto id = DurationCount::start("emplace");
    detectionResults.emplace(channelIndex, processingResult);
    DurationCount::stop(id);

    //
    // Add to detail report
    //
    id = DurationCount::start("append");
    appendToDetailReport(processingResult, detailReports, detailOutputFolder, chIdx, tileIdx);
    DurationCount::stop(id);

  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
  }
}

///
/// \brief      Set detail report header
/// \author     Joachim Danmayr
///
void Pipeline::setDetailReportHeader(joda::reporting::Table &detailReportTable, const std::string &channelName,
                                     int tempChannelIdx)
{
  int colIdx = NR_OF_COLUMNS_PER_CHANNEL_IN_DETAIL_REPORT * tempChannelIdx;
  detailReportTable.setColumnNames(
      {{colIdx + static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE), channelName + "#confidence"},
       {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY), channelName + "#intensity"},
       {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN), channelName + "#Min"},
       {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX), channelName + "#Max"},
       {colIdx + static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE), channelName + "#areaSize"},
       {colIdx + static_cast<int>(ColumnIndexDetailedReport::PERIMETER), channelName + "#perimeter"},
       {colIdx + static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY), channelName + "#circularity"},
       {colIdx + static_cast<int>(ColumnIndexDetailedReport::VALIDITY), channelName + "#validity"}});
}

///
/// \brief      Append to detailed report
/// \author     Joachim Danmayr
/// \param[in]  inputFolder Inputfolder of the images
///
void Pipeline::appendToDetailReport(joda::func::DetectionResponse &result, joda::reporting::Table &detailReportTable,
                                    const std::string &detailReportOutputPath, int tempChannelIdx, uint32_t tileIdx)
{
  static const std::string separator(1, std::filesystem::path::preferred_separator);

  int colIdx = NR_OF_COLUMNS_PER_CHANNEL_IN_DETAIL_REPORT * tempChannelIdx;
  // Free memory
  std::vector<int> compression_params;
  compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
  compression_params.push_back(0);

  auto id = DurationCount::start("write-control-image");
  if(!result.controlImage.empty()) {
    cv::imwrite(detailReportOutputPath + separator + "control_" + std::to_string(tempChannelIdx) + "_" +
                    std::to_string(tileIdx) + ".png",
                result.controlImage, compression_params);
  }
  // if(!result.originalImage.empty()) {
  //   cv::imwrite(detailReportOutputPath + separator + "original_" + std::to_string(tempChannelIdx) + "_" +
  //                   std::to_string(tileIdx) + ".png",
  //               result.originalImage * ((float) UINT8_MAX / (float) UINT16_MAX), compression_params);
  // }
  DurationCount::stop(id);

  for(const auto &imgData : result.result) {
    detailReportTable.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE),
                                               imgData.getIndex(), imgData.getConfidence(), imgData.getValidity());
    detailReportTable.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY),
                                               imgData.getIndex(), imgData.getIntensity(), imgData.getValidity());
    detailReportTable.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN),
                                               imgData.getIndex(), imgData.getIntensityMin(), imgData.getValidity());
    detailReportTable.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX),
                                               imgData.getIndex(), imgData.getIntensityMax(), imgData.getValidity());
    detailReportTable.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE),
                                               imgData.getIndex(), imgData.getAreaSize(), imgData.getValidity());
    detailReportTable.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::PERIMETER),
                                               imgData.getIndex(), imgData.getPerimeter(), imgData.getValidity());
    detailReportTable.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY),
                                               imgData.getIndex(), imgData.getCircularity(), imgData.getValidity());
    detailReportTable.appendValueToColumnAtRow(colIdx + static_cast<int>(ColumnIndexDetailedReport::VALIDITY),
                                               imgData.getIndex(), imgData.getValidity());
  }
}

///
/// \brief      Append to all over report
/// \author     Joachim Danmayr
/// \param[in]  inputFolder Inputfolder of the images
/// \param[in]  nrOfChannels Nr. of channels
///
void Pipeline::appendToAllOverReport(joda::reporting::Table &allOverReport,
                                     const joda::reporting::Table &detailedReport, const std::string &imageName,
                                     int nrOfChannels)
{
  const int NR_OF_COLUMNS_PER_CHANNEL = 7;

  for(int tempChannelIdx = 0; tempChannelIdx < nrOfChannels; tempChannelIdx++) {
    int colIdx             = NR_OF_COLUMNS_PER_CHANNEL * tempChannelIdx;
    int colIdxDetailReport = NR_OF_COLUMNS_PER_CHANNEL_IN_DETAIL_REPORT * tempChannelIdx;

    allOverReport.setColumnNames(
        {{colIdx + 0, "Valid"},
         {colIdx + 1, "Invalid"},
         {colIdx + 2,
          detailedReport.getColumnNameAt(colIdxDetailReport + static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE))},
         {colIdx + 3,
          detailedReport.getColumnNameAt(colIdxDetailReport + static_cast<int>(ColumnIndexDetailedReport::INTENSITY)) +
              "#avg. intensity"},
         {colIdx + 4,
          detailedReport.getColumnNameAt(colIdxDetailReport + static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE)) +
              "#avg. areaSize"},
         {colIdx + 5,
          detailedReport.getColumnNameAt(colIdxDetailReport + static_cast<int>(ColumnIndexDetailedReport::PERIMETER)) +
              "#avg. perimeter"},
         {colIdx + 6, detailedReport.getColumnNameAt(colIdxDetailReport +
                                                     static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY)) +
                          "#avg. circularity"}});

    auto colStatistics =
        detailedReport.getStatistics(colIdxDetailReport + static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
    allOverReport.appendValueToColumn(colIdx + 0, colStatistics.getNr(), joda::func::ParticleValidity::VALID);

    colStatistics =
        detailedReport.getStatistics(colIdxDetailReport + static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
    allOverReport.appendValueToColumn(colIdx + 1, colStatistics.getInvalid(), joda::func::ParticleValidity::VALID);

    colStatistics =
        detailedReport.getStatistics(colIdxDetailReport + static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
    allOverReport.appendValueToColumn(colIdx + 2, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

    colStatistics =
        detailedReport.getStatistics(colIdxDetailReport + static_cast<int>(ColumnIndexDetailedReport::INTENSITY));
    allOverReport.appendValueToColumn(colIdx + 3, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

    colStatistics =
        detailedReport.getStatistics(colIdxDetailReport + static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE));
    allOverReport.appendValueToColumn(colIdx + 4, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);
    colStatistics =
        detailedReport.getStatistics(colIdxDetailReport + static_cast<int>(ColumnIndexDetailedReport::PERIMETER));
    allOverReport.appendValueToColumn(colIdx + 5, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);
    colStatistics =
        detailedReport.getStatistics(colIdxDetailReport + static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY));
    int rowIdx =
        allOverReport.appendValueToColumn(colIdx + 6, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

    allOverReport.setRowName(rowIdx, imageName);
  }
}
///
/// \brief      Creates the output folder for the results and returns the path.
///             Outputfolder = <inputFolder>/results/<DATE-TIME>
/// \author     Joachim Danmayr
/// \param[in]  inputFolder Inputfolder of the images
/// \return     Outputfolder of the results
///
[[nodiscard]] auto Pipeline::prepareOutputFolder(const std::string &inputFolder) -> std::string
{
  static const std::string separator(1, std::filesystem::path::preferred_separator);

  auto nowString    = ::joda::helper::timeNowToString();
  auto outputFolder = inputFolder + separator + RESULTS_PATH_NAME + separator + nowString;
  try {
    bool directoryExists = false;
    if(!std::filesystem::exists(outputFolder)) {
      directoryExists = std::filesystem::create_directories(outputFolder);
      if(!directoryExists) {
        setStateError("Can not create output folder!");
      }
    } else {
      directoryExists = true;
    }
    return outputFolder;
  } catch(const std::exception &ex) {
    setStateError(ex.what());
  }
}

}    // namespace joda::pipeline
