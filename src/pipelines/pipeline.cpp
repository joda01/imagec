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

  // Iterate over each image
  for(const auto &imagePath : mImageFileContainer->getFilesList()) {
    //
    // Process channel by channel
    joda::reporting::Table detailReport;
    int tempChannelIdx = 0;
    auto detailOutput =
        mOutputFolder + std::filesystem::path::preferred_separator + helper::getFileNameFromPath(imagePath);
    std::filesystem::create_directories(detailOutput);

    for(const auto &[_, channelSettings] : mAnalyzeSettings.getChannels()) {
      try {
        auto processingResult = joda::algo ::ChannelProcessor::processChannel(channelSettings, imagePath,
                                                                              &mProgress.image, getStopReference());

        appendToDetailReport(processingResult, detailReport, detailOutput, channelSettings, tempChannelIdx);
        tempChannelIdx++;

      } catch(const std::exception &ex) {
        joda::log::logError(ex.what());
      }
    }

    detailReport.flushReportToFile(detailOutput + std::filesystem::path::preferred_separator + "detail.csv");

    mProgress.total.finished++;
    if(mStop) {
      break;
    }
  }

  std::string resultsFile = mOutputFolder + std::filesystem::path::preferred_separator + "results.csv";
  mAllOverReporting.flushReportToFile(resultsFile);
  mState = State::FINISHED;
}

///
/// \brief      Append to detailed report
/// \author     Joachim Danmayr
/// \param[in]  inputFolder Inputfolder of the images
/// \return     Outputfolder of the results
///
void Pipeline::appendToDetailReport(joda::func::ProcessingResult &result, joda::reporting::Table &detailReportTable,
                                    const std::string &detailReportOutputPath,
                                    const settings::json::ChannelSettings &channelSettings, int tempChannelIdx)
{
  const int NR_OF_COLUMNS_PER_CHANNEL = 6;
  int colIdx                          = NR_OF_COLUMNS_PER_CHANNEL * tempChannelIdx;

  detailReportTable.setColumnNames({{colIdx + 0, channelSettings.getChannelInfo().getName() + "#confidence"},
                                    {colIdx + 1, channelSettings.getChannelInfo().getName() + "#intensity"},
                                    {colIdx + 2, channelSettings.getChannelInfo().getName() + "#Min"},
                                    {colIdx + 3, channelSettings.getChannelInfo().getName() + "#Max"},
                                    {colIdx + 4, channelSettings.getChannelInfo().getName() + "#areaSize"},
                                    {colIdx + 5, channelSettings.getChannelInfo().getName() + "#circularity"}});

  for(const auto &[tileIdx, tileData] : result) {
    cv::imwrite(detailReportOutputPath + std::filesystem::path::preferred_separator + "control_" +
                    std::to_string(tempChannelIdx) + "_" + std::to_string(tileIdx) + ".jpg",
                tileData.controlImage);

    for(const auto &imgData : tileData.result) {
      detailReportTable.appendValueToColumnAtRow(colIdx + 0, imgData.index, imgData.confidence);
      detailReportTable.appendValueToColumnAtRow(colIdx + 1, imgData.index, imgData.intensity);
      detailReportTable.appendValueToColumnAtRow(colIdx + 2, imgData.index, imgData.intensityMin);
      detailReportTable.appendValueToColumnAtRow(colIdx + 3, imgData.index, imgData.intensityMax);
      detailReportTable.appendValueToColumnAtRow(colIdx + 4, imgData.index, imgData.areaSize);
      detailReportTable.appendValueToColumnAtRow(colIdx + 5, imgData.index, imgData.circularity);
    }
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
