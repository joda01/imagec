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
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include "../helper/helper.hpp"
#include "../image_processing/channel_processor.hpp"
#include "../logger/console_logger.hpp"
#include "backend/image_reader/image_reader.hpp"
#include "cell_approximation/cell_approximation.hpp"
#include <opencv2/imgcodecs.hpp>

namespace joda::pipeline {

using namespace std;
using namespace std::filesystem;

Pipeline::Pipeline(const joda::settings::json::AnalyzeSettings &settings,
                   joda::helper::ImageFileContainer *imageFileContainer) :
    mAnalyzeSettings(settings),
    mImageFileContainer(imageFileContainer)
{
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
void Pipeline::runJob(const std::string &inputFolder)
{
  mState = State::RUNNING;
  // Prepare
  mOutputFolder = prepareOutputFolder(inputFolder);

  // Store configuration
  static const std::string separator(1, std::filesystem::path::preferred_separator);
  mAnalyzeSettings.storeConfigToFile(mOutputFolder + separator + "settings.json");

  // Look for images in the input folder
  mImageFileContainer->setWorkingDirectory(inputFolder);
  mImageFileContainer->waitForFinished();
  mProgress.total.total = mImageFileContainer->getNrOfFiles();

  joda::reporting::Table alloverReport;

  //
  // Iterate over each image to do detection
  //
  int nrOfChannels = mAnalyzeSettings.getChannels().size();
  for(const auto &imagePath : mImageFileContainer->getFilesList()) {
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

    joda::reporting::Table detailReports;
    for(uint32_t tileIdx = 0; tileIdx < runs; tileIdx++) {
      //
      // Execute for each channel of the selected tile
      //
      std::map<int32_t, joda::func::DetectionResponse> detectionResults;
      int tempChannelIdx = 0;

      for(const auto &[_, channelSettings] : mAnalyzeSettings.getChannels()) {
        int32_t channelIndex = channelSettings.getChannelInfo().getChannelIndex();

        setDetailReportHeader(detailReports, channelSettings.getChannelInfo().getName(), tempChannelIdx);

        try {
          auto processingResult = joda::algo ::ChannelProcessor::processChannel(channelSettings, imagePath, tileIdx);
          //
          // Add processing result to the detection result map
          //
          detectionResults.emplace(channelIndex, processingResult);

          //
          // Add to detail report
          //
          appendToDetailReport(processingResult, detailReports, detailOutputFolder, tempChannelIdx, tileIdx);
          tempChannelIdx++;

        } catch(const std::exception &ex) {
          joda::log::logError(ex.what());
        }
      }

      //
      // Execute pipeline steps
      //
      for(const auto &pipelineStep : mAnalyzeSettings.getPipelineSteps()) {
        auto [index, response] = pipelineStep.execute(mAnalyzeSettings, detectionResults, detailOutputFolder);
        if(index != settings::json::PipelineStepSettings::PipelineStepIndex::NONE) {
          detectionResults.emplace(static_cast<int32_t>(index), response);
          setDetailReportHeader(detailReports, "Approx. Cells", tempChannelIdx);
          appendToDetailReport(response, detailReports, detailOutputFolder, tempChannelIdx, tileIdx);
          nrOfChannels++;
        }
      }

      mProgress.image.finished = tileIdx + 1;
      //
      // Free memory
      //
      if(mStop) {
        break;
      }
    }

    //
    // Write report
    //
    detailReports.flushReportToFile(detailOutputFolder + separator + "detail.csv");
    appendToAllOverReport(alloverReport, detailReports, imageName, nrOfChannels);

    mProgress.total.finished++;
    if(mStop) {
      break;
    }
  }

  std::string resultsFile = mOutputFolder + separator + "results.csv";
  alloverReport.flushReportToFile(resultsFile);
  mState = State::FINISHED;
}

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
  if(!result.controlImage.empty()) {
    cv::imwrite(detailReportOutputPath + separator + "control_" + std::to_string(tempChannelIdx) + "_" +
                    std::to_string(tileIdx) + ".jpg",
                result.controlImage);
  }
  if(!result.originalImage.empty()) {
    cv::imwrite(detailReportOutputPath + separator + "original_" + std::to_string(tempChannelIdx) + "_" +
                    std::to_string(tileIdx) + ".jpg",
                result.originalImage);
  }

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
  const int NR_OF_COLUMNS_PER_CHANNEL = 6;

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
         {colIdx + 5, detailedReport.getColumnNameAt(colIdxDetailReport +
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
        detailedReport.getStatistics(colIdxDetailReport + static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY));
    int rowIdx =
        allOverReport.appendValueToColumn(colIdx + 5, colStatistics.getAvg(), joda::func::ParticleValidity::VALID);

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

  bool directoryExists = false;
  if(!std::filesystem::exists(outputFolder)) {
    directoryExists = std::filesystem::create_directories(outputFolder);
    if(!directoryExists) {
      throw std::runtime_error("Can not create output folder!");
    }
  } else {
    directoryExists = true;
  }
  return outputFolder;
}

}    // namespace joda::pipeline
