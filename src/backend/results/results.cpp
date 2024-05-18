///
/// \file      results.cpp
/// \author    Joachim Danmayr
/// \date      2024-05-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "results.hpp"
#include <regex.h>
#include <exception>
#include <memory>
#include <regex>
#include <stdexcept>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/uuid.hpp"
#include "backend/image_processing/reader/tif/image_loader_tif.hpp"
#include "backend/pipelines/processor/image_processor.hpp"
#include "backend/results/database/database.hpp"

namespace joda::results {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Results::Results(const std::filesystem::path &resultsFolder, const ExperimentSetting &settings) :
    mExperimentSettings(settings), mJobId(joda::helper::generate_uuid())
{
  prepareOutputFolders(resultsFolder);
  mDatabase = std::make_shared<db::Database>(mDatabaseFileName);
  mDatabase->open();
  try {
    mDatabase->createJob(db::JobMeta{.experimentId = settings.experimentId,
                                     .jobId        = mJobId,
                                     .name         = settings.jobName,
                                     .scientists   = {settings.scientistName},
                                     .location     = "",
                                     .notes        = ""});
  } catch(const std::exception &ex) {
    std::cout << mJobId << " | " << settings.experimentId << std::endl;
    joda::log::logError(ex.what());
  }

  mDatabase->createPlate(db::PlateMeta{.jobId = mJobId, .plateId = settings.plateIdx, .notes = ""});
}

///
///
/// \brief      Creates the output folder for the results and returns the path.
///             Outputfolder = <outputFolder>/<DATE-TIME>_<JOB-NAME>
/// \author     Joachim Danmayr
/// \param[in]  resultsFolder Folder where the results should be stored in
///
void Results::prepareOutputFolders(const std::filesystem::path &resultsFolder)
{
  auto nowString      = ::joda::helper::timeNowToString();
  mOutputFolder       = resultsFolder / (nowString + "_" + mExperimentSettings.jobName);
  mOutputFolderImages = mOutputFolder / mJobId / CONTROL_IMAGE_PATH;
  mDatabaseFileName   = mOutputFolder / DB_FILENAME;

  if(!std::filesystem::exists(mOutputFolder)) {
    std::filesystem::create_directories(mOutputFolder);
  }

  if(!std::filesystem::exists(mOutputFolderImages)) {
    std::filesystem::create_directories(mOutputFolderImages);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Results::appendToDetailReport(const joda::image::detect::DetectionResponse &results,
                                   const joda::settings::ChannelSettingsMeta &channelSettings, uint16_t tileIdx,
                                   const image::ImageProperties &imgProps, const std::filesystem::path &imagePath)
{
  auto id           = DurationCount::start("Append to detail report");
  auto wellPosition = applyRegex(mExperimentSettings.imageFileNameRegex, imagePath);
  mDatabase->createWell(db::WellMeta{.jobId    = mJobId,
                                     .plateId  = mExperimentSettings.plateIdx,
                                     .wellId   = wellPosition.well.wellId,
                                     .wellPosX = wellPosition.well.wellPos[WellId::POS_X],
                                     .wellPosY = wellPosition.well.wellPos[WellId::POS_Y],
                                     .notes    = ""});

  mDatabase->createImage(db::ImageMeta{.jobId     = mJobId,
                                       .plateId   = mExperimentSettings.plateIdx,
                                       .wellId    = wellPosition.well.wellId,
                                       .imageId   = wellPosition.imageId,
                                       .imageName = imagePath.filename().string(),
                                       .width     = imgProps.width,
                                       .height    = imgProps.height});

  auto controlImagePath = createControlImage(results, channelSettings, tileIdx, imgProps, imagePath);
  auto channelId        = static_cast<uint8_t>(channelSettings.channelIdx);
  mDatabase->createChannel(db::ChannelMeta{.jobId            = mJobId,
                                           .plateId          = mExperimentSettings.plateIdx,
                                           .wellId           = wellPosition.well.wellId,
                                           .imageId          = wellPosition.imageId,
                                           .channelId        = channelId,
                                           .name             = channelSettings.name,
                                           .controlImagePath = controlImagePath.string()});

  auto [offsetX, offsetY] =
      ::joda::image::TiffLoader::calculateTileXYoffset(::joda::pipeline::TILES_TO_LOAD_PER_RUN, tileIdx, imgProps.width,
                                                       imgProps.height, imgProps.tileWidth, imgProps.tileHeight);
  int64_t xMul = offsetX * imgProps.tileWidth;
  int64_t yMul = offsetY * imgProps.tileHeight;

  duckdb::vector<duckdb::Value> measureChannelKeys{};
  db::objects_t objects;
  uint64_t roiIdx = 0;
  for(const auto &roi : results.result) {
    uint64_t index = roiIdx;
    roiIdx++;
    db::Data &chan = objects[index];

    chan.validity = toValidity(roi.getValidity());

    // Measure channels
    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::CONFIDENCE, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(roi.getConfidence()));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::AREA_SIZE, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(roi.getAreaSize()));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::PERIMETER, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(roi.getPerimeter()));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::CIRCULARITY, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(roi.getCircularity()));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::CENTER_OF_MASS_X, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(static_cast<double>(roi.getCenterOfMass().x) + xMul));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::CENTER_OF_MASS_Y, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(static_cast<double>(roi.getCenterOfMass().y) + yMul));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::BOUNDING_BOX_WIDTH, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(static_cast<double>(roi.getBoundingBox().width)));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::BOUNDING_BOX_HEIGHT, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(static_cast<double>(roi.getBoundingBox().height)));

    double intensityAvg = 0;
    double intensityMin = 0;
    double intensityMax = 0;
    if(roi.getIntensity().contains(channelSettings.channelIdx)) {
      auto intensityMe = roi.getIntensity().at(channelSettings.channelIdx);
      intensityAvg     = intensityMe.intensity;
      intensityMin     = intensityMe.intensityMin;
      intensityMax     = intensityMe.intensityMax;
    }

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::INTENSITY_AVG, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(intensityAvg));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::INTENSITY_MIN, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(intensityMin));

    chan.keys.emplace_back(
        duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::INTENSITY_MAX, ChannelIndex::ME)));
    chan.vals.emplace_back(duckdb::Value::DOUBLE(intensityMax));

    //
    // Intensity channels
    //
    for(const auto &[idx, intensity] : roi.getIntensity()) {
      if(idx != channelSettings.channelIdx) {
        chan.keys.emplace_back(duckdb::Value::UINTEGER(
            (uint32_t) MeasureChannelId(MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, toMeasureChannelIndex(idx))));
        chan.vals.emplace_back(duckdb::Value::DOUBLE(intensity.intensity));

        chan.keys.emplace_back(duckdb::Value::UINTEGER(
            (uint32_t) MeasureChannelId(MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, toMeasureChannelIndex(idx))));
        chan.vals.emplace_back(duckdb::Value::DOUBLE(intensity.intensityMin));

        chan.keys.emplace_back(duckdb::Value::UINTEGER(
            (uint32_t) MeasureChannelId(MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, toMeasureChannelIndex(idx))));
        chan.vals.emplace_back(duckdb::Value::DOUBLE(intensity.intensityMax));
      }
    }

    //
    // Counting channels
    //
    for(const auto &[idx, intersecting] : roi.getIntersectingRois()) {
      chan.keys.emplace_back(duckdb::Value::UINTEGER(
          (uint32_t) MeasureChannelId(MeasureChannel::CROSS_CHANNEL_COUNT, toMeasureChannelIndex(idx))));
      chan.vals.emplace_back(duckdb::Value::DOUBLE(intersecting.roiValid.size()));
    }
  }

  mDatabase->createObjects(db::ObjectMeta{.jobId     = mJobId,
                                          .plateId   = mExperimentSettings.plateIdx,
                                          .wellId    = wellPosition.well.wellId,
                                          .imageId   = wellPosition.imageId,
                                          .channelId = static_cast<uint8_t>(channelSettings.channelIdx),
                                          .tileId    = tileIdx,
                                          .objects   = objects});

  DurationCount::stop(id);
}

///
/// \brief      Create control image
/// \author     Joachim Danmayr
///
std::filesystem::path Results::createControlImage(const joda::image::detect::DetectionResponse &result,
                                                  const joda::settings::ChannelSettingsMeta &channelSettings,
                                                  uint16_t tileIdx, const image::ImageProperties &imgProps,
                                                  const std::filesystem::path &imagePath)
{
  // Free memory
  auto id = DurationCount::start("Create control image");
  std::vector<int> compression_params;
  compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
  compression_params.push_back(1);

  std::filesystem::path controlImageFileNameWithPlaceholder =
      (imagePath.filename().string() + "_" + joda::settings::to_string(channelSettings.channelIdx) + "_${tile_id}" +
       CONTROL_IMAGES_FILE_EXTENSION);

  std::filesystem::path relativeFolderToWrite = mJobId / CONTROL_IMAGE_PATH / imagePath.filename();
  std::filesystem::path absoluteFolderToWrite = mOutputFolder / relativeFolderToWrite;
  if(!std::filesystem::exists(absoluteFolderToWrite)) {
    std::filesystem::create_directories(absoluteFolderToWrite);
  }

  if(!result.controlImage.empty()) {
    std::string crlImgFileNameWithTile = controlImageFileNameWithPlaceholder;
    helper::stringReplace(crlImgFileNameWithTile, "${tile_id}", std::to_string(tileIdx));
    std::to_string(tileIdx);
    cv::imwrite((absoluteFolderToWrite / crlImgFileNameWithTile).string(), result.controlImage, compression_params);
  } else {
    std::cout << "CTRL img null" << std::endl;
  }
  DurationCount::stop(id);
  return relativeFolderToWrite / controlImageFileNameWithPlaceholder;
}

///
/// \brief      Apply regex
/// \author     Joachim Danmayr
///
WellId Results::applyRegex(const std::string &regex, const std::filesystem::path &imagePath)
{
  std::regex pattern(regex);
  std::smatch match;
  WellId result;

  std::string fileName = imagePath.filename().string();
  if(std::regex_search(fileName, match, pattern)) {
    if(match.size() >= 5) {
      result.well.wellPos[WellId::POS_Y] = helper::stringToNumber(match[2].str());
      result.well.wellPos[WellId::POS_X] = helper::stringToNumber(match[3].str());
      result.imageId                     = helper::stringToNumber(match[4].str());

    } else {
      throw std::invalid_argument("Pattern not found.");
    }
  } else {
    throw std::invalid_argument("Pattern not found.");
  }
  return result;
}

}    // namespace joda::results
