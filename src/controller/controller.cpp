///
/// \file      controller.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "controller.hpp"
#include <memory>
#include <stdexcept>
#include "backend/helper/reader/image_reader.hpp"
#include "backend/helper/system/system_resources.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"

namespace joda::ctrl {

///
/// \brief
/// \author
/// \return
///
auto Controller::getSystemResources() -> joda::system::SystemResources
{
  return joda::system::acquire();
}

///
/// \brief
/// \author
/// \return
///
Controller::~Controller()
{
  if(mActThread.joinable()) {
    if(mActProcessor) {
      mActProcessor->stop();
    }
    mActThread.join();
  }
}

///
/// \brief
/// \author
/// \return
///
auto Controller::calcOptimalThreadNumber(const settings::AnalyzeSettings &settings) -> joda::thread::ThreadingSettings
{
  joda::thread::ThreadingSettings threads;

  auto ome             = getImageProperties(mWorkingDirectory.gitFirstFile());
  int64_t imgNr        = mWorkingDirectory.getNrOfFiles();
  int64_t tileNr       = 1;
  int64_t pipelineNr   = settings.pipelines.size();
  const auto &props    = ome.getImageInfo();
  auto systemRecources = getSystemResources();

  // Load image in tiles if too big
  const auto &imageInfo = ome.getImageInfo().resolutions.at(0);
  if(imageInfo.imageMemoryUsage > joda::processor::PipelineInitializer::MAX_IMAGE_SIZE_BYTES_TO_LOAD_AT_ONCE) {
    auto [tilesX, tilesY] = imageInfo.getNrOfTiles(joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH,
                                                   joda::processor::PipelineInitializer::COMPOSITE_TILE_HEIGHT);
    tileNr                = static_cast<int64_t>(tilesX) * tilesY;
    threads.ramPerImage   = (imageInfo.bits * joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH *
                           joda::processor::PipelineInitializer::COMPOSITE_TILE_HEIGHT) /
                          8;
  } else {
    tileNr              = 1;
    threads.ramPerImage = imageInfo.imageMemoryUsage;
  }

  if(threads.ramPerImage <= 0) {
    threads.ramPerImage = 1;
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

  // Maximum number of cores depends on the available RAM.)
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
  threads.coresUsed = maxNumberOfCoresToAssign;

  threads.cores[joda::thread::ThreadingSettings::IMAGES]   = 1;
  threads.cores[joda::thread::ThreadingSettings::TILES]    = 1;
  threads.cores[joda::thread::ThreadingSettings::CHANNELS] = 1;

  if(imgNr > tileNr) {
    if(imgNr > pipelineNr) {
      // Image Nr wins
      threads.cores[joda::thread::ThreadingSettings::IMAGES] = maxNumberOfCoresToAssign;
    } else {
      // Channel Nr wins
      threads.cores[joda::thread::ThreadingSettings::CHANNELS] = maxNumberOfCoresToAssign;
    }
  } else {
    if(tileNr > pipelineNr) {
      // Tile nr wins
      threads.cores[joda::thread::ThreadingSettings::TILES] = maxNumberOfCoresToAssign;
    } else {
      // Channel Nr wins
      threads.cores[joda::thread::ThreadingSettings::CHANNELS] = maxNumberOfCoresToAssign;
    }
  }

  threads.totalRuns = imgNr * tileNr * pipelineNr;

  return threads;
}

///
/// \brief
/// \author
/// \return
///
auto Controller::getNrOfFoundImages() -> uint32_t
{
  return mWorkingDirectory.getNrOfFiles();
}

///
/// \brief
/// \author
/// \return
///
auto Controller::getListOfFoundImages() -> const std::map<uint8_t, std::vector<std::filesystem::path>> &
{
  return mWorkingDirectory.getFilesList();
}

///
/// \brief
/// \author
/// \return
///
bool Controller::isLookingForImages()
{
  return mWorkingDirectory.isRunning();
}

///
/// \brief
/// \author
/// \return
///
void Controller::stopLookingForFiles()
{
  mWorkingDirectory.stop();
}

///
/// \brief
/// \author
/// \return
///
void Controller::setWorkingDirectory(uint8_t plateNr, const std::filesystem::path &dir)
{
  mWorkingDirectory.setWorkingDirectory(plateNr, dir);
}

///
/// \brief
/// \author
/// \return
///
void Controller::registerImageLookupCallback(
    const std::function<void(joda::filesystem::State)> &lookingForFilesFinished)
{
  mWorkingDirectory.addListener(lookingForFilesFinished);
}

// PREVIEW ///////////////////////////////////////////////////

void Controller::preview(const settings::ProjectImageSetup &imageSetup, const settings::Pipeline &pipeline,
                         const std::filesystem::path &imagePath, int32_t tileX, int32_t tileY, Preview &previewOut)
{
  processor::Processor process;
  auto [originalImg, previewImage] = process.generatePreview(imageSetup, pipeline, imagePath, 0, 0, tileX, tileY);
  previewOut.originalImage.setImage(std::move(originalImg));
  previewOut.previewImage.setImage(std::move(previewImage));
}

///
/// \brief
/// \author
/// \return
///
auto Controller::getImageProperties(const std::filesystem::path &image, int series) -> joda::ome::OmeInfo
{
  return joda::image::reader::ImageReader::getOmeInformation(image);
}

cv::Size Controller::getCompositeTileSize() const
{
  return {joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH,
          joda::processor::PipelineInitializer::COMPOSITE_TILE_HEIGHT};
}

// FLOW CONTROL ///////////////////////////////////////////////////

///
/// \brief
/// \author
/// \return
///
void Controller::start(const settings::AnalyzeSettings &settings, const joda::thread::ThreadingSettings &threadSettings,
                       const std::string &jobName)
{
  if(!mActThread.joinable()) {
    mActThread = std::thread([this, settings] {
      mActProcessor = std::make_unique<processor::Processor>();
      mActProcessor->execute(settings, mWorkingDirectory);
    });
  } else {
    throw std::runtime_error("There is still a job running. Stop this job first!");
  }
}

///
/// \brief
/// \author
/// \return
///
void Controller::stop()
{
  if(mActProcessor) {
    return mActProcessor->stop();
  }
  throw std::runtime_error("No job running!");
}

///
/// \brief
/// \author
/// \return
///
[[nodiscard]] auto Controller::getState() const -> const joda::processor::ProcessProgress &
{
  if(mActProcessor) {
    return mActProcessor->getProgress();
  }
  throw std::runtime_error("No job running!");
}

///
/// \brief
/// \author
/// \return
///
[[nodiscard]] const processor::ProcessInformation &Controller::getJobInformation() const
{
  if(mActProcessor) {
    return mActProcessor->getJobInformation();
  }
  throw std::runtime_error("No job executed!");
}

}    // namespace joda::ctrl
