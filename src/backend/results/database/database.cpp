///
/// \file      database.cpp
/// \author
/// \date      2024-05-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "database.hpp"
#include <duckdb.h>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/fnv1a.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include <duckdb/common/bind_helpers.hpp>
#include <duckdb/common/types.hpp>
#include <duckdb/common/types/value.hpp>
#include <duckdb/common/types/vector.hpp>
#include <duckdb/main/appender.hpp>
#include <duckdb/main/config.hpp>
#include <duckdb/main/connection.hpp>
#include <duckdb/main/database.hpp>
#include <duckdb.hpp>

namespace joda::results::db {

Database::Database(const std::filesystem::path &dbFile)
{
  // cfg.SetOption("memory_limit", "10GB");
  // cfg.SetOption("external_threads", 1);
  // mDbCfg.SetOption("threads", threadsToUse);
  mDbCfg.SetOption("temp_directory", dbFile.parent_path().string());
  mDb         = std::make_unique<duckdb::DuckDB>(dbFile.string(), &mDbCfg);
  mConnection = std::make_unique<duckdb::Connection>(*mDb);
}
Database::~Database()
{
}

///
/// \brief      Open database connection and create tables if not exist
/// \author     Joachim Danmayr
///
void Database::open()
{
  // Command to create a table
  const char *create_table_sql =
      "CREATE TABLE IF NOT EXISTS analyzes ("
      "	run_id UUID,"
      "	analyze_id UUID,"
      "	name TEXT,"
      " scientists TEXT[],"
      " datetime TIMESTAMP,"
      " location TEXT,"
      " notes TEXT,"
      " PRIMARY KEY (analyze_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS plate ("
      "	analyze_id UUID,"
      "	plate_id UTINYINT,"
      " notes TEXT,"
      " PRIMARY KEY (analyze_id, plate_id),"
      " FOREIGN KEY(analyze_id) REFERENCES analyzes(analyze_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS well ("
      "	analyze_id UUID,"
      "	plate_id UTINYINT,"
      "	well_id USMALLINT,"
      "	well_pos_x UTINYINT,"
      "	well_pos_y UTINYINT,"
      " notes TEXT,"
      " PRIMARY KEY (analyze_id, plate_id, well_id),"
      " FOREIGN KEY(analyze_id, plate_id) REFERENCES plate(analyze_id, plate_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS image ("
      "	analyze_id UUID,"
      "	image_id UHUGEINT,"
      " image_idx UINTEGER,"
      " file_name TEXT,"
      " original_image_path TEXT,"
      " width UHUGEINT,"
      " height UHUGEINT,"
      " PRIMARY KEY (analyze_id, image_id),"
      " FOREIGN KEY(analyze_id) REFERENCES analyzes(analyze_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS image_well ("
      "	analyze_id UUID,"
      "	image_id UHUGEINT,"
      "	plate_id UTINYINT,"
      "	well_id USMALLINT,"
      " PRIMARY KEY (analyze_id, image_id),"
      " FOREIGN KEY(analyze_id, image_id) REFERENCES image(analyze_id, image_id),"
      " FOREIGN KEY(analyze_id, plate_id, well_id) REFERENCES well(analyze_id, plate_id, well_id),"
      ");"

      "CREATE TABLE IF NOT EXISTS channel ("
      "	analyze_id UUID,"
      "	channel_id UTINYINT,"
      " name TEXT,"
      " measurements UINTEGER[],"
      " PRIMARY KEY (analyze_id, channel_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS channel_image ("
      "	analyze_id UUID,"
      "	image_id UHUGEINT,"
      "	channel_id UTINYINT,"
      " control_image_path TEXT,"
      " validity UINTEGER,"
      " invalidateAll BOOLEAN,"
      " PRIMARY KEY (analyze_id, image_id, channel_id),"
      " FOREIGN KEY(analyze_id, image_id) REFERENCES image(analyze_id, image_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS object ("
      "	analyze_id UUID,"
      "	image_id UHUGEINT,"
      "	channel_id UTINYINT,"
      "	object_id UINTEGER,"
      "	tile_id USMALLINT,"
      " validity UINTEGER,"
      " values MAP(UINTEGER, DOUBLE)"
      ");"
      "CREATE INDEX object_idx ON object (analyze_id, image_id, channel_id);";

  auto result = mConnection->Query(create_table_sql);
  if(result->HasError()) {
    throw std::invalid_argument(result->GetError());
  }
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::close()
{
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createAnalyze(const AnalyzeMeta &meta)
{
  auto prepare = mConnection->Prepare(
      "INSERT INTO analyzes (run_id, analyze_id, name, scientists, datetime, location, notes) VALUES (?, ?, ?, ?, ?, "
      "?, "
      "?)");
  // Convert it to time since epoch
  auto timeNowMs = (duckdb::timestamp_t) std::chrono::duration_cast<std::chrono::microseconds>(
                       std::chrono::high_resolution_clock::now().time_since_epoch())
                       .count();
  prepare->Execute(duckdb::Value::UUID(meta.runId), duckdb::Value::UUID(meta.analyzeId), duckdb::Value(meta.name),
                   duckdb::Value::LIST({meta.scientists.begin(), meta.scientists.end()}),
                   duckdb::Value::TIMESTAMP(timeNowMs), duckdb::Value(meta.location), duckdb::Value(meta.notes));
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createPlate(const PlateMeta &meta)
{
  auto prepare = mConnection->Prepare("INSERT INTO plate (analyze_id, plate_id, notes) VALUES (?, ?, ?)");

  auto timestamp = duckdb::timestamp_t(std::chrono::duration_cast<std::chrono::microseconds>(
                                           std::chrono::high_resolution_clock::now().time_since_epoch())
                                           .count());
  prepare->Execute(duckdb::Value::UUID(meta.analyzeId), meta.plateId, meta.notes);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createWell(const WellMeta &meta)
{
  auto prepare = mConnection->Prepare(
      "INSERT INTO well (analyze_id, plate_id, well_id,well_pos_x,well_pos_y, notes) VALUES (?, ?, ?, ?, ?, ?)");
  prepare->Execute(duckdb::Value::UUID(meta.analyzeId), meta.plateId, meta.wellId.well.wellId, meta.wellPosX,
                   meta.wellPosY, meta.notes);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createImage(const ImageMeta &meta)
{
  {
    auto prepare = mConnection->Prepare(
        "INSERT INTO image (analyze_id, image_id, image_idx, file_name, original_image_path, width, height) VALUES "
        "(?, ?, ?, ?, ?, ?, ?)");
    prepare->Execute(duckdb::Value::UUID(meta.analyzeId), meta.imageId, meta.imageIdx,
                     meta.originalImagePath.filename().string(), meta.originalImagePath.string(), meta.width,
                     meta.height);
  }

  {
    auto prepare = mConnection->Prepare(
        "INSERT INTO image_well (analyze_id, image_id, plate_id, well_id) VALUES "
        "(?, ?, ?, ?)");
    prepare->Execute(duckdb::Value::UUID(meta.analyzeId), meta.imageId, meta.plateId, meta.wellId.well.wellId);
  }
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createChannel(const ChannelMeta &meta)
{
  auto prepare = mConnection->Prepare(
      "INSERT INTO channel (analyze_id, channel_id, name, measurements) VALUES "
      "(?, ?, ?, ?)");

  duckdb::vector<duckdb::Value> measurements(meta.measurements.size());
  int n = 0;
  for(const auto &val : meta.measurements) {
    measurements[n] = duckdb::Value::UINTEGER(val.getKey());
    n++;
  }

  prepare->Execute(duckdb::Value::UUID(meta.analyzeId), duckdb::Value(static_cast<uint8_t>(meta.channelId)),
                   duckdb::Value(meta.name), duckdb::Value::LIST(std::move(measurements)));
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createImageChannel(const ImageChannelMeta &meta)
{
  auto prepare = mConnection->Prepare(
      "INSERT INTO channel_image (analyze_id, image_id, channel_id, control_image_path, validity, invalidateAll) "
      "VALUES "
      "(?, ?, ?, ?, ?, ?)");

  prepare->Execute(duckdb::Value::UUID(meta.analyzeId), meta.imageId, static_cast<uint8_t>(meta.channelId),
                   meta.controlImagePath.string(), static_cast<uint32_t>(meta.validity), meta.invalidateAll);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createObjects(const ObjectMeta &data)
{
  duckdb::Appender appender(*mConnection, "object");

  // Loop to insert 100 elements

  auto id   = DurationCount::start("loop");    // 30ms
  auto uuid = duckdb::Value::UUID(data.analyzeId);

  for(const auto &[objectKey, measureValues] : data.objects) {
    appender.BeginRow();
    appender.Append(uuid);
    appender.Append<uint64_t>(data.imageId);
    appender.Append<uint16_t>(static_cast<uint16_t>(data.channelId));
    appender.Append<uint32_t>(objectKey);
    appender.Append<uint16_t>(data.tileId);
    appender.Append<uint32_t>(static_cast<uint32_t>(measureValues.validity));
    // 0.02 ms
    auto mapToInsert =
        duckdb::Value::MAP(duckdb::LogicalType(duckdb::LogicalTypeId::UINTEGER),
                           duckdb::LogicalType(duckdb::LogicalTypeId::DOUBLE), measureValues.keys, measureValues.vals);

    appender.Append<duckdb::Value>(mapToInsert);    // 0.004ms

    appender.EndRow();
  }
  DurationCount::stop(id);

  // id = DurationCount::start("Close");    // 80ms

  appender.Close();
  // DurationCount::stop(id);
}

}    // namespace joda::results::db

// SELECT SUM(element_at(values, 0)[1]) as val_sum FROM test_with_idx.main."object" WHERE plate_id=1 AND well_id=1 AND
// image_id=10585059649949508029 AND channel_id=1

/*

SELECT SUM(element_at(values, 65536)[1]) as val_sum  FROM object INNER JOIN image_well ON
object.image_id=image_well.image_id WHERE object.image_id=4261282133957314495
*/

/*
SELECT SUM(element_at(values, 65536)[1]) as val_sum  FROM object INNER JOIN image_well ON
object.image_id=image_well.image_id WHERE image_well.well_id=266
*/
