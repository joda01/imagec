///
/// \file      controller.cpp
/// \author    Joachim Danmayr
/// \date      2023-08-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "controller.hpp"
#include <algorithm>
#include <map>
#include <ranges>
#include "backend/helper/system_resources.hpp"
#include "backend/image_processing/channel_processor.hpp"
#include "backend/settings/analze_settings_parser.hpp"
#include "backend/settings/channel_settings.hpp"

namespace joda::ctrl {

Controller::Controller()
{
}

///
/// \brief      Start a new process
/// \author     Joachim Danmayr
///
void Controller::start(const settings::json::AnalyzeSettings &settings,
                       const pipeline::Pipeline::ThreadingSettings &threadSettings)
{
  try {
    mActProcessId = joda::pipeline::PipelineFactory::startNewJob(settings, mWorkingDirectory.getWorkingDirectory(),
                                                                 &mWorkingDirectory, threadSettings);
    joda::log::logInfo("Analyze started!");
  } catch(const std::exception &ex) {
    joda::log::logWarning("Analyze could not be started! Got " + std::string(ex.what()) + ".");
  }
}

///
/// \brief      Stop a running process
/// \author     Joachim Danmayr
///
void Controller::stop()
{
  joda::pipeline::PipelineFactory::stopJob(mActProcessId);
}

///
/// \brief      Stop a running process
/// \author     Joachim Danmayr
///
void Controller::reset()
{
  joda::pipeline::PipelineFactory::reset();
}

///
/// \brief      Returns process state
/// \author     Joachim Danmayr
///
std::tuple<joda::pipeline::Pipeline::ProgressIndicator, joda::pipeline::Pipeline::State, std::string>
Controller::getState()
{
  return joda::pipeline::PipelineFactory::getState(mActProcessId);
}

///
/// \brief      Get actual settings
/// \author     Joachim Danmayr
///
void Controller::getSettings()
{
}

///
/// \brief      Sets the working directory
/// \author     Joachim Danmayr
///
void Controller::setWorkingDirectory(const std::string &dir)
{
  mWorkingDirectory.setWorkingDirectory(dir);
}

///
/// \brief      Sets the working directory
/// \author     Joachim Danmayr
///
auto Controller::getNrOfFoundImages() -> uint32_t
{
  return mWorkingDirectory.getNrOfFiles();
}

///
/// \brief      Returns preview
/// \author     Joachim Danmayr
///
auto Controller::preview(const settings::json::ChannelSettings &settings, int imgIndex) -> Preview
{
  auto result = joda::algo::ChannelProcessor::processChannel(settings, mWorkingDirectory.getFileAt(imgIndex), 0);
  std::vector<uchar> buffer;
  std::vector<int> compression_params;
  compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
  compression_params.push_back(9);
  cv::imencode(".png", result.controlImage, buffer, compression_params);    // Assuming you want to encode as JPEG

  return {.data = buffer, .height = result.controlImage.rows, .width = result.controlImage.cols};
}

///
/// \brief      Returns properties of given image
/// \author     Joachim Danmayr
///
auto Controller::getImageProperties(int imgIndex) -> ImageProperties
{
  auto imagePath = mWorkingDirectory.getFileAt(imgIndex);

  ImageProperties props;
  if(imagePath.ends_with(".jpg")) {
    props = JpgLoader::getImageProperties(imagePath);
  } else {
    props = TiffLoader::getImageProperties(imagePath, 0);
  }
  return props;
}

///
/// \brief      Returns properties of given image
/// \author     Joachim Danmayr
///
auto Controller::getSystemRescources() -> Resources
{
  return {.ramTotal     = system::getTotalSystemMemory(),
          .ramAvailable = system::getAvailableSystemMemory(),
          .cpus         = system::getNrOfCPUs()};
}

///
/// \brief      Calc optimal number of threads
/// \author     Joachim Danmayr
///
auto Controller::calcOptimalThreadNumber(const settings::json::AnalyzeSettings &settings, int imgIndex)
    -> pipeline::Pipeline::ThreadingSettings
{
  pipeline::Pipeline::ThreadingSettings threads;

  auto props        = getImageProperties(imgIndex);
  int64_t imgNr     = mWorkingDirectory.getNrOfFiles();
  int64_t tileNr    = 1;
  int64_t channelNr = settings.getChannels().size();

  auto systemRecources = getSystemRescources();
  if(props.imageSize > joda::algo::MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE) {
    tileNr              = props.nrOfTiles / joda::algo::TILES_TO_LOAD_PER_RUN;
    threads.ramPerImage = props.tileSize * joda::algo::TILES_TO_LOAD_PER_RUN;
  } else {
    threads.ramPerImage = props.imageSize;
  }
  threads.ramFree        = systemRecources.ramAvailable;
  threads.ramTotal       = systemRecources.ramTotal;
  threads.coresAvailable = systemRecources.cpus;

  // No multi threading when AI is used, sinze AI is still using all cPUs
  // for(const auto &ch : settings.getChannelsVector()) {
  //  if(ch.getDetectionSettings().getDetectionMode() == settings::json::ChannelDetection::DetectionMode::AI) {
  //    // return threads;
  //  }
  //}

  // Maximum number of cores depends on the available RAM.
  int32_t maxNumberOfCoresToAssign =
      std::min(static_cast<uint64_t>(systemRecources.cpus),
               static_cast<uint64_t>(systemRecources.ramAvailable / threads.ramPerImage));
  if(maxNumberOfCoresToAssign <= 0) {
    maxNumberOfCoresToAssign = 1;
  }
  if(maxNumberOfCoresToAssign > 1 && maxNumberOfCoresToAssign == systemRecources.cpus) {
    // Don't use all CPU cores if there are more than 1
    maxNumberOfCoresToAssign--;
  }

  std::multimap<int64_t, pipeline::Pipeline::ThreadingSettings::Type> numbers = {
      {imgNr, pipeline::Pipeline::ThreadingSettings::IMAGES},
      {tileNr, pipeline::Pipeline::ThreadingSettings::TILES},
      {channelNr, pipeline::Pipeline::ThreadingSettings::CHANNELS}};

  // Iterate through the set in reverse order
  for(auto rit = numbers.rbegin(); rit != numbers.rend(); ++rit) {
    uint64_t cores = 1;
    if(maxNumberOfCoresToAssign > 1) {
      cores = std::min(static_cast<uint64_t>(rit->first), static_cast<uint64_t>(maxNumberOfCoresToAssign));
      maxNumberOfCoresToAssign -= cores;
      if(maxNumberOfCoresToAssign <= 0) {
        maxNumberOfCoresToAssign = 1;
      }
    }
    threads.cores[rit->second] = cores;
  }

  return threads;
}

}    // namespace joda::ctrl
