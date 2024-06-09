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
#include <cstdint>
#include <exception>
#include <filesystem>
#include <memory>
#include <mutex>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/fnv1a.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/uuid.hpp"
#include "backend/image_processing/reader/tif/image_loader_tif.hpp"
#include "backend/pipelines/processor/image_processor.hpp"
#include "backend/results/database/database.hpp"
#include "backend/results/database/database_interface.hpp"
#include "backend/results/db_column_ids.hpp"
#include "backend/settings/channel/channel_settings_meta.hpp"
#include "backend/settings/experiment_settings.hpp"
#include <duckdb/common/types/value.hpp>

namespace joda::results {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Results::Results(const std::filesystem::path &pathToRawData, const ExperimentSetting &settings,
                 const joda::settings::AnalyzeSettings &analyzeSettings) :
    mPathToRawData(pathToRawData),
    mExperimentSettings(settings), mAnalyzeId(joda::helper::generate_uuid())
{
  // std::filesystem::path = ".."/pathToRawData;
  prepareOutputFolders(pathToRawData / "imagec");
  mDatabase = std::make_shared<db::Database>(mDatabaseFileName);
  mDatabase->open();
  try {
    nlohmann::json analyzeSettingsJson = analyzeSettings;
    mDatabase->createAnalyze(db::AnalyzeMeta{.runId                    = settings.runId,
                                             .analyzeId                = mAnalyzeId,
                                             .name                     = settings.analyzeName,
                                             .scientists               = {settings.scientistName},
                                             .location                 = "",
                                             .notes                    = "",
                                             .analysesSettingsJsonDump = analyzeSettingsJson.dump()});
  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
  }

  mDatabase->createPlate(db::PlateMeta{.analyzeId = mAnalyzeId, .plateId = settings.plateIdx, .notes = ""});
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
  mOutputFolder       = resultsFolder / (nowString + "_" + mExperimentSettings.analyzeName);
  mOutputFolderImages = mOutputFolder / mAnalyzeId / CONTROL_IMAGE_PATH;
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
void Results::appendChannelsToDetailReport(const joda::settings::AnalyzeSettings &settings)
{
  auto addToVector = [this](const settings::ChannelSettingsMeta &meta,
                            const settings::CrossChannelSettings &crossChannel) {
    std::vector<MeasureChannelId> measureChannels;
    measureChannels.emplace_back(MeasureChannelId(MeasureChannel::CONFIDENCE, ChannelIndex::ME));
    measureChannels.emplace_back(MeasureChannelId(MeasureChannel::AREA_SIZE, ChannelIndex::ME));
    measureChannels.emplace_back(MeasureChannelId(MeasureChannel::PERIMETER, ChannelIndex::ME));
    measureChannels.emplace_back(MeasureChannelId(MeasureChannel::CIRCULARITY, ChannelIndex::ME));
    measureChannels.emplace_back(MeasureChannelId(MeasureChannel::VALID, ChannelIndex::ME));
    measureChannels.emplace_back(MeasureChannelId(MeasureChannel::INVALID, ChannelIndex::ME));
    measureChannels.emplace_back(MeasureChannelId(MeasureChannel::CENTER_OF_MASS_X, ChannelIndex::ME));
    measureChannels.emplace_back(MeasureChannelId(MeasureChannel::CENTER_OF_MASS_Y, ChannelIndex::ME));
    measureChannels.emplace_back(MeasureChannelId(MeasureChannel::BOUNDING_BOX_WIDTH, ChannelIndex::ME));
    measureChannels.emplace_back(MeasureChannelId(MeasureChannel::BOUNDING_BOX_HEIGHT, ChannelIndex::ME));
    measureChannels.emplace_back(MeasureChannelId(MeasureChannel::INTENSITY_AVG, ChannelIndex::ME));
    measureChannels.emplace_back(MeasureChannelId(MeasureChannel::INTENSITY_MIN, ChannelIndex::ME));
    measureChannels.emplace_back(MeasureChannelId(MeasureChannel::INTENSITY_MAX, ChannelIndex::ME));

    for(const auto crossChannelIntensityIdx : crossChannel.crossChannelIntensityChannels) {
      measureChannels.emplace_back(
          MeasureChannelId(MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, toChannelIndex(crossChannelIntensityIdx)));

      measureChannels.emplace_back(
          MeasureChannelId(MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, toChannelIndex(crossChannelIntensityIdx)));

      measureChannels.emplace_back(
          MeasureChannelId(MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, toChannelIndex(crossChannelIntensityIdx)));
    }

    for(const auto crossChannelCountIdx : crossChannel.crossChannelCountChannels) {
      measureChannels.emplace_back(
          MeasureChannelId(MeasureChannel::CROSS_CHANNEL_COUNT, toChannelIndex(crossChannelCountIdx)));
    }

    mDatabase->createChannel(db::ChannelMeta{.analyzeId    = mAnalyzeId,
                                             .channelId    = toChannelIndex(meta.channelIdx),
                                             .name         = meta.name,
                                             .measurements = std::move(measureChannels)

    });
  };

  for(const auto &channel : settings.channels) {
    addToVector(channel.meta, channel.crossChannel);
  }

  for(const auto &channel : settings.vChannels) {
    if(channel.$intersection.has_value()) {
      addToVector(channel.$intersection->meta, channel.$intersection->crossChannel);
    }
    if(channel.$voronoi.has_value()) {
      addToVector(channel.$voronoi->meta, channel.$voronoi->crossChannel);
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Results::appendImageToDetailReport(const image::ImageProperties &imgProps, const std::filesystem::path &imagePath)
{
  WellPosGenerator::Pos posInWell;
  {
    GroupInformation groupInfo;
    switch(mExperimentSettings.groupBy) {
      case settings::ExperimentSettings::GroupBy::OFF: {
        groupInfo.groupName     = "";
        groupInfo.well.wellPosY = UINT16_MAX;
        groupInfo.well.wellPosX = UINT16_MAX;
        groupInfo.well.imageIdx = UINT32_MAX;
      } break;
      case settings::ExperimentSettings::GroupBy::DIRECTORY: {
        groupInfo.groupName     = imagePath.parent_path().string();
        groupInfo.well.wellPosY = UINT16_MAX;
        groupInfo.well.wellPosX = UINT16_MAX;
        groupInfo.well.imageIdx = UINT32_MAX;
      } break;
      case settings::ExperimentSettings::GroupBy::FILENAME: {
        groupInfo = applyRegex(mExperimentSettings.imageFileNameRegex, imagePath);
      } break;
    }
    {
      std::lock_guard<std::mutex> lock(mWellGeneratorLock);
      posInWell = mWellPosGenerator.getGroupId(groupInfo);
    }
  }

  uint64_t imageId = calcImagePathHash(mExperimentSettings.runId, imagePath);

  try {
    mDatabase->createGroup(db::GroupMeta{.analyzeId = mAnalyzeId,
                                         .plateId   = mExperimentSettings.plateIdx,
                                         .groupId   = posInWell.groupId,
                                         .wellPosX  = posInWell.x,
                                         .wellPosY  = posInWell.y,
                                         .name      = posInWell.groupName,
                                         .notes     = ""});
  } catch(const std::exception &ex) {
    joda::log::logWarning("Ceate Well:" + std::string(ex.what()));
  }

  try {
    mDatabase->createImage(db::ImageMeta{.analyzeId         = mAnalyzeId,
                                         .plateId           = mExperimentSettings.plateIdx,
                                         .groupId           = posInWell.groupId,
                                         .imageId           = imageId,
                                         .imageIdx          = posInWell.imgIdx,
                                         .originalImagePath = std::filesystem::relative(imagePath, mOutputFolder),
                                         .width             = imgProps.width,
                                         .height            = imgProps.height});
  } catch(const std::exception &ex) {
    joda::log::logWarning("Ceate Image:" + std::string(ex.what()) + "\n" +
                          std::filesystem::relative(imagePath, mOutputFolder).string() + "\n" +
                          std::to_string(imageId));
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Results::prepareDetailReportAdding()
    -> std::tuple<std::shared_ptr<duckdb::Appender>, std::shared_ptr<duckdb::Connection>>
{
  auto connection = mDatabase->acquire();
  // connection->BeginTransaction();
  auto appender = std::make_shared<duckdb::Appender>(*connection, "objects");
  return {appender, connection};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Results::writePredatedData(std::shared_ptr<duckdb::Appender> appender,
                                std::shared_ptr<duckdb::Connection> connection)
{
  auto id = DurationCount::start("Close");
  appender->Close();
  // connection->Commit();
  DurationCount::stop(id);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Results::appendToDetailReport(std::shared_ptr<duckdb::Appender> appender,
                                   const joda::image::detect::DetectionResponse &results,
                                   const joda::settings::ChannelSettingsMeta &channelSettings, uint16_t tileIdx,
                                   const image::ImageProperties &imgProps, const std::filesystem::path &imagePath)
{
  try {
    auto id          = DurationCount::start("Append to detail report");
    uint64_t imageId = calcImagePathHash(mExperimentSettings.runId, imagePath);

    auto controlImagePath = createControlImage(results, channelSettings, tileIdx, imgProps, imagePath);
    auto channelId        = toChannelIndex(channelSettings.channelIdx);
    mDatabase->createImageChannel(db::ImageChannelMeta{.analyzeId        = mAnalyzeId,
                                                       .imageId          = imageId,
                                                       .channelId        = channelId,
                                                       .validity         = toChannelValidity(results.responseValidity),
                                                       .invalidateAll    = results.invalidateWholeImage,
                                                       .controlImagePath = controlImagePath});

    auto [offsetX, offsetY] = ::joda::image::TiffLoader::calculateTileXYoffset(::joda::pipeline::TILES_TO_LOAD_PER_RUN,
                                                                               tileIdx, imgProps.width, imgProps.height,
                                                                               imgProps.tileWidth, imgProps.tileHeight);
    int64_t xMul            = offsetX * imgProps.tileWidth;
    int64_t yMul            = offsetY * imgProps.tileHeight;

    uint64_t roiIdx = 0;
    auto id2 = DurationCount::start("loop db prepare >" + std::to_string(results.result->size()) + "<.");    // 30ms

    auto uuid = duckdb::Value::UUID(mAnalyzeId);
    for(const auto &roi : *results.result) {
      uint64_t index = roiIdx;
      roiIdx++;
      // db::Data &chan = objects[index];

      duckdb::vector<duckdb::Value> keys;
      duckdb::vector<duckdb::Value> vals;

      // Measure channels
      keys.emplace_back(
          duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::CONFIDENCE, ChannelIndex::ME)));
      vals.emplace_back(duckdb::Value::DOUBLE(roi.getConfidence()));

      keys.emplace_back(
          duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::AREA_SIZE, ChannelIndex::ME)));
      vals.emplace_back(duckdb::Value::DOUBLE(roi.getAreaSize()));

      keys.emplace_back(
          duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::PERIMETER, ChannelIndex::ME)));
      vals.emplace_back(duckdb::Value::DOUBLE(roi.getPerimeter()));

      keys.emplace_back(
          duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::CIRCULARITY, ChannelIndex::ME)));
      vals.emplace_back(duckdb::Value::DOUBLE(roi.getCircularity()));

      keys.emplace_back(duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::VALID, ChannelIndex::ME)));
      vals.emplace_back(duckdb::Value::TINYINT(roi.isValid() ? 1 : 0));

      keys.emplace_back(
          duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::INVALID, ChannelIndex::ME)));
      vals.emplace_back(duckdb::Value::TINYINT(roi.isValid() ? 0 : 1));

      keys.emplace_back(
          duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::CENTER_OF_MASS_X, ChannelIndex::ME)));
      vals.emplace_back(duckdb::Value::DOUBLE(static_cast<double>(roi.getCenterOfMass().x) + xMul));

      keys.emplace_back(
          duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::CENTER_OF_MASS_Y, ChannelIndex::ME)));
      vals.emplace_back(duckdb::Value::DOUBLE(static_cast<double>(roi.getCenterOfMass().y) + yMul));

      keys.emplace_back(
          duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::BOUNDING_BOX_WIDTH, ChannelIndex::ME)));
      vals.emplace_back(duckdb::Value::DOUBLE(static_cast<double>(roi.getBoundingBox().width)));

      keys.emplace_back(
          duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::BOUNDING_BOX_HEIGHT, ChannelIndex::ME)));
      vals.emplace_back(duckdb::Value::DOUBLE(static_cast<double>(roi.getBoundingBox().height)));

      double intensityAvg = 0;
      double intensityMin = 0;
      double intensityMax = 0;
      if(roi.getIntensity().contains(channelSettings.channelIdx)) {
        auto intensityMe = roi.getIntensity().at(channelSettings.channelIdx);
        intensityAvg     = intensityMe.intensity;
        intensityMin     = intensityMe.intensityMin;
        intensityMax     = intensityMe.intensityMax;
      }

      keys.emplace_back(
          duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::INTENSITY_AVG, ChannelIndex::ME)));
      vals.emplace_back(duckdb::Value::DOUBLE(intensityAvg));

      keys.emplace_back(
          duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::INTENSITY_MIN, ChannelIndex::ME)));
      vals.emplace_back(duckdb::Value::DOUBLE(intensityMin));

      keys.emplace_back(
          duckdb::Value::UINTEGER((uint32_t) MeasureChannelId(MeasureChannel::INTENSITY_MAX, ChannelIndex::ME)));
      vals.emplace_back(duckdb::Value::DOUBLE(intensityMax));

      //
      // Intensity channels
      //
      for(const auto &[idx, intensity] : roi.getIntensity()) {
        if(idx != channelSettings.channelIdx) {
          keys.emplace_back(duckdb::Value::UINTEGER(
              (uint32_t) MeasureChannelId(MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, toChannelIndex(idx))));
          vals.emplace_back(duckdb::Value::DOUBLE(intensity.intensity));

          keys.emplace_back(duckdb::Value::UINTEGER(
              (uint32_t) MeasureChannelId(MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, toChannelIndex(idx))));
          vals.emplace_back(duckdb::Value::DOUBLE(intensity.intensityMin));

          keys.emplace_back(duckdb::Value::UINTEGER(
              (uint32_t) MeasureChannelId(MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, toChannelIndex(idx))));
          vals.emplace_back(duckdb::Value::DOUBLE(intensity.intensityMax));
        }
      }

      //
      // Counting channels
      //
      for(const auto &[idx, intersecting] : roi.getIntersectingRois()) {
        keys.emplace_back(duckdb::Value::UINTEGER(
            (uint32_t) MeasureChannelId(MeasureChannel::CROSS_CHANNEL_COUNT, toChannelIndex(idx))));
        vals.emplace_back(duckdb::Value::DOUBLE(intersecting.roiValid.size()));
      }

      {
        std::lock_guard<std::mutex> lock(mAppenderMutex);
        appender->BeginRow();
        appender->Append(uuid);
        appender->Append<uint64_t>(imageId);
        appender->Append<uint16_t>(static_cast<uint16_t>(channelId));
        appender->Append<uint32_t>(index);
        appender->Append<uint16_t>(tileIdx);
        appender->Append<uint64_t>(toValidity(roi.getValidity()).to_ulong());
        auto mapToInsert = duckdb::Value::MAP(duckdb::LogicalType(duckdb::LogicalTypeId::UINTEGER),
                                              duckdb::LogicalType(duckdb::LogicalTypeId::DOUBLE), keys, vals);
        appender->Append<duckdb::Value>(mapToInsert);    // 0.004ms
        appender->EndRow();
      }
    }
    DurationCount::stop(id2);

    DurationCount::stop(id);
  } catch(const std::exception &ex) {
    joda::log::logWarning("Append to detail report:" + std::string(ex.what()));
  }
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

  std::filesystem::path relativeFolderToWrite = mAnalyzeId / CONTROL_IMAGE_PATH / imagePath.filename();
  std::filesystem::path absoluteFolderToWrite = mOutputFolder / relativeFolderToWrite;
  if(!std::filesystem::exists(absoluteFolderToWrite)) {
    std::filesystem::create_directories(absoluteFolderToWrite);
  }

  if(!result.controlImage.empty()) {
    std::string crlImgFileNameWithTile = controlImageFileNameWithPlaceholder.string();
    helper::stringReplace(crlImgFileNameWithTile, "${tile_id}", std::to_string(tileIdx));
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
GroupInformation Results::applyRegex(const std::string &regex, const std::filesystem::path &imagePath)
{
  std::regex pattern(regex);
  std::smatch match;
  GroupInformation result;

  std::string fileName = imagePath.filename().string();
  if(std::regex_search(fileName, match, pattern)) {
    if(match.size() >= 5) {
      result.groupName     = match[1].str();
      result.well.wellPosY = helper::stringToNumber(match[2].str());
      result.well.wellPosX = helper::stringToNumber(match[3].str());
      result.well.imageIdx = helper::stringToNumber(match[4].str());
    } else if(match.size() >= 3) {
      result.groupName     = match[1].str();
      result.well.wellPosY = UINT16_MAX;
      result.well.wellPosX = UINT16_MAX;
      result.well.imageIdx = helper::stringToNumber(match[2].str());
    } else if(match.size() >= 2) {
      result.groupName     = match[1].str();
      result.well.wellPosY = UINT16_MAX;
      result.well.wellPosX = UINT16_MAX;
      result.well.imageIdx = UINT32_MAX;
    } else {
      throw std::invalid_argument("Pattern not found.");
    }
  } else {
    throw std::invalid_argument("Pattern not found.");
  }
  return result;
}

///
/// \brief      Apply regex
/// \author     Joachim Danmayr
///
uint64_t Results::calcImagePathHash(const std::string &runId, const std::filesystem::path &pathToOriginalImage)
{
  return helper::fnv1a(runId + pathToOriginalImage.string());
}

}    // namespace joda::results
