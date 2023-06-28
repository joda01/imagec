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
#include "helper/helper.hpp"
#include "image_processing/channel_processor.hpp"
#include "logger/console_logger.hpp"

namespace joda::pipeline {

using namespace std;
using namespace std::filesystem;

Pipeline::Pipeline(const joda::settings::json::AnalyzeSettings &settings) : mAnalyzeSettings(settings)
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
  mInputFiles.lookForImagesInFolderAndSubfolder(inputFolder);
  mProgress.total.total = mInputFiles.getNrOfFiles();

  // Iterate over each image
  for(const auto &imagePath : mInputFiles.getFilesList()) {
    //
    // Process channel by channel
    for(const auto &[_, channelSettings] : mAnalyzeSettings.getChannels()) {
      try {
        auto processingResult = joda::algo ::ChannelProcessor::processChannel(channelSettings, imagePath,
                                                                              &mProgress.image, getStopReference());
      } catch(const std::exception &ex) {
        joda::log::logError(ex.what());
      }
    }

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
