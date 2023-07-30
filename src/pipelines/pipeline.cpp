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
#include <bits/types/FILE.h>
#include <algorithm>
#include <exception>
#include <filesystem>
#include <string>
#include "helper/helper.hpp"
#include "image_processing/channel_processor.hpp"
#include "image_processing/functions/detection/voronoi_grid/voronoi_grid.hpp"
#include "logger/console_logger.hpp"
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
  mAnalyzeSettings.storeConfigToFile(mOutputFolder + std::filesystem::path::preferred_separator + "settings.json");

  // Look for images in the input folder
  mImageFileContainer->setWorkingDirectory(inputFolder);
  mImageFileContainer->waitForFinished();
  mProgress.total.total = mImageFileContainer->getNrOfFiles();

  joda::reporting::Table alloverReport;
  std::map<int, joda::func::DetectionResponse> detectionResults;

  //
  // Iterate over each image to do detection
  //
  int nrOfChannels = mAnalyzeSettings.getChannels().size();
  for(const auto &imagePath : mImageFileContainer->getFilesList()) {
    //
    // Process channel by channel
    joda::reporting::Table detailReport;
    int tempChannelIdx    = 0;
    std::string imageName = helper::getFileNameFromPath(imagePath);
    auto detailOutput     = mOutputFolder + std::filesystem::path::preferred_separator + imageName;
    std::filesystem::create_directories(detailOutput);

    for(const auto &[_, channelSettings] : mAnalyzeSettings.getChannels()) {
      try {
        auto processingResult = joda::algo ::ChannelProcessor::processChannel(channelSettings, imagePath,
                                                                              &mProgress.image, getStopReference());

        //
        // Add processing result to the detection result map
        //
        detectionResults.emplace(channelSettings.getChannelInfo().getChannelIndex(), processingResult.at(0));

        //
        // Add to detail report
        //
        appendToDetailReport(processingResult, detailReport, detailOutput, channelSettings, tempChannelIdx);
        tempChannelIdx++;

      } catch(const std::exception &ex) {
        joda::log::logError(ex.what());
      }
    }

    //
    // Execute the next pipeline steps
    //
    joda::func::img::VoronoiGrid grid(detectionResults[2].result);
    grid.forward(detectionResults[2].controlImage, detectionResults[2].originalImage);

    //
    // Write report
    //
    detailReport.flushReportToFile(detailOutput + std::filesystem::path::preferred_separator + "detail.csv");
    appendToAllOverReport(alloverReport, detailReport, imageName, nrOfChannels);
    mProgress.total.finished++;
    if(mStop) {
      break;
    }
  }

  std::string resultsFile = mOutputFolder + std::filesystem::path::preferred_separator + "results.csv";
  alloverReport.flushReportToFile(resultsFile);
  mState = State::FINISHED;
}

///
/// \brief      Append to detailed report
/// \author     Joachim Danmayr
/// \param[in]  inputFolder Inputfolder of the images
///
void Pipeline::appendToDetailReport(joda::func::ProcessingResult &result, joda::reporting::Table &detailReportTable,
                                    const std::string &detailReportOutputPath,
                                    const settings::json::ChannelSettings &channelSettings, int tempChannelIdx)
{
  int colIdx = NR_OF_COLUMNS_PER_CHANNEL_IN_DETAIL_REPORT * tempChannelIdx;

  detailReportTable.setColumnNames({{colIdx + static_cast<int>(ColumnIndexDetailedReport::CONFIDENCE),
                                     channelSettings.getChannelInfo().getName() + "#confidence"},
                                    {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY),
                                     channelSettings.getChannelInfo().getName() + "#intensity"},
                                    {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MIN),
                                     channelSettings.getChannelInfo().getName() + "#Min"},
                                    {colIdx + static_cast<int>(ColumnIndexDetailedReport::INTENSITY_MAX),
                                     channelSettings.getChannelInfo().getName() + "#Max"},
                                    {colIdx + static_cast<int>(ColumnIndexDetailedReport::AREA_SIZE),
                                     channelSettings.getChannelInfo().getName() + "#areaSize"},
                                    {colIdx + static_cast<int>(ColumnIndexDetailedReport::CIRCULARITY),
                                     channelSettings.getChannelInfo().getName() + "#circularity"},
                                    {colIdx + static_cast<int>(ColumnIndexDetailedReport::VALIDITY),
                                     channelSettings.getChannelInfo().getName() + "#validity"}});

  for(const auto &[tileIdx, tileData] : result) {
    cv::imwrite(detailReportOutputPath + std::filesystem::path::preferred_separator + "control_" +
                    std::to_string(tempChannelIdx) + "_" + std::to_string(tileIdx) + ".jpg",
                tileData.controlImage);

    cv::imwrite(detailReportOutputPath + std::filesystem::path::preferred_separator + "original_" +
                    std::to_string(tempChannelIdx) + "_" + std::to_string(tileIdx) + ".jpg",
                tileData.originalImage);

    for(const auto &imgData : tileData.result) {
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
  auto nowString    = ::joda::helper::timeNowToString();
  auto outputFolder = inputFolder + std::filesystem::path::preferred_separator + RESULTS_PATH_NAME +
                      std::filesystem::path::preferred_separator + nowString;

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
