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
#include "backend/helper/file_info.hpp"
#include "backend/helper/system_resources.hpp"
#include "backend/image_reader/bioformats/bioformats_loader.hpp"
#include "backend/pipelines/processor/channel_processor.hpp"
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
/// \brief      Get outputfolder of the actual job
/// \author     Joachim Danmayr
///
std::string Controller::getOutputFolder() const
{
  return joda::pipeline::PipelineFactory::getOutputFolder(mActProcessId);
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
/// \brief      Returns true as long as the thread is looking for files
/// \author     Joachim Danmayr
///
auto Controller::isLookingForFiles() -> bool
{
  return mWorkingDirectory.isRunning();
}

///
/// \brief      Sends a stop request and waits until the file looking has been ended
/// \author     Joachim Danmayr
///
void Controller::stopLookingForFiles()
{
  mWorkingDirectory.stop();
}

///
/// \brief      Sets the working directory
/// \author     Joachim Danmayr
///
auto Controller::getListOfFoundImages() -> const std::vector<FileInfo> &
{
  return mWorkingDirectory.getFilesList();
}

///
/// \brief      Returns preview
/// \author     Joachim Danmayr
///
auto Controller::preview(const settings::json::ChannelSettings &settings, int imgIndex, int tileIndex) -> Preview
{
  // To also preview tetraspeck removal we must first process the reference spot
  // channels This is a little bit more complicated therefor not supported yet

  // Now we can process the original channel
  auto imageFileName = mWorkingDirectory.getFileAt(imgIndex);
  if(!imageFileName.getFilename().empty()) {
    auto result = joda::algo::ChannelProcessor::processChannel(settings, imageFileName, tileIndex);
    std::vector<uchar> buffer;
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(0);
    cv::imencode(".png", result.controlImage, buffer,
                 compression_params);    // Assuming you want to encode as JPEG

    return {.data            = buffer,
            .height          = result.controlImage.rows,
            .width           = result.controlImage.cols,
            .detectionResult = result.result,
            .imageFileName   = imageFileName};
  }
  return {.data = {}, .height = 0, .width = 0, .detectionResult = {}, .imageFileName = {}};
}

///
/// \brief      Returns properties of given image
/// \author     Joachim Danmayr
///
auto Controller::getImageProperties(int imgIndex, int series) -> ImageProperties
{
  auto imagePath = mWorkingDirectory.getFileAt(imgIndex);

  ImageProperties props;
  switch(imagePath.getDecoder()) {
    case FileInfo::Decoder::JPG:
      props = JpgLoader::getImageProperties(imagePath);
      break;
    case FileInfo::Decoder::TIFF:
      props = TiffLoader::getImageProperties(imagePath, 0);
      break;
    case FileInfo::Decoder::BIOFORMATS:
      auto [_, propIn] = BioformatsLoader::getOmeInformation(imagePath, series);
      props            = propIn;
      break;
  }

  return props;
}

///
/// \brief      Returns properties of given image
/// \author     Joachim Danmayr
///
auto Controller::getSystemResources() -> Resources
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
  int series = 0;

  if(!settings.getChannelsVector().empty()) {
    series = settings.getChannelsVector()[0].getChannelInfo().getChannelSeries();
  }

  auto props        = getImageProperties(imgIndex, series);
  int64_t imgNr     = mWorkingDirectory.getNrOfFiles();
  int64_t tileNr    = 1;
  int64_t channelNr = settings.getChannels().size();

  auto systemRecources = getSystemResources();
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
  //  if(ch.getDetectionSettings().getDetectionMode() ==
  //  settings::json::ChannelDetection::DetectionMode::AI) {
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

  threads.cores[pipeline::Pipeline::ThreadingSettings::IMAGES]   = imgNr;
  threads.cores[pipeline::Pipeline::ThreadingSettings::TILES]    = tileNr;
  threads.cores[pipeline::Pipeline::ThreadingSettings::CHANNELS] = channelNr;

  uint64_t nr = threads.cores[pipeline::Pipeline::ThreadingSettings::CHANNELS];
  while(nr > maxNumberOfCoresToAssign) {
    threads.cores[pipeline::Pipeline::ThreadingSettings::CHANNELS]--;
    nr = threads.cores[pipeline::Pipeline::ThreadingSettings::CHANNELS];
  }

  nr = threads.cores[pipeline::Pipeline::ThreadingSettings::CHANNELS] *
       threads.cores[pipeline::Pipeline::ThreadingSettings::TILES];
  while(nr > maxNumberOfCoresToAssign) {
    threads.cores[pipeline::Pipeline::ThreadingSettings::TILES]--;
    nr = threads.cores[pipeline::Pipeline::ThreadingSettings::CHANNELS] *
         threads.cores[pipeline::Pipeline::ThreadingSettings::TILES];
  }

  nr = threads.cores[pipeline::Pipeline::ThreadingSettings::CHANNELS] *
       threads.cores[pipeline::Pipeline::ThreadingSettings::TILES] *
       threads.cores[pipeline::Pipeline::ThreadingSettings::IMAGES];
  while(nr > maxNumberOfCoresToAssign) {
    threads.cores[pipeline::Pipeline::ThreadingSettings::IMAGES]--;
    nr = threads.cores[pipeline::Pipeline::ThreadingSettings::CHANNELS] *
         threads.cores[pipeline::Pipeline::ThreadingSettings::TILES] *
         threads.cores[pipeline::Pipeline::ThreadingSettings::IMAGES];
  }

  threads.totalRuns = imgNr * tileNr * channelNr;

  return threads;
}

}    // namespace joda::ctrl
