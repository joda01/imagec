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
    } else {
      std::cout << "T: " << std::to_string((uint8_t) value.type().id()) << std::endl;
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
auto Analyzer::getImagesForJob(const std::string &analyzeId) -> std::vector<db::ImageMeta>
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
  for(size_t n = 0; n < materializedResult->RowCount(); n++)
    images.emplace_back(db::ImageMeta{
        .analyzeId         = materializedResult->GetValue(0, n).GetValue<std::string>(),
        .plateId           = materializedResult->GetValue(9, n).GetValue<uint8_t>(),
        .wellId            = materializedResult->GetValue(10, n).GetValue<uint16_t>(),
        .imageId           = materializedResult->GetValue(1, n).GetValue<uint64_t>(),
        .imageIdx          = materializedResult->GetValue(2, n).GetValue<uint32_t>(),
        .originalImagePath = materializedResult->GetValue(4, n).GetValue<std::string>(),
        .width             = materializedResult->GetValue(5, n).GetValue<uint64_t>(),
        .height            = materializedResult->GetValue(6, n).GetValue<uint64_t>(),

    });

  return images;
}

///
/// \brief      Create control image
/// \author     Joachim Danmayr
///
auto Analyzer::getChannelsForImage(const std::string &analyzeId, uint64_t imageId) -> std::vector<db::ChannelMeta>
{
  std::vector<db::ChannelMeta> channels;
  std::unique_ptr<duckdb::QueryResult> result =
      mDatabase.select("SELECT * FROM channel WHERE analyze_id=? AND image_id=? ORDER BY control_image_path",
                       duckdb::Value::UUID(analyzeId), imageId);

  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }

  auto materializedResult = result->Cast<duckdb::StreamQueryResult>().Materialize();
  for(size_t n = 0; n < materializedResult->RowCount(); n++)
    channels.emplace_back(db::ChannelMeta{
        .analyzeId        = materializedResult->GetValue(0, n).GetValue<std::string>(),
        .imageId          = materializedResult->GetValue(1, n).GetValue<uint64_t>(),
        .channelId        = materializedResult->GetValue(2, n).GetValue<uint8_t>(),
        .name             = materializedResult->GetValue(3, n).GetValue<std::string>(),
        .controlImagePath = materializedResult->GetValue(4, n).GetValue<std::string>(),

    });

  return channels;
}

}    // namespace joda::results
