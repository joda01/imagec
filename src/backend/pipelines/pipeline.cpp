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
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "../helper/helper.hpp"
#include "../logger/console_logger.hpp"
#include "backend/helper/file_info.hpp"
#include "backend/helper/system_resources.hpp"
#include "backend/helper/thread_pool.hpp"
#include "backend/image_reader/bioformats/bioformats_loader.hpp"
#include "backend/image_reader/image_reader.hpp"
#include "backend/reporting/reporting_container.hpp"
#include "backend/settings/channel_settings.hpp"
#include "backend/settings/pipeline_settings.hpp"
#include "pipeline_steps/cell_approximation/cell_approximation.hpp"
#include "processor/channel_processor.hpp"
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
    mProgress.image.total = mThreadingSettings.totalRuns;

    int threadPoolImage = mThreadingSettings.cores[ThreadingSettings::IMAGES];
    BS::thread_pool imageThreadPool(threadPoolImage);
    auto idStart = DurationCount::start("analyze");

    std::map<std::string, joda::reporting::ReportingContainer> alloverReport;
    for(const auto &imagePath : mImageFileContainer->getFilesList()) {
      if(threadPoolImage > 1) {
        imageThreadPool.push_task([this, &alloverReport, imagePath] { analyzeImage(alloverReport, imagePath); });

        while(imageThreadPool.get_tasks_total() > (threadPoolImage + THREAD_POOL_BUFFER)) {
          std::this_thread::sleep_for(100us);
        }
      } else {
        analyzeImage(alloverReport, imagePath);
      }
      if(mStop) {
        break;
      }
    }

    imageThreadPool.wait_for_tasks();
    DurationCount::stop(idStart);

    std::string resultsFile = mOutputFolder + separator + "results.xlsx";

    reporting::ReportingContainer::flushReportToFile(alloverReport, resultsFile,
                                                     reporting::ReportingContainer::OutputFormat::HORIZONTAL);

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
/// \brief      Analyze image
/// \author     Joachim Danmayr
///
void Pipeline::analyzeImage(std::map<std::string, joda::reporting::ReportingContainer> &alloverReport,
                            const FileInfo &imagePath)
{
  int threadPoolTile = mThreadingSettings.cores[ThreadingSettings::TILES];
  BS::thread_pool tileThreadPool(threadPoolTile);

  std::string imageName       = helper::getFileNameFromPath(imagePath.getPath());
  std::string imageParentPath = helper::getFolderNameFromPath(imagePath.getPath());

  static const std::string separator(1, std::filesystem::path::preferred_separator);
  auto detailOutputFolder = mOutputFolder + separator + imageName;

  std::filesystem::create_directories(detailOutputFolder);

  //
  // Execute for each tile
  //
  ImageProperties props;
  if(imagePath.getDecoder() == FileInfo::Decoder::JPG) {
    props = JpgLoader::getImageProperties(imagePath.getPath());
  } else if(imagePath.getDecoder() == FileInfo::Decoder::TIFF) {
    props = TiffLoader::getImageProperties(imagePath.getPath(), 0);
  } else {
    if(!mAnalyzeSettings.getChannelsVector().empty()) {
      auto series       = mAnalyzeSettings.getChannelsVector()[0].getChannelInfo().getChannelSeries();
      auto [ome, props] = BioformatsLoader::getOmeInformation(imagePath.getPath(), series);
    }
  }
  int64_t runs = 1;
  if(props.imageSize > joda::algo::MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE) {
    runs = props.nrOfTiles / joda::algo::TILES_TO_LOAD_PER_RUN;
  }

  //
  // Iterate over each tile
  //
  std::map<std::string, reporting::ReportingContainer> detailReports;
  std::mutex writeDetailReportMutex;

  for(uint32_t tileIdx = 0; tileIdx < runs; tileIdx++) {
    if(threadPoolTile > 1) {
      tileThreadPool.push_task(
          [this, &detailReports, &imagePath, &detailOutputFolder](int tileIdx) {
            analyzeTile(detailReports[""], imagePath, detailOutputFolder, tileIdx);
          },
          tileIdx);

      while(tileThreadPool.get_tasks_total() > (threadPoolTile + THREAD_POOL_BUFFER)) {
        std::this_thread::sleep_for(100us);
      }
    } else {
      analyzeTile(detailReports[""], imagePath, detailOutputFolder, tileIdx);
    }
    if(mStop) {
      break;
    }
  }
  tileThreadPool.wait_for_tasks();

  //
  // Write report
  //
  reporting::ReportingContainer::flushReportToFile(detailReports, detailOutputFolder + separator + "detail.xlsx",
                                                   reporting::ReportingContainer::OutputFormat::VERTICAL);

  auto nrOfChannels = mAnalyzeSettings.getChannelsVector().size() + mAnalyzeSettings.getPipelineSteps().size();
  appendToAllOverReport(alloverReport, detailReports[""], imageParentPath, imageName, nrOfChannels);

  mProgress.total.finished++;
}

///
/// \brief      Analyze tile
/// \author     Joachim Danmayr
///
void Pipeline::analyzeTile(joda::reporting::ReportingContainer &detailReports, FileInfo imagePath,
                           std::string detailOutputFolder, int tileIdx)
{
  auto idChannels = DurationCount::start("channels");
  std::map<int32_t, joda::func::DetectionResponse> detectionResults;
  int threadPoolChannel = mThreadingSettings.cores[ThreadingSettings::CHANNELS];
  BS::thread_pool channelThreadPool(threadPoolChannel);

  //
  // Analyze the reference spots first
  //
  auto referenceSpotChannels = this->mAnalyzeSettings.getChannels(settings::json::ChannelInfo::Type::SPOT_REFERENCE);
  if(!referenceSpotChannels.empty()) {
    for(const auto &referenceSpot : referenceSpotChannels) {
      if(threadPoolChannel > 1) {
        channelThreadPool.push_task(
            [this, &detailReports, &detectionResults, &imagePath, &detailOutputFolder](int chIdx, int tileIdx) {
              auto channelSettings = this->mAnalyzeSettings.getChannelsVector().at(chIdx);
              analyszeChannel(detailReports, detectionResults, channelSettings, imagePath, detailOutputFolder, chIdx,
                              tileIdx);
            },
            referenceSpot.getArrayIndex(), tileIdx);
        while(channelThreadPool.get_tasks_total() > (threadPoolChannel + THREAD_POOL_BUFFER)) {
          std::this_thread::sleep_for(100us);
        }
      } else {
        auto channelSettings = this->mAnalyzeSettings.getChannelsVector().at(referenceSpot.getArrayIndex());
        analyszeChannel(detailReports, detectionResults, channelSettings, imagePath, detailOutputFolder,
                        referenceSpot.getArrayIndex(), tileIdx);
      }
    }
    channelThreadPool.wait_for_tasks();
  }

  //
  // Iterate over each channel except reference spots
  //
  for(int chIdx = 0; chIdx < mAnalyzeSettings.getChannelsVector().size(); chIdx++) {
    if(this->mAnalyzeSettings.getChannelsVector().at(chIdx).getChannelInfo().getType() !=
       settings::json::ChannelInfo::Type::SPOT_REFERENCE) {
      if(threadPoolChannel > 1) {
        channelThreadPool.push_task(
            [this, &detailReports, &detectionResults, &imagePath, &detailOutputFolder](int chIdx, int tileIdx) {
              auto channelSettings = this->mAnalyzeSettings.getChannelsVector().at(chIdx);
              analyszeChannel(detailReports, detectionResults, channelSettings, imagePath, detailOutputFolder, chIdx,
                              tileIdx);
            },
            chIdx, tileIdx);
        while(channelThreadPool.get_tasks_total() > (threadPoolChannel + THREAD_POOL_BUFFER)) {
          std::this_thread::sleep_for(100us);
        }
      } else {
        auto channelSettings = this->mAnalyzeSettings.getChannelsVector().at(chIdx);
        analyszeChannel(detailReports, detectionResults, channelSettings, imagePath, detailOutputFolder, chIdx,
                        tileIdx);
      }
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
  auto idColoc       = DurationCount::start("pipelinesteps");
  int tempChannelIdx = mAnalyzeSettings.getChannelsVector().size();
  for(const auto &pipelineStep : mAnalyzeSettings.getPipelineSteps()) {
    auto [chSettings, response] = pipelineStep.execute(mAnalyzeSettings, detectionResults, detailOutputFolder);
    if(chSettings.index != settings::json::PipelineStepSettings::NONE_PIPELINE_STEP) {
      detectionResults.emplace(static_cast<int32_t>(chSettings.index), response);
      setDetailReportHeader(detailReports, chSettings.name, tempChannelIdx);
      appendToDetailReport(response, detailReports, detailOutputFolder, tempChannelIdx, tileIdx);
      tempChannelIdx++;
    }
  }
  DurationCount::stop(idColoc);
}

///
/// \brief      Analyze channel
/// \author     Joachim Danmayr
///
void Pipeline::analyszeChannel(joda::reporting::ReportingContainer &detailReports,
                               std::map<int32_t, joda::func::DetectionResponse> &detectionResults,
                               const joda::settings::json::ChannelSettings &channelSettings, FileInfo imagePath,
                               std::string detailOutputFolder, int chIdx, int tileIdx)
{
  int32_t channelIndex = channelSettings.getChannelInfo().getChannelIndex();

  setDetailReportHeader(detailReports, channelSettings.getChannelInfo().getName(), chIdx);

  try {
    auto processingResult =
        joda::algo ::ChannelProcessor::processChannel(channelSettings, imagePath, tileIdx, &detectionResults);

    std::cout << "RES " << std::to_string(processingResult.result.size()) << std::endl;

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
  mProgress.image.finished++;
}

///
/// \brief      Set detail report header
/// \author     Joachim Danmayr
///
void Pipeline::setDetailReportHeader(joda::reporting::ReportingContainer &detailReportTable,
                                     const std::string &channelName, int tempChannelIdx)
{
  detailReportTable.getTableAt(tempChannelIdx, channelName)
      .setColumnNames({{static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE), "#confidence"},
                       {static_cast<int>(ColumnIndexDetailedReport::INTENSITY), "#intensity"},
                       {static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN), "#Min"},
                       {static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX), "#Max"},
                       {static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE), "#areaSize"},
                       {static_cast<int>(ColumnIndexDetailedReport::PERIMETER), "#perimeter"},
                       {static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY), "#circularity"},
                       {static_cast<int>(ColumnIndexDetailedReport::VALIDITY), "#validity"}});
}

///
/// \brief      Append to detailed report
/// \author     Joachim Danmayr
/// \param[in]  inputFolder Inputfolder of the images
///
void Pipeline::appendToDetailReport(joda::func::DetectionResponse &result,
                                    joda::reporting::ReportingContainer &detailReportTable,
                                    const std::string &detailReportOutputPath, int tempChannelIdx, uint32_t tileIdx)
{
  static const std::string separator(1, std::filesystem::path::preferred_separator);

  // Free memory
  std::vector<int> compression_params;
  compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
  compression_params.push_back(0);

  auto id = DurationCount::start("write-control-image");
  if(!result.controlImage.empty()) {
    cv::imwrite(detailReportOutputPath + separator + "control_" + std::to_string(tempChannelIdx) + "_" +
                    std::to_string(tileIdx) + ".png",
                result.controlImage, compression_params);
  } else {
    std::cout << "CTRL img null" << std::endl;
  }
  // if(!result.originalImage.empty()) {
  //   cv::imwrite(detailReportOutputPath + separator + "original_" + std::to_string(tempChannelIdx) + "_" +
  //                   std::to_string(tileIdx) + ".png",
  //               result.originalImage * ((float) UINT8_MAX / (float) UINT16_MAX), compression_params);
  // }
  DurationCount::stop(id);

  for(const auto &imgData : result.result) {
    detailReportTable.getTableAt(tempChannelIdx, "")
        .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE), imgData.getIndex(),
                                  imgData.getConfidence(), imgData.getValidity());
    detailReportTable.getTableAt(tempChannelIdx, "")
        .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::INTENSITY), imgData.getIndex(),
                                  imgData.getIntensity(), imgData.getValidity());
    detailReportTable.getTableAt(tempChannelIdx, "")
        .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN), imgData.getIndex(),
                                  imgData.getIntensityMin(), imgData.getValidity());
    detailReportTable.getTableAt(tempChannelIdx, "")
        .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX), imgData.getIndex(),
                                  imgData.getIntensityMax(), imgData.getValidity());
    detailReportTable.getTableAt(tempChannelIdx, "")
        .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE), imgData.getIndex(),
                                  imgData.getAreaSize(), imgData.getValidity());
    detailReportTable.getTableAt(tempChannelIdx, "")
        .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::PERIMETER), imgData.getIndex(),
                                  imgData.getPerimeter(), imgData.getValidity());
    detailReportTable.getTableAt(tempChannelIdx, "")
        .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY), imgData.getIndex(),
                                  imgData.getCircularity(), imgData.getValidity());
    detailReportTable.getTableAt(tempChannelIdx, "")
        .appendValueToColumnAtRow(static_cast<int>(ColumnIndexDetailedReport::VALIDITY), imgData.getIndex(),
                                  imgData.getValidity());
  }
}

///
/// \brief      Append to all over report
/// \author     Joachim Danmayr
/// \param[in]  inputFolder Inputfolder of the images
/// \param[in]  nrOfChannels Nr. of channels
///
void Pipeline::appendToAllOverReport(std::map<std::string, joda::reporting::ReportingContainer> &allOverReport,
                                     const joda::reporting::ReportingContainer &detailedReport,
                                     const std::string &imagePath, const std::string &imageName, int nrOfChannels)
{
  const int NR_OF_COLUMNS_PER_CHANNEL = 7;

  for(int tempChannelIdx = 0; tempChannelIdx < nrOfChannels; tempChannelIdx++) {
    allOverReport[imagePath]
        .getTableAt(tempChannelIdx, detailedReport.getTableAt(tempChannelIdx).getTableName())
        .setColumnNames({{0, "#valid"},
                         {1, "#invalid"},
                         {2, detailedReport.getTableAt(tempChannelIdx)
                                 .getColumnNameAt(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE))},
                         {3, detailedReport.getTableAt(tempChannelIdx)
                                 .getColumnNameAt(static_cast<int>(ColumnIndexDetailedReport::INTENSITY))},
                         {4, detailedReport.getTableAt(tempChannelIdx)
                                 .getColumnNameAt(static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE))},
                         {5, detailedReport.getTableAt(tempChannelIdx)
                                 .getColumnNameAt(static_cast<int>(ColumnIndexDetailedReport::PERIMETER))},
                         {6, detailedReport.getTableAt(tempChannelIdx)
                                 .getColumnNameAt(static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY))}});

    /// \todo Copy constructor ia called here
    auto colStatistics = detailedReport.getTableAt(tempChannelIdx)
                             .getStatistics(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
    allOverReport[imagePath]
        .getTableAt(tempChannelIdx, "")
        .appendValueToColumn(0, colStatistics.getNr(), joda::func::ParticleValidity::VALID);

    colStatistics = detailedReport.getTableAt(tempChannelIdx)
                        .getStatistics(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
    allOverReport[imagePath]
        .getTableAt(tempChannelIdx, "")
        .appendValueToColumn(1, colStatistics.getInvalid(), joda::func::ParticleValidity::VALID);

    colStatistics = detailedReport.getTableAt(tempChannelIdx)
                        .getStatistics(static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE));
    allOverReport[imagePath]
        .getTableAt(tempChannelIdx, "")
        .appendValueToColumn(2, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

    colStatistics =
        detailedReport.getTableAt(tempChannelIdx).getStatistics(static_cast<int>(ColumnIndexDetailedReport::INTENSITY));
    allOverReport[imagePath]
        .getTableAt(tempChannelIdx, "")
        .appendValueToColumn(3, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

    colStatistics =
        detailedReport.getTableAt(tempChannelIdx).getStatistics(static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE));
    allOverReport[imagePath]
        .getTableAt(tempChannelIdx, "")
        .appendValueToColumn(4, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);
    colStatistics =
        detailedReport.getTableAt(tempChannelIdx).getStatistics(static_cast<int>(ColumnIndexDetailedReport::PERIMETER));
    allOverReport[imagePath]
        .getTableAt(tempChannelIdx, "")
        .appendValueToColumn(5, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);
    colStatistics = detailedReport.getTableAt(tempChannelIdx)
                        .getStatistics(static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY));
    int rowIdx = allOverReport[imagePath]
                     .getTableAt(tempChannelIdx, "")
                     .appendValueToColumn(6, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

    allOverReport[imagePath].getTableAt(tempChannelIdx, "").setRowName(rowIdx, imageName);
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
