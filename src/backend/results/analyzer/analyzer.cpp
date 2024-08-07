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

#include "analyzer.hpp"
#include <cstddef>
#include <filesystem>
#include <iterator>
#include <memory>
#include <string>
#include <tuple>
#include "backend/results/database/database_interface.hpp"
#include "backend/results/db_column_ids.hpp"
#include "plugins/stats_for_well.hpp"
#include <duckdb/common/types/value.hpp>
#include <duckdb/main/stream_query_result.hpp>

namespace joda::results {

Analyzer::Analyzer(const std::filesystem::path &databasePath) :
    mDatabase(databasePath), mParentPathToDb(databasePath.parent_path())
{
}

///
/// \brief      Returns all jobs
/// \author     Joachim Danmayr
///
auto Analyzer::getAnalyzes() -> std::vector<db::AnalyzeMeta>
{
  std::vector<db::AnalyzeMeta> jobs;
  std::unique_ptr<duckdb::QueryResult> result = mDatabase.select("SELECT * FROM analyzes");
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    std::vector<std::string> scientists;

    const auto &value = materializedResult->GetValue(3, n);

    if(value.IsNull()) {
      std::cout << "NULL value found" << std::endl;
    } else if(value.type().id() == duckdb::LogicalTypeId::LIST) {
      const auto &scientistName = duckdb::ListValue::GetChildren(value);
      for(auto const &data : scientistName) {
        scientists.push_back(data.GetValue<std::string>());
      }
    }

    // std::cout << dataChunk->ToString() << std::endl;
    jobs.emplace_back(
        db::AnalyzeMeta{.runId      = materializedResult->GetValue(0, n).GetValue<std::string>(),
                        .analyzeId  = materializedResult->GetValue(1, n).GetValue<std::string>(),
                        .name       = materializedResult->GetValue(2, n).GetValue<std::string>(),
                        .scientists = std::move(scientists),
                        .timestamp  = std::chrono::system_clock::from_time_t(duckdb::Timestamp::GetEpochSeconds(
                            materializedResult->GetValue<duckdb::timestamp_t>(4, n))),
                        .addressOrganization = materializedResult->GetValue(5, n).GetValue<std::string>(),
                        .notes               = materializedResult->GetValue(6, n).GetValue<std::string>()});
  }
  return jobs;
}

///
/// \brief      Create control image
/// \author     Joachim Danmayr
///
auto Analyzer::getAbsolutePathToControlImage(const std::string &relativePath, int32_t tileID) const
    -> std::filesystem::path
{
  std::string pathRel = relativePath;
  helper::stringReplace(pathRel, "${tile_id}", std::to_string(tileID));
  return mParentPathToDb / pathRel;
}

///
/// \brief      Create control image
/// \author     Joachim Danmayr
///
auto Analyzer::getImagesForAnalyses(const std::string &analyzeId) -> std::vector<db::ImageMeta>
{
  std::vector<db::ImageMeta> images;
  std::unique_ptr<duckdb::QueryResult> result = mDatabase.select(
      "SELECT * FROM images INNER JOIN images_groups ON images.image_id=images_groups.image_id WHERE "
      "images.analyze_id=? ORDER BY file_name",
      duckdb::Value::UUID(analyzeId));

  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    images.emplace_back(db::ImageMeta{
        .analyzeId         = materializedResult->GetValue(0, n).GetValue<std::string>(),
        .plateId           = materializedResult->GetValue(9, n).GetValue<uint8_t>(),
        .groupId           = materializedResult->GetValue(10, n).GetValue<uint16_t>(),
        .imageId           = materializedResult->GetValue(1, n).GetValue<uint64_t>(),
        .imageIdx          = materializedResult->GetValue(2, n).GetValue<uint32_t>(),
        .originalImagePath = materializedResult->GetValue(4, n).GetValue<std::string>(),
        .width             = materializedResult->GetValue(5, n).GetValue<uint64_t>(),
        .height            = materializedResult->GetValue(6, n).GetValue<uint64_t>(),
    });
  }

  return images;
}

///
/// \brief      Get information about one image
/// \author     Joachim Danmayr
///
auto Analyzer::getImageInformation(const std::string &analyzeId, uint8_t plateId, ChannelIndex channel,
                                   uint64_t imageId) -> std::tuple<db::ImageMeta, db::ChannelMeta, db::ImageChannelMeta>
{
  db::ImageMeta imageMeta;
  db::ChannelMeta channelMeta;
  db::ImageChannelMeta imgChannelMeta;
  std::unique_ptr<duckdb::QueryResult> result = mDatabase.select(
      "SELECT * FROM images "
      "INNER JOIN images_groups ON images.image_id=images_groups.image_id "
      "INNER JOIN channels_images ON (images.image_id=channels_images.image_id) "
      "INNER JOIN channels ON (channels_images.channel_id=channels.channel_id) "
      "WHERE images.analyze_id=? AND images.image_id=? AND channels_images.channel_id=?",
      duckdb::Value::UUID(analyzeId), imageId, (uint8_t) channel);

  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    auto analyzeId    = materializedResult->GetValue(0, n).GetValue<std::string>();    // Analyze ID
    auto imageId      = materializedResult->GetValue(1, n).GetValue<uint64_t>();       // Image ID
    auto imageIdx     = materializedResult->GetValue(2, n).GetValue<uint32_t>();       // Image IDX
    auto fileName     = materializedResult->GetValue(3, n).GetValue<std::string>();    // Filename
    auto originalPath = materializedResult->GetValue(4, n).GetValue<std::string>();    // Original image path
    auto width        = materializedResult->GetValue(5, n).GetValue<uint64_t>();       // width
    auto height       = materializedResult->GetValue(6, n).GetValue<uint64_t>();       // height
    // materializedResult->GetValue(7, n).GetValue<std::string>();                        // Analyze ID
    // materializedResult->GetValue(8, n).GetValue<uint64_t>();                           // Image ID
    auto plateId = materializedResult->GetValue(9, n).GetValue<uint8_t>();      // Plate ID
    auto groupId = materializedResult->GetValue(10, n).GetValue<uint16_t>();    // Group ID
    // materializedResult->GetValue(11, n).GetValue<std::string>();                // Analyze ID
    // materializedResult->GetValue(12, n).GetValue<uint64_t>();                   // Image ID
    auto channelId        = materializedResult->GetValue(13, n).GetValue<uint8_t>();        // Channel ID
    auto controlImagePath = materializedResult->GetValue(14, n).GetValue<std::string>();    // Control image path
    ChannelValidity validity{materializedResult->GetValue(15, n).GetValue<uint64_t>()};     // Validity
    auto invalidateAll = materializedResult->GetValue(16, n).GetValue<bool>();              // Invalidate all
    // materializedResult->GetValue(17, n).GetValue<std::string>();                // Analyze ID
    // auto channelId = materializedResult->GetValue(18, n).GetValue<uint8_t>();    // Channel ID
    auto channelName = materializedResult->GetValue(19, n).GetValue<std::string>();    // Control image path

    std::vector<MeasureChannelId> measurements;
    const auto &value = materializedResult->GetValue(20, n);

    if(value.IsNull()) {
      std::cout << "NULL value found" << std::endl;
    } else if(value.type().id() == duckdb::LogicalTypeId::LIST) {
      const auto &measurementList = duckdb::ListValue::GetChildren(value);
      for(auto const &data : measurementList) {
        measurements.push_back(MeasureChannelId(data.GetValue<uint32_t>()));
      }
    }

    imageMeta = db::ImageMeta{
        .analyzeId         = analyzeId,
        .plateId           = plateId,
        .groupId           = groupId,
        .imageId           = imageId,
        .imageIdx          = imageIdx,
        .originalImagePath = originalPath,
        .width             = width,
        .height            = height,
    };

    channelMeta = db::ChannelMeta{
        .analyzeId    = analyzeId,
        .channelId    = static_cast<ChannelIndex>(channelId),
        .name         = channelName,
        .measurements = std::move(measurements),
    };

    imgChannelMeta = db::ImageChannelMeta{.analyzeId        = analyzeId,
                                          .imageId          = imageId,
                                          .channelId        = static_cast<ChannelIndex>(channelId),
                                          .validity         = validity,
                                          .invalidateAll    = invalidateAll,
                                          .controlImagePath = controlImagePath};
  }

  return {imageMeta, channelMeta, imgChannelMeta};
}

///
/// \brief      Create control image
/// \author     Joachim Danmayr
///
auto Analyzer::getChannelsForAnalyses(const std::string &analyzeId) -> std::vector<db::ChannelMeta>
{
  std::vector<db::ChannelMeta> channels;
  std::unique_ptr<duckdb::QueryResult> result =
      mDatabase.select("SELECT * FROM channels WHERE analyze_id=? ORDER BY name", duckdb::Value::UUID(analyzeId));

  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    std::vector<MeasureChannelId> measurements;
    const auto &value = materializedResult->GetValue(3, n);

    if(value.IsNull()) {
      std::cout << "NULL value found" << std::endl;
    } else if(value.type().id() == duckdb::LogicalTypeId::LIST) {
      const auto &measurementList = duckdb::ListValue::GetChildren(value);
      for(auto const &data : measurementList) {
        measurements.push_back(MeasureChannelId(data.GetValue<uint32_t>()));
      }
    }

    channels.emplace_back(db::ChannelMeta{
        .analyzeId    = materializedResult->GetValue(0, n).GetValue<std::string>(),
        .channelId    = static_cast<ChannelIndex>(materializedResult->GetValue(1, n).GetValue<uint8_t>()),
        .name         = materializedResult->GetValue(2, n).GetValue<std::string>(),
        .measurements = std::move(measurements),

    });
  }

  return channels;
}

///
/// \brief      Create control image
/// \author     Joachim Danmayr
///
auto Analyzer::getPlatesForAnalyses(const std::string &analyzeId) -> std::vector<db::PlateMeta>
{
  std::vector<db::PlateMeta> plates;
  std::unique_ptr<duckdb::QueryResult> result =
      mDatabase.select("SELECT * FROM plates WHERE analyze_id=? ORDER BY plate_id", duckdb::Value::UUID(analyzeId));

  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    plates.emplace_back(db::PlateMeta{
        .analyzeId = materializedResult->GetValue(0, n).GetValue<std::string>(),
        .plateId   = materializedResult->GetValue(1, n).GetValue<uint8_t>(),
        .notes     = materializedResult->GetValue(2, n).GetValue<std::string>(),
    });
  }

  return plates;
}

///
/// \brief      Create control image
/// \author     Joachim Danmayr
///
auto Analyzer::getGroupsForPlate(const std::string &analyzeId, uint8_t plateId) -> std::vector<db::GroupMeta>
{
  std::vector<db::GroupMeta> groups;
  std::unique_ptr<duckdb::QueryResult> result =
      mDatabase.select("SELECT * FROM groups WHERE analyze_id=? AND plate_id=? ORDER BY (plate_id,group_id)",
                       duckdb::Value::UUID(analyzeId), plateId);

  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    groups.emplace_back(db::GroupMeta{
        .analyzeId = materializedResult->GetValue(0, n).GetValue<std::string>(),
        .plateId   = materializedResult->GetValue(1, n).GetValue<uint8_t>(),
        .groupId   = materializedResult->GetValue(2, n).GetValue<uint16_t>(),
        .wellPosX  = materializedResult->GetValue(3, n).GetValue<uint16_t>(),
        .wellPosY  = materializedResult->GetValue(4, n).GetValue<uint16_t>(),
        .name      = materializedResult->GetValue(5, n).GetValue<std::string>(),
        .notes     = materializedResult->GetValue(6, n).GetValue<std::string>(),
    });
  }

  return groups;
}

///
/// \brief      Create control image
/// \author     Joachim Danmayr
///
auto Analyzer::getGroupInformation(const std::string &analyzeId, uint8_t plateId, ChannelIndex channel,
                                   uint16_t groupId) -> std::tuple<db::GroupMeta, db::ChannelMeta>
{
  db::GroupMeta groupMeta;
  db::ChannelMeta channelMeta;

  {
    std::unique_ptr<duckdb::QueryResult> result =
        mDatabase.select("SELECT * FROM groups WHERE analyze_id=? AND plate_id=? AND group_id=?",
                         duckdb::Value::UUID(analyzeId), plateId, groupId);

    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }

    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      groupMeta = db::GroupMeta{
          .analyzeId = materializedResult->GetValue(0, n).GetValue<std::string>(),
          .plateId   = materializedResult->GetValue(1, n).GetValue<uint8_t>(),
          .groupId   = materializedResult->GetValue(2, n).GetValue<uint16_t>(),
          .wellPosX  = materializedResult->GetValue(3, n).GetValue<uint16_t>(),
          .wellPosY  = materializedResult->GetValue(4, n).GetValue<uint16_t>(),
          .name      = materializedResult->GetValue(5, n).GetValue<std::string>(),
          .notes     = materializedResult->GetValue(6, n).GetValue<std::string>(),
      };
    }
  }
  {
    std::unique_ptr<duckdb::QueryResult> result =
        mDatabase.select("SELECT * FROM channels WHERE analyze_id=? AND channel_id=?", duckdb::Value::UUID(analyzeId),
                         (uint8_t) channel);

    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }

    auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
    for(size_t n = 0; n < materializedResult->RowCount(); n++) {
      std::vector<MeasureChannelId> measurements;
      const auto &value = materializedResult->GetValue(3, n);

      if(value.IsNull()) {
        std::cout << "NULL value found" << std::endl;
      } else if(value.type().id() == duckdb::LogicalTypeId::LIST) {
        const auto &measurementList = duckdb::ListValue::GetChildren(value);
        for(auto const &data : measurementList) {
          measurements.push_back(MeasureChannelId(data.GetValue<uint32_t>()));
        }
      }

      channelMeta = db::ChannelMeta{
          .analyzeId    = materializedResult->GetValue(0, n).GetValue<std::string>(),
          .channelId    = static_cast<ChannelIndex>(materializedResult->GetValue(1, n).GetValue<uint8_t>()),
          .name         = materializedResult->GetValue(2, n).GetValue<std::string>(),
          .measurements = std::move(measurements),

      };
    }
  }

  return {groupMeta, channelMeta};
}

///
/// \brief      Create control image
/// \author     Joachim Danmayr
///
void Analyzer::markImageChannelAsManualInvalid(const std::string &analyzeId, uint8_t plateId, ChannelIndex channel,
                                               uint64_t imageId)
{
  std::unique_ptr<duckdb::QueryResult> result = mDatabase.select(
      "UPDATE channels_images SET validity = validity | ? WHERE analyze_id=? AND channel_id=? AND image_id=?",
      static_cast<uint64_t>((1 << static_cast<uint32_t>(ObjectValidityEnum::MANUAL_OUT_SORTED))),
      duckdb::Value::UUID(analyzeId), static_cast<uint8_t>(channel), imageId);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
}

void Analyzer::unMarkImageChannelAsManualInvalid(const std::string &analyzeId, uint8_t plateId, ChannelIndex channel,
                                                 uint64_t imageId)
{
  std::unique_ptr<duckdb::QueryResult> result = mDatabase.select(
      "UPDATE channels_images SET validity = validity & ? WHERE analyze_id=? AND channel_id=? AND image_id=?",
      ~static_cast<uint64_t>((1 << static_cast<uint32_t>(ObjectValidityEnum::MANUAL_OUT_SORTED))),
      duckdb::Value::UUID(analyzeId), static_cast<uint8_t>(channel), imageId);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
}

}    // namespace joda::results
