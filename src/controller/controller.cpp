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
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include "backend/database/data/dashboard/data_dashboard.hpp"
#include "backend/database/database.hpp"
#include "backend/database/exporter/r/exporter_r.hpp"
#include "backend/database/exporter/xlsx/exporter_xlsx.hpp"
#include "backend/database/query/filter.hpp"
#include "backend/database/query/query_for_image.hpp"
#include "backend/database/query/query_for_well.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/helper/system/system_resources.hpp"
#include "backend/helper/table/table.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/results_settings/results_settings.hpp"
#include "backend/settings/settings.hpp"
#include "ui/gui/helper/template_parser/template_parser.hpp"
#include <nlohmann/json_fwd.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

#undef slots
#include <torch/cuda.h>
#define slots Q_SLOTS

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
/// \param[in]
/// \param[out]
/// \return
///
void Controller::initApplication()
{
  // ======================================
  // Reserve system resources
  // ======================================
  auto systemRecourses   = joda::system::acquire();
  int32_t totalRam       = static_cast<int32_t>(std::ceil(static_cast<float>(systemRecourses.ramTotal) / 1000000.0F));
  int32_t availableRam   = static_cast<int32_t>(std::ceil(static_cast<float>(systemRecourses.ramAvailable) / 1000000.0F));
  int32_t jvmReservedRam = static_cast<int32_t>(std::ceil(static_cast<float>(systemRecourses.ramReservedForJVM) / 1000000.0F));

  // bool cudaAvailable = torch::cuda::is_available();
  // int numCudaDevices = torch::cuda::device_count();

  joda::log::logInfo("Total available RAM " + std::to_string(totalRam) + " MB.");
  joda::log::logInfo("Usable RAM " + std::to_string(availableRam) + " MB.");
  joda::log::logInfo("JVM reserved RAM " + std::to_string(jvmReservedRam) + " MB.");
  // joda::log::logInfo("CUDA available: " + std::to_string(static_cast<int>(cudaAvailable)));
  // joda::log::logInfo("Found CUDA devices: " + std::to_string(numCudaDevices));
  joda::log::logInfo("Global template folder: " + templates::TemplateParser::getGlobalTemplateDirectory("").string());
  joda::log::logInfo("User template folder: " + templates::TemplateParser::getUsersTemplateDirectory().string());

  joda::image::reader::ImageReader::init(systemRecourses.ramReservedForJVM);    // Costs ~50MB RAM
}

///
/// \brief
/// \author
/// \return
///
auto Controller::calcOptimalThreadNumber(const settings::AnalyzeSettings &settings, const std::optional<joda::ome::OmeInfo> &imageOmeInfo)
    -> joda::thread::ThreadingSettings
{
  if(mWorkingDirectory.getNrOfFiles() > 0) {
    return calcOptimalThreadNumber(settings, mWorkingDirectory.gitFirstFile(), static_cast<int32_t>(mWorkingDirectory.getNrOfFiles()), imageOmeInfo);
  }
  return {};
}

///
/// \brief
/// \author
/// \return
///
auto Controller::calcOptimalThreadNumber(const settings::AnalyzeSettings &settings, const std::filesystem::path &file, int nrOfFiles,
                                         const std::optional<joda::ome::OmeInfo> &imageOmeInfo) -> joda::thread::ThreadingSettings
{
  joda::thread::ThreadingSettings threads;

  joda::ome::OmeInfo ome;
  if(!imageOmeInfo.has_value()) {
    ome = getImageProperties(file, settings.imageSetup.series, settings.imageSetup.imagePixelSizeSettings);
  } else {
    ome = imageOmeInfo.value();
  }
  int64_t imgNr      = nrOfFiles;
  int64_t tileNr     = 1;
  int64_t pipelineNr = static_cast<int64_t>(settings.pipelines.size());
  // const auto &props    = ome.getImageInfo(settings.imageSetup.series);
  auto systemRecourses = getSystemResources();

  // Load image in tiles if too big
  const auto &imageInfo = ome.getImageInfo(settings.imageSetup.series).resolutions.at(0);

  bool canLoadTiles =
      (imageInfo.optimalTileHeight <= settings.imageSetup.imageTileSettings.tileHeight && imageInfo.optimalTileWidth <= imageInfo.imageWidth);
  if(canLoadTiles && (imageInfo.imageWidth > settings.imageSetup.imageTileSettings.tileWidth ||
                      imageInfo.imageHeight > settings.imageSetup.imageTileSettings.tileHeight)) {
    auto [tilesX, tilesY] = imageInfo.getNrOfTiles(settings.imageSetup.imageTileSettings.tileWidth, settings.imageSetup.imageTileSettings.tileHeight);
    tileNr                = static_cast<int64_t>(tilesX) * tilesY;
    threads.ramPerImage   = static_cast<uint64_t>((imageInfo.rgbChannelCount * imageInfo.bits * settings.imageSetup.imageTileSettings.tileWidth *
                                                 settings.imageSetup.imageTileSettings.tileHeight) /
                                                8);
  } else {
    tileNr              = 1;
    threads.ramPerImage = static_cast<uint64_t>(imageInfo.imageMemoryUsage);
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
      static_cast<int32_t>(std::min(static_cast<uint64_t>(systemRecourses.cpus), static_cast<uint64_t>(threads.ramFree / threads.ramPerImage)));
  if(maxNumberOfCoresToAssign <= 0) {
    maxNumberOfCoresToAssign = 1;
  }
  if(maxNumberOfCoresToAssign > 1 && maxNumberOfCoresToAssign == static_cast<int32_t>(systemRecourses.cpus)) {
    // Don't use all CPU cores if there are more than 1
    maxNumberOfCoresToAssign--;
  }
  threads.coresUsed = static_cast<uint32_t>(maxNumberOfCoresToAssign);

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

  threads.totalRuns = static_cast<uint64_t>(imgNr * tileNr * pipelineNr);

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
  return static_cast<uint32_t>(mWorkingDirectory.getNrOfFiles());
}

///
/// \brief
/// \author
/// \return
///
auto Controller::getListOfFoundImages() const -> const std::vector<std::filesystem::path> &
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
void Controller::setWorkingDirectory(const std::filesystem::path &dir)
{
  mWorkingDirectory.setWorkingDirectory(dir);
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
                         const settings::Pipeline &pipeline, const std::filesystem::path &imagePath, int32_t tileX, int32_t tileY, int32_t tStack,
                         Preview &previewOut, const joda::ome::OmeInfo &ome)
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
  auto [originalImg, editedImageWithoutOverlay, thumb, validity, objects] = process.generatePreview(
      previewSettings, imageSetup, settings, threadSettings, pipeline, imagePath, tStack, 0, tileX, tileY, generateThumb, ome);
  previewOut.originalImage.setImage(std::move(originalImg));
  previewOut.editedImage.setImage(std::move(editedImageWithoutOverlay));
  if(generateThumb) {
    previewOut.thumbnail.setImage(std::move(thumb));
  }
  previewOut.results.objectMap     = std::move(objects);
  previewOut.results.noiseDetected = validity.test(enums::ChannelValidityEnum::POSSIBLE_NOISE);
  previewOut.results.isOverExposed = validity.test(enums::ChannelValidityEnum::POSSIBLE_WRONG_THRESHOLD);
  previewOut.tStacks               = ome.getNrOfTStack(imageSetup.series);
}

///
/// \brief
/// \author
/// \return
///
auto Controller::loadImage(const std::filesystem::path &imagePath, uint16_t series, const joda::image::reader::ImageReader::Plane &imagePlane,
                           const joda::ome::TileToLoad &tileLoad,
                           const joda::settings::ProjectImageSetup::PhysicalSizeSettings &defaultPhysicalSizeSettings, Preview &previewOut,
                           joda::ome::OmeInfo &omeOut, enums::ZProjection zProjection) -> void
{
  static std::filesystem::path lastImagePath;

  if(lastImagePath != imagePath) {
    lastImagePath          = imagePath;
    ome::PhyiscalSize phys = {};
    if(defaultPhysicalSizeSettings.mode == enums::PhysicalSizeMode::Manual) {
      phys = joda::ome::PhyiscalSize{static_cast<double>(defaultPhysicalSizeSettings.pixelWidth),
                                     static_cast<double>(defaultPhysicalSizeSettings.pixelHeight), 0, defaultPhysicalSizeSettings.pixelSizeUnit};
    }
    omeOut = joda::image::reader::ImageReader::getOmeInformation(imagePath, series, phys);
  }
  loadImage(imagePath, series, imagePlane, tileLoad, previewOut, &omeOut, zProjection);
}

///
/// \brief
/// \author
/// \return
///
auto Controller::loadImage(const std::filesystem::path &imagePath, uint16_t series, const joda::image::reader::ImageReader::Plane &imagePlane,
                           const joda::ome::TileToLoad &tileLoad, Preview &previewOut, const joda::ome::OmeInfo *omeIn,
                           enums::ZProjection zProjection) -> void
{
  if(nullptr == omeIn) {
    return;
  }
  static std::filesystem::path lastImagePath;
  static joda::image::reader::ImageReader::Plane lastImagePlane = {-1, -1, -1};
  static joda::ome::TileToLoad lastImageTile                    = {-1};
  static int32_t lastImageSeries                                = -1;
  static enums::ZProjection lastZProjection                     = enums::ZProjection::UNDEFINED;
  bool generateThumb                                            = false;
  bool refreshImage                                             = false;

  if(imagePath != lastImagePath || previewOut.thumbnail.empty() || lastImagePlane != imagePlane || lastImageTile != tileLoad ||
     lastImageSeries != series || zProjection != lastZProjection) {
    lastImageSeries = series;
    lastImagePath   = imagePath;
    generateThumb   = true;
    refreshImage    = true;
    lastImagePlane  = imagePlane;
    lastImageTile   = tileLoad;
    lastZProjection = zProjection;
  }

  if(refreshImage) {
    auto loadImageTile = [&tileLoad, series, &omeIn, &imagePath](int32_t z, int32_t c, int32_t t) {
      return joda::image::reader::ImageReader::loadImageTile(imagePath.string(), joda::image::reader::ImageReader::Plane{.z = z, .c = c, .t = t},
                                                             series, 0, tileLoad, *omeIn);
    };

    //
    // Do z -projection if activated
    //
    int32_t c  = imagePlane.c;
    int32_t z  = imagePlane.z;
    int32_t t  = imagePlane.t;
    auto image = loadImageTile(z, c, t);

    if(zProjection != enums::ZProjection::NONE && zProjection != enums::ZProjection::TAKE_MIDDLE) {
      auto max = [&loadImageTile, &image, c, t](int zIdx) { image = cv::max(image, loadImageTile(zIdx, c, t)); };
      auto min = [&loadImageTile, &image, c, t](int zIdx) { image = cv::min(image, loadImageTile(zIdx, c, t)); };
      auto avg = [&loadImageTile, &image, c, t](int zIdx) {
        auto tmp = loadImageTile(zIdx, c, t);
        tmp.convertTo(tmp, CV_32SC1);
        image = image + tmp;
      };

      std::function<void(int)> func;
      auto imageType = image.type();

      switch(zProjection) {
        case enums::ZProjection::MAX_INTENSITY:
          func = max;
          break;
        case enums::ZProjection::MIN_INTENSITY:
          func = min;
          break;
        case enums::ZProjection::AVG_INTENSITY:
          image.convertTo(image, CV_32SC1);    // Need to scale up because we are adding a lot of images to avoid overflow
          func = avg;
          break;
        case enums::ZProjection::NONE:
        case enums::ZProjection::$:
        case enums::ZProjection::UNDEFINED:
        case enums::ZProjection::TAKE_MIDDLE:
          break;
      }

      for(uint32_t zIdx = 1; zIdx < static_cast<uint32_t>(omeIn->getNrOfZStack(series)); zIdx++) {
        func(static_cast<int32_t>(zIdx));
      }
      // Avg intensity projection
      if(enums::ZProjection::AVG_INTENSITY == zProjection) {
        image = image / omeIn->getNrOfZStack(series);
        image.convertTo(image, imageType);    // no scaling
      }
    }

    previewOut.originalImage.setImage(std::move(image));
  }

  if(generateThumb) {
    auto thumb = joda::image::reader::ImageReader::loadThumbnail(imagePath.string(), imagePlane, series, *omeIn);
    previewOut.thumbnail.setImage(std::move(thumb));
  }

  previewOut.tStacks = omeIn->getNrOfTStack(series);
}

///
/// \brief
/// \author
/// \return
///
auto Controller::getImageProperties(const std::filesystem::path &image, int series,
                                    const joda::settings::ProjectImageSetup::PhysicalSizeSettings &defaultPhysicalSizeSettings) -> joda::ome::OmeInfo
{
  ome::PhyiscalSize phys = {};
  if(defaultPhysicalSizeSettings.mode == enums::PhysicalSizeMode::Manual) {
    phys = joda::ome::PhyiscalSize{static_cast<double>(defaultPhysicalSizeSettings.pixelWidth),
                                   static_cast<double>(defaultPhysicalSizeSettings.pixelHeight), 0, defaultPhysicalSizeSettings.pixelSizeUnit};
  }
  return joda::image::reader::ImageReader::getOmeInformation(image, static_cast<uint16_t>(series), phys);
}

// FLOW CONTROL ////////////////////////////////////////////////
/// \brief
/// \author
/// \return
///
void Controller::start(const settings::AnalyzeSettings &settings, const joda::thread::ThreadingSettings & /*threadSettings*/,
                       const std::string &jobName)
{
  if(mActThread.joinable()) {
    mActThread.join();
  }
  setWorkingDirectory(settings.projectSettings.plate.imageFolder);
  mWorkingDirectory.waitForFinished();

  mActProcessor.reset();
  mActProcessor = std::make_unique<processor::Processor>();
  mActThread    = std::thread([this, settings, jobName] {
    mActProcessor->execute(
        settings, jobName,
        calcOptimalThreadNumber(settings, mWorkingDirectory.gitFirstFile(), static_cast<int32_t>(mWorkingDirectory.getNrOfFiles()), std::nullopt),
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
  enums::ClassId actClass = enums::ClassId::C1;
  int colorIdx            = 0;
  for(const auto &[_, channelIn] : channels) {
    auto addSubClass = [&actClass, &classes, &templatePerType](const std::string &channel, const std::string &subclass, const std::string &color) {
      std::vector<settings::ResultsTemplate> measurements;
      if(templatePerType.contains(subclass)) {
        measurements = templatePerType.at(subclass);
      }

      classes.classes.push_back(joda::settings::Class{
          .classId = actClass, .name = channel + "@" + subclass, .color = color, .notes = "", .defaultMeasurements = measurements});
      actClass = static_cast<enums::ClassId>((static_cast<uint16_t>(actClass)) + 1);
    };
    // auto waveLength         = channel.emissionWaveLength;
    auto channelName        = helper::toLower(channelIn.name);
    std::string color       = joda::settings::COLORS.at(static_cast<uint64_t>(colorIdx) % joda::settings::COLORS.size());
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
void Controller::exportData(const std::filesystem::path &pathToDbFile, const joda::exporter::xlsx::ExportSettings &settings,
                            const std::filesystem::path &outputFilePath, const std::optional<std::list<joda::settings::Class>> &classesList)
{
  auto analyzer = std::make_unique<joda::db::Database>();
  analyzer->openDatabase(std::filesystem::path(pathToDbFile.string()));
  auto experiment = analyzer->selectExperiment();

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

  uint64_t groupId = 0;
  if(!settings.filter.groupName.empty()) {
    groupId = analyzer->selectGroupIdFromGroupName(settings.filter.groupName);
    if(groupId == UINT16_MAX) {
      throw std::invalid_argument("Group with name >" + settings.filter.groupName + "< not found in database!");
    }
  }

  // Filter options
  settings::ResultsSettings filter;
  if(!classesList.has_value()) {
    // Use default settings
    filter = nlohmann::json::parse(analyzer->selectResultsTableSettings(experiment.jobId));
  } else {
    // Generate settings
    filter = joda::settings::Settings::toResultsSettings(
        joda::settings::Settings::ResultSettingsInput{.classes             = {classesList->begin(), classesList->end()},
                                                      .outputClasses       = analyzer->selectOutputClasses(),
                                                      .intersectingClasses = analyzer->selectIntersectingClassForClasses(),
                                                      .measuredChannels    = analyzer->selectMeasurementChannelsForClasses(),
                                                      .distanceFromClasses = analyzer->selectDistanceClassForClasses()});
  }
  filter.setFilter(static_cast<uint8_t>(settings.filter.plateId), static_cast<uint16_t>(groupId), settings.filter.tStack, {imageId});

  joda::log::logInfo("Export started!");
  auto grouping = db::StatsPerGroup::Grouping::BY_IMAGE;
  joda::table::Table dataToExport;
  int32_t imgWidth  = 0;
  int32_t imgHeight = 0;
  switch(settings.view) {
    case exporter::xlsx::ExportSettings::ExportView::PLATE: {
      grouping     = db::StatsPerGroup::Grouping::BY_PLATE;
      dataToExport = joda::db::StatsPerGroup::toTable(analyzer.get(), filter, grouping);
    } break;
    case exporter::xlsx::ExportSettings::ExportView::WELL:
      grouping     = db::StatsPerGroup::Grouping::BY_WELL;
      dataToExport = joda::db::StatsPerGroup::toTable(analyzer.get(), filter, grouping);
      break;
    case exporter::xlsx::ExportSettings::ExportView::IMAGE:
      auto image   = analyzer->selectImageInfo(imageId);
      imgWidth     = static_cast<int32_t>(image.width);
      imgHeight    = static_cast<int32_t>(image.height);
      grouping     = db::StatsPerGroup::Grouping::BY_IMAGE;
      dataToExport = joda::db::StatsPerImage::toTable(analyzer.get(), filter);
      break;
  }

  settings::AnalyzeSettings analyzeSettings = nlohmann::json::parse(experiment.analyzeSettingsJsonString);

  if(settings.format == exporter::xlsx::ExportSettings::ExportFormat::XLSX) {
    if(exporter::xlsx::ExportSettings::ExportStyle::LIST == settings.style) {
      joda::db::data::Dashboard dashboard;
      auto colocClasses    = analyzer->selectColocalizingClasses();
      auto tmp             = std::shared_ptr<joda::table::Table>(&dataToExport, [](joda::table::Table *) { /* no delete */ });
      auto retValDashboard = dashboard.convert(tmp, colocClasses, settings.view == exporter::xlsx::ExportSettings::ExportView::IMAGE);
      std::vector<const exporter::Exportable *> retVal;
      retVal.reserve(retValDashboard.size());
      for(const auto &[_, data] : retValDashboard) {
        retVal.emplace_back(data.get());
      }
      joda::exporter::xlsx::Exporter::startExport(retVal, analyzeSettings, experiment.jobName, experiment.timestampStart, experiment.timestampFinish,
                                                  experiment.physicalPixelSizeUnit, outputFilePath.string());

    } else if(exporter::xlsx::ExportSettings::ExportStyle::HEATMAP == settings.style) {
      joda::exporter::xlsx::Exporter::startHeatmapExport({&dataToExport}, analyzeSettings, experiment.jobName, experiment.timestampStart,
                                                         experiment.timestampFinish, outputFilePath.string(), filter, settings.view, imgHeight,
                                                         imgWidth, experiment.physicalPixelSizeUnit);
    }
  } else {
    joda::db::data::Dashboard dashboard;
    auto colocClasses    = analyzer->selectColocalizingClasses();
    auto tmp             = std::shared_ptr<joda::table::Table>(&dataToExport, [](joda::table::Table *) { /* no delete */ });
    auto retValDashboard = dashboard.convert(tmp, colocClasses, settings.view == exporter::xlsx::ExportSettings::ExportView::IMAGE);
    std::vector<const exporter::Exportable *> retVal;
    retVal.reserve(retValDashboard.size());
    for(const auto &[_, data] : retValDashboard) {
      retVal.emplace_back(data.get());
    }
    joda::exporter::r::Exporter::startExport(retVal, analyzeSettings, experiment.jobName, experiment.timestampStart, experiment.timestampFinish,
                                             experiment.physicalPixelSizeUnit, outputFilePath.string());
  }
  joda::log::logInfo("Export finished!");
}

}    // namespace joda::ctrl
