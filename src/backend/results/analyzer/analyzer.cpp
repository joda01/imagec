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
#include <iterator>
#include <memory>
#include <string>
#include "backend/results/database/database_interface.hpp"
#include "backend/results/db_column_ids.hpp"
#include "plugins/stats_for_well.hpp"
#include <duckdb/common/types/value.hpp>
#include <duckdb/main/stream_query_result.hpp>

namespace joda::results {

Analyzer::Analyzer(const std::filesystem::path &databasePath) : mDatabase(databasePath)
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
                        .location   = materializedResult->GetValue(5, n).GetValue<std::string>(),
                        .notes      = materializedResult->GetValue(6, n).GetValue<std::string>()});
  }
  return jobs;
}

///
/// \brief      Create control image
/// \author     Joachim Danmayr
///
auto Analyzer::getImagesForAnalyses(const std::string &analyzeId) -> std::vector<db::ImageMeta>
{
  std::vector<db::ImageMeta> images;
  std::unique_ptr<duckdb::QueryResult> result = mDatabase.select(
      "SELECT * FROM image INNER JOIN image_well ON image.image_id=image_well.image_id WHERE image.analyze_id=? ORDER "
      "BY file_name",
      duckdb::Value::UUID(analyzeId));

  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    images.emplace_back(db::ImageMeta{
        .analyzeId         = materializedResult->GetValue(0, n).GetValue<std::string>(),
        .plateId           = materializedResult->GetValue(9, n).GetValue<uint8_t>(),
        .wellId            = WellId{.well{.wellId = materializedResult->GetValue(10, n).GetValue<uint16_t>()}},
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
/// \brief      Create control image
/// \author     Joachim Danmayr
///
auto Analyzer::getChannelsForAnalyses(const std::string &analyzeId) -> std::vector<db::ChannelMeta>
{
  std::vector<db::ChannelMeta> channels;
  std::unique_ptr<duckdb::QueryResult> result =
      mDatabase.select("SELECT * FROM channel WHERE analyze_id=? ORDER BY name", duckdb::Value::UUID(analyzeId));

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
      mDatabase.select("SELECT * FROM plate WHERE analyze_id=? ORDER BY plate_id", duckdb::Value::UUID(analyzeId));

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
auto Analyzer::getWellsForPlate(const std::string &analyzeId, uint8_t plateId) -> std::vector<db::WellMeta>
{
  std::vector<db::WellMeta> wells;
  std::unique_ptr<duckdb::QueryResult> result =
      mDatabase.select("SELECT * FROM well WHERE analyze_id=? AND plate_id=? ORDER BY (plate_id,well_id)",
                       duckdb::Value::UUID(analyzeId), plateId);

  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  for(size_t n = 0; n < materializedResult->RowCount(); n++) {
    wells.emplace_back(db::WellMeta{
        .analyzeId = materializedResult->GetValue(0, n).GetValue<std::string>(),
        .plateId   = materializedResult->GetValue(1, n).GetValue<uint8_t>(),
        .wellId    = WellId{.well{.wellId = materializedResult->GetValue(2, n).GetValue<uint16_t>()}},
        .wellPosX  = materializedResult->GetValue(3, n).GetValue<uint8_t>(),
        .wellPosY  = materializedResult->GetValue(4, n).GetValue<uint8_t>(),
        .notes     = materializedResult->GetValue(5, n).GetValue<std::string>(),
    });
  }

  return wells;
}

}    // namespace joda::results