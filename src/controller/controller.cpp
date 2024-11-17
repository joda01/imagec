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
#include <exception>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/helper/system/system_resources.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include "backend/settings/analze_settings.hpp"

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
  return calcOptimalThreadNumber(settings, mWorkingDirectory.gitFirstFile(), mWorkingDirectory.getNrOfFiles());
}

///
/// \brief
/// \author
/// \return
///
auto Controller::calcOptimalThreadNumber(const settings::AnalyzeSettings &settings, const std::filesystem::path &file, int nrOfFiles)
    -> joda::thread::ThreadingSettings
{
  joda::thread::ThreadingSettings threads;

  auto ome             = getImageProperties(file);
  int64_t imgNr        = nrOfFiles;
  int64_t tileNr       = 1;
  int64_t pipelineNr   = settings.pipelines.size();
  const auto &props    = ome.getImageInfo();
  auto systemRecourses = getSystemResources();

  // Load image in tiles if too big
  const auto &imageInfo = ome.getImageInfo().resolutions.at(0);

  bool canLoadTiles =
      (imageInfo.optimalTileHeight <= settings.imageSetup.imageTileSettings.tileHeight && imageInfo.optimalTileWidth <= imageInfo.imageWidth);
  if(canLoadTiles && (imageInfo.imageWidth > settings.imageSetup.imageTileSettings.tileWidth ||
                      imageInfo.imageHeight > settings.imageSetup.imageTileSettings.tileHeight)) {
    auto [tilesX, tilesY] = imageInfo.getNrOfTiles(settings.imageSetup.imageTileSettings.tileWidth, settings.imageSetup.imageTileSettings.tileHeight);
    tileNr                = static_cast<int64_t>(tilesX) * tilesY;
    threads.ramPerImage   = (imageInfo.rgbChannelCount * imageInfo.bits * settings.imageSetup.imageTileSettings.tileWidth *
                           settings.imageSetup.imageTileSettings.tileHeight) /
                          8;
  } else {
    tileNr              = 1;
    threads.ramPerImage = imageInfo.imageMemoryUsage;
  }

  if(threads.ramPerImage <= 0) {
    threads.ramPerImage = 1;
  }
  threads.ramFree        = std::min(systemRecourses.ramAvailable, systemRecourses.ramReservedForJVM);
  threads.ramTotal       = systemRecourses.ramTotal;
  threads.coresAvailable = systemRecourses.cpus;

  // No multi threading when AI is used, sinze AI is still using all cPUs
  // for(const auto &ch : settings.getChannelsVector()) {
  //  if(ch.getDetectionSettings().getDetectionMode() ==
  //  settings::json::ChannelDetection::DetectionMode::AI) {
  //    // return threads;
  //  }
  //}

  // Maximum number of cores depends on the available RAM.)
  int32_t maxNumberOfCoresToAssign =
      std::min(static_cast<uint64_t>(systemRecourses.cpus), static_cast<uint64_t>(threads.ramFree / threads.ramPerImage));
  if(maxNumberOfCoresToAssign <= 0) {
    maxNumberOfCoresToAssign = 1;
  }
  if(maxNumberOfCoresToAssign > 1 && maxNumberOfCoresToAssign == systemRecourses.cpus) {
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

  std::cout << "Calculated threads " << std::to_string(imageInfo.optimalTileHeight) << "x" << std::to_string(imageInfo.optimalTileWidth) << " | "
            << std::to_string((float) threads.ramPerImage / 1000000.0f) << " MB "
            << " | " << std::to_string(threads.coresUsed) << std::endl;

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
void Controller::registerImageLookupCallback(const std::function<void(joda::filesystem::State)> &lookingForFilesFinished)
{
  mWorkingDirectory.addListener(lookingForFilesFinished);
}

// PREVIEW ///////////////////////////////////////////////////

void Controller::preview(const settings::ProjectImageSetup &imageSetup, const processor::PreviewSettings &previewSettings,
                         const settings::AnalyzeSettings &settings, const settings::Pipeline &pipeline, const std::filesystem::path &imagePath,
                         int32_t tileX, int32_t tileY, Preview &previewOut, const joda::ome::OmeInfo &ome,
                         const settings::ObjectInputClusters &clustersClassesToShow)
{
  static std::filesystem::path lastImagePath;
  bool generateThumb = false;
  if(imagePath != lastImagePath || previewOut.thumbnail.empty()) {
    lastImagePath = imagePath;
    generateThumb = true;
  }

  processor::Processor process;
  auto [originalImg, previewImage, thumb, foundObjects] = process.generatePreview(previewSettings, imageSetup, settings, pipeline, imagePath, 0, 0,
                                                                                  tileX, tileY, generateThumb, ome, clustersClassesToShow);
  previewOut.originalImage.setImage(std::move(originalImg));
  previewOut.previewImage.setImage(std::move(previewImage));
  if(generateThumb) {
    previewOut.thumbnail.setImage(std::move(thumb));
  }
  previewOut.foundObjects.clear();
  for(const auto &[key, val] : foundObjects) {
    previewOut.foundObjects[key].color = val.color;
    previewOut.foundObjects[key].count = val.count;
  }
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

// FLOW CONTROL ///////////////////////////////////////////////////

///
/// \brief
/// \author
/// \return
///
void Controller::start(const settings::AnalyzeSettings &settings, const joda::thread::ThreadingSettings &threadSettings, const std::string &jobName)
{
  if(mActThread.joinable()) {
    mActThread.join();
  }
  mActProcessor.reset();
  mActThread = std::thread([this, settings, jobName] {
    mActProcessor = std::make_unique<processor::Processor>();
    mActProcessor->execute(settings, jobName, calcOptimalThreadNumber(settings, mWorkingDirectory.gitFirstFile(), mWorkingDirectory.getNrOfFiles()),
                           mWorkingDirectory);
  });
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
