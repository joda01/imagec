///
/// \file      controller.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "controller.hpp"
#include <exception>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/database/exporter/r/exporter_r.hpp"
#include "backend/helper/database/exporter/xlsx/exporter.hpp"
#include "backend/helper/database/plugins/control_image.hpp"
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/helper/database/plugins/stats_for_image.hpp"
#include "backend/helper/database/plugins/stats_for_well.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/helper/system/system_resources.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include <nlohmann/json_fwd.hpp>
#include <opencv2/core/mat.hpp>

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
  if(mWorkingDirectory.getNrOfFiles() > 0) {
    return calcOptimalThreadNumber(settings, mWorkingDirectory.gitFirstFile(), mWorkingDirectory.getNrOfFiles());
  }
  return {};
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

  auto ome             = getImageProperties(file, settings.imageSetup.series);
  int64_t imgNr        = nrOfFiles;
  int64_t tileNr       = 1;
  int64_t pipelineNr   = settings.pipelines.size();
  const auto &props    = ome.getImageInfo(settings.imageSetup.series);
  auto systemRecourses = getSystemResources();

  // Load image in tiles if too big
  const auto &imageInfo = ome.getImageInfo(settings.imageSetup.series).resolutions.at(0);

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

  /* joda::log::logInfo("Calculated threads " + std::to_string(imageInfo.optimalTileHeight) + "x" + std::to_string(imageInfo.optimalTileWidth) + " | "
     + std::to_string((float) threads.ramPerImage / 1000000.0f) + " MB " + " | " + std::to_string(threads.coresUsed));*/

  joda::log::logInfo("Number of CPU cores to use: " + std::to_string(threads.coresUsed));
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
                         const settings::AnalyzeSettings &settings, const joda::thread::ThreadingSettings &threadSettings,
                         const settings::Pipeline &pipeline, const std::filesystem::path &imagePath, int32_t tileX, int32_t tileY,
                         Preview &previewOut, const joda::ome::OmeInfo &ome, const settings::ObjectInputClasses &classesToShow)
{
  static std::filesystem::path lastImagePath;
  static int32_t lastImageChannel = -1;
  static int32_t lastImageSeries  = -1;
  bool generateThumb              = false;
  if(imagePath != lastImagePath || previewOut.thumbnail.empty() || lastImageChannel != pipeline.pipelineSetup.cStackIndex ||
     lastImageSeries != imageSetup.series) {
    lastImageSeries  = imageSetup.series;
    lastImagePath    = imagePath;
    generateThumb    = true;
    lastImageChannel = pipeline.pipelineSetup.cStackIndex;
  }

  processor::Processor process;
  auto [originalImg, overlay, editedImageWithoutOverlay, thumb, foundObjects, validity] = process.generatePreview(
      previewSettings, imageSetup, settings, threadSettings, pipeline, imagePath, 0, 0, tileX, tileY, generateThumb, ome, classesToShow);
  previewOut.originalImage.setImage(std::move(originalImg));
  previewOut.overlay.setImage(std::move(overlay));
  previewOut.editedImage.setImage(std::move(editedImageWithoutOverlay));
  if(generateThumb) {
    previewOut.thumbnail.setImage(std::move(thumb));
  }
  previewOut.results.foundObjects.clear();
  for(const auto &[key, val] : foundObjects) {
    previewOut.results.foundObjects[key].color = val.color;
    previewOut.results.foundObjects[key].count = val.count;
  }
  previewOut.results.noiseDetected = validity.test(enums::ChannelValidityEnum::POSSIBLE_NOISE);
  previewOut.results.isOverExposed = validity.test(enums::ChannelValidityEnum::POSSIBLE_WRONG_THRESHOLD);
}

///
/// \brief
/// \author
/// \return
///
auto Controller::loadImage(const std::filesystem::path &imagePath, uint16_t series, const joda::image::reader::ImageReader::Plane &imagePlane,
                           const joda::ome::TileToLoad &tileLoad, Preview &previewOut, joda::ome::OmeInfo &omeOut, int32_t markerX, int32_t markerY)
    -> void
{
  static std::filesystem::path lastImagePath;
  static int32_t lastImageChannel = -1;
  static int32_t lastImageSeries  = -1;
  bool generateThumb              = false;
  omeOut                          = joda::image::reader::ImageReader::getOmeInformation(imagePath, series);
  if(imagePath != lastImagePath || previewOut.thumbnail.empty() || lastImageChannel != imagePlane.c || lastImageSeries != series) {
    lastImageSeries  = series;
    lastImagePath    = imagePath;
    generateThumb    = true;
    lastImageChannel = imagePlane.c;
  }

  auto originalImg = joda::image::reader::ImageReader::loadImageTile(imagePath.string(), imagePlane, series, 0, tileLoad, omeOut);
  cv::Mat overlay  = cv::Mat::zeros(originalImg.rows, originalImg.cols, CV_8UC3);

  //
  // Generate overlay
  //
  auto drawCrosshair = [](cv::Mat &image, int x, int y, int size = 10, cv::Scalar color = cv::Scalar(0, 255, 0), int thickness = 2) {
    // Draw horizontal line across full width
    cv::line(image, cv::Point(0, y), cv::Point(image.cols - 1, y), color, thickness);

    // Draw vertical line across full height
    cv::line(image, cv::Point(x, 0), cv::Point(x, image.rows - 1), color, thickness);
  };
  drawCrosshair(overlay, markerX, markerY);

  //
  // Set original image
  //
  auto editedImage = originalImg.clone();
  previewOut.originalImage.setImage(std::move(originalImg));
  previewOut.overlay.setImage(std::move(overlay));
  previewOut.editedImage.setImage(std::move(editedImage));
  if(generateThumb) {
    auto thumb = joda::image::reader::ImageReader::loadThumbnail(imagePath.string(), imagePlane, series, omeOut);
    previewOut.thumbnail.setImage(std::move(thumb));
  }
  previewOut.results.foundObjects.clear();
}

///
/// \brief
/// \author
/// \return
///
auto Controller::getImageProperties(const std::filesystem::path &image, int series) -> joda::ome::OmeInfo
{
  return joda::image::reader::ImageReader::getOmeInformation(image, series);
}

// FLOW CONTROL ////////////////////////////////////////////////
/// \brief
/// \author
/// \return
///
void Controller::start(const settings::AnalyzeSettings &settings, const joda::thread::ThreadingSettings &threadSettings, const std::string &jobName)
{
  if(mActThread.joinable()) {
    mActThread.join();
  }
  setWorkingDirectory(settings.projectSettings.plates.begin()->plateId, settings.projectSettings.plates.begin()->imageFolder);
  mWorkingDirectory.waitForFinished();

  mActProcessor.reset();
  mActProcessor = std::make_unique<processor::Processor>();
  mActThread    = std::thread([this, settings, jobName] {
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

///
/// \brief  Extract classes from image OME info
/// \author
/// \return
///
auto Controller::populateClassesFromImage(const joda::ome::OmeInfo &omeInfo, int32_t series) -> joda::settings::Classification
{
  std::map<std::string, std::vector<settings::ResultsTemplate>> templatePerType = {
      {"spot",
       std::vector<settings::ResultsTemplate>{
           {settings::ResultsTemplate{.measureChannel = enums::Measurement::COUNT, .stats = {enums::Stats::OFF}}},
           {settings::ResultsTemplate{.measureChannel = enums::Measurement::INTENSITY_AVG, .stats = {enums::Stats::SUM, enums::Stats::AVG}}},
           {settings::ResultsTemplate{.measureChannel = enums::Measurement::INTENSITY_SUM, .stats = {enums::Stats::SUM, enums::Stats::AVG}}},
           {settings::ResultsTemplate{.measureChannel = enums::Measurement::AREA_SIZE, .stats = {enums::Stats::SUM, enums::Stats::AVG}}}}},
      {"nucleus", std::vector<settings::ResultsTemplate>{{settings::ResultsTemplate{.measureChannel = enums::Measurement::COUNT,
                                                                                    .stats          = {enums::Stats::OFF}}}}},
      {"cell", std::vector<settings::ResultsTemplate>{
                   {settings::ResultsTemplate{.measureChannel = enums::Measurement::COUNT, .stats = {enums::Stats::OFF}}},
                   {settings::ResultsTemplate{.measureChannel = enums::Measurement::INTERSECTING, .stats = {enums::Stats::OFF}}},
                   {settings::ResultsTemplate{.measureChannel = enums::Measurement::INTENSITY_AVG, .stats = {enums::Stats::SUM, enums::Stats::AVG}}},
                   {settings::ResultsTemplate{.measureChannel = enums::Measurement::INTENSITY_SUM, .stats = {enums::Stats::SUM, enums::Stats::AVG}}},
                   {settings::ResultsTemplate{.measureChannel = enums::Measurement::AREA_SIZE, .stats = {enums::Stats::SUM, enums::Stats::AVG}}}}}};

  const std::map<std::string, std::pair<std::string, std::string>> channelNameToColorMap = {
      {"dapi", {"#3399FF", "nucleus"}},    {"cfp", {"#33FFD1", "spot"}}, /*{"dpss", {"#??", "spot"}},*/ {"fitc", {"#66FF33", "spot"}},
      {"gfp", {"#33CFFF", "spot"}},        {"yfp", {"#5EFF00", "spot"}}, {"cy3", {"#D2FF00", "spot"}},
      {"tritc", {"#F6FF00", "spot"}},      {"rfp", {"#FFD200", "spot"}}, {"mcherry", {"#FF9B00", "spot"}},
      {"cy5", {"#FF3366", "spot"}},        {"cy7", {"#640000", "spot"}}, {"bf", {"#FFFF33", "cell"}},
      {"brightfield", {"#FFFF33", "cell"}}};

  joda::settings::Classification classes;
  auto channels           = omeInfo.getChannelInfos(series);
  enums::ClassId actClass = enums::ClassId::C0;
  int colorIdx            = 0;
  for(const auto &[_, channel] : channels) {
    auto addSubClass = [&actClass, &classes, &templatePerType](const std::string &channel, const std::string &subclass, const std::string &color) {
      std::vector<settings::ResultsTemplate> measurements;
      if(templatePerType.contains(subclass)) {
        measurements = templatePerType.at(subclass);
      }

      classes.classes.push_back(joda::settings::Class{
          .classId = actClass, .name = channel + "@" + subclass, .color = color, .notes = "", .defaultMeasurements = measurements});
      actClass = static_cast<enums::ClassId>((static_cast<uint16_t>(actClass)) + 1);
    };
    auto waveLength         = channel.emissionWaveLength;
    auto channelName        = helper::toLower(channel.name);
    std::string color       = joda::settings::COLORS.at(colorIdx % joda::settings::COLORS.size());
    std::string channelType = "spot";
    // Try to find the best matching
    for(const auto &[channelFluorophore, data] : channelNameToColorMap) {
      auto found = channelName.find(channelFluorophore);
      if(found != std::string::npos) {
        color       = data.first;
        channelType = data.second;
        break;
      }
    }

    addSubClass(channelName, channelType, color);
  }

  return classes;
}

///
/// \brief  Export data
/// \todo Allow multiple image Ids
/// \author
/// \return
///
void Controller::exportData(const std::filesystem::path &pathToDbFile, settings::ResultsSettings &filter, const ExportSettings &settings,
                            const std::filesystem::path &outputFilePath)
{
  auto analyzer = std::make_unique<joda::db::Database>();
  analyzer->openDatabase(std::filesystem::path(pathToDbFile.string()));

  if(outputFilePath.empty()) {
    return;
  }
  uint64_t imageId = 0;
  if(!settings.filter.imageFileName.empty()) {
    imageId = analyzer->selectImageIdFromImageFileName(settings.filter.imageFileName);
    if(imageId == 0) {
      throw std::invalid_argument("Image with name >" + settings.filter.imageFileName + "< not found in database!");
    }
  }
  filter.setFilter(settings.filter.plateId, settings.filter.groupId, {imageId});

  joda::log::logInfo("Export started!");
  auto grouping = db::StatsPerGroup::Grouping::BY_IMAGE;
  std::map<int32_t, joda::table::Table> dataToExport;
  switch(settings.view) {
    case ExportSettings::ExportView::PLATE: {
      grouping = db::StatsPerGroup::Grouping::BY_PLATE;
      if(ExportSettings::ExportFormat::LIST == settings.format) {
        dataToExport = joda::db::StatsPerGroup::toTable(analyzer.get(), filter, grouping);
      } else {
        dataToExport = joda::db::StatsPerGroup::toHeatmap(analyzer.get(), filter, grouping);
      }
    } break;
    case ExportSettings::ExportView::WELL:
      grouping = db::StatsPerGroup::Grouping::BY_WELL;
      if(ExportSettings::ExportFormat::LIST == settings.format) {
        dataToExport = joda::db::StatsPerGroup::toTable(analyzer.get(), filter, grouping);
      } else {
        dataToExport = joda::db::StatsPerGroup::toHeatmap(analyzer.get(), filter, grouping);
      }
      break;
    case ExportSettings::ExportView::IMAGE:
      grouping = db::StatsPerGroup::Grouping::BY_IMAGE;
      if(ExportSettings::ExportFormat::LIST == settings.format) {
        dataToExport = joda::db::StatsPerImage::toTable(analyzer.get(), filter);
      } else {
        dataToExport = joda::db::StatsPerImage::toHeatmap(analyzer.get(), filter);
      }
      break;
  }

  auto experiment                           = analyzer->selectExperiment();
  settings::AnalyzeSettings analyzeSettings = nlohmann::json::parse(experiment.analyzeSettingsJsonString);

  if(settings.type == ExportSettings::ExportType::XLSX) {
    if(ExportSettings::ExportFormat::HEATMAP == settings.format) {
      joda::db::BatchExporter::startExportHeatmap(dataToExport, analyzeSettings, experiment.jobName, experiment.timestampStart,
                                                  experiment.timestampFinish, outputFilePath.string());
    } else {
      joda::db::BatchExporter::startExportList(dataToExport, analyzeSettings, experiment.jobName, experiment.timestampStart,
                                               experiment.timestampFinish, outputFilePath.string());
    }
  } else {
    joda::db::RExporter::startExport(filter, grouping, analyzeSettings, experiment.jobName, experiment.timestampStart, experiment.timestampFinish,
                                     outputFilePath.string());
  }
  joda::log::logInfo("Export finished!");
}

}    // namespace joda::ctrl
