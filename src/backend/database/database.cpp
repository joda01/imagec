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
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "backend/duration_count/duration_count.h"
#include "backend/helper/fnv1a.hpp"
#include "backend/logger/console_logger.hpp"
#include <duckdb/common/bind_helpers.hpp>
#include <duckdb/common/types.hpp>
#include <duckdb/common/types/vector.hpp>
#include <duckdb/main/appender.hpp>
#include <duckdb/main/connection.hpp>
#include <duckdb/main/database.hpp>
#include <duckdb.hpp>

namespace joda::db {
Database::Database(const std::string &dbFile) : mDbFile(dbFile)
{
}

///
/// \brief      Open database connection and create tables if not exist
/// \author     Joachim Danmayr
///
void Database::open()
{
  mDb         = new duckdb::DuckDB(mDbFile.data());
  mConnection = new duckdb::Connection(*mDb);

  // Command to create a table
  const char *create_table_sql =
      "CREATE TABLE IF NOT EXISTS experiment ("
      "	experiment_id UUID,"
      "	name TEXT,"
      " scientists TEXT[],"
      " datetime TIMESTAMP,"
      " location TEXT,"
      " notes TEXT,"
      " PRIMARY KEY (experiment_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS plate ("
      "	experiment_id UUID,"
      "	plate_id UTINYINT,"
      " datetime TIMESTAMP,"
      " notes TEXT,"
      " PRIMARY KEY (experiment_id, plate_id),"
      " FOREIGN KEY(experiment_id) REFERENCES experiment(experiment_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS well ("
      "	experiment_id UUID,"
      "	plate_id UTINYINT,"
      "	well_id USMALLINT,"
      " notes TEXT,"
      " PRIMARY KEY (experiment_id, plate_id, well_id),"
      " FOREIGN KEY(experiment_id, plate_id) REFERENCES plate(experiment_id, plate_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS image ("
      "	experiment_id UUID,"
      "	plate_id UTINYINT,"
      "	well_id USMALLINT,"
      "	image_id UBIGINT,"
      " file_name TEXT,"
      " width UHUGEINT,"
      " height UHUGEINT,"
      " PRIMARY KEY (experiment_id, plate_id, well_id, image_id),"
      " FOREIGN KEY(experiment_id, plate_id, well_id) REFERENCES well(experiment_id, plate_id, well_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS channel ("
      "	experiment_id UUID,"
      "	plate_id UTINYINT,"
      "	well_id USMALLINT,"
      "	image_id UBIGINT,"
      "	channel_id UTINYINT,"
      " PRIMARY KEY (experiment_id, plate_id, well_id, image_id, channel_id),"
      " FOREIGN KEY(experiment_id, plate_id, well_id, image_id) REFERENCES image(experiment_id, plate_id, well_id, "
      "image_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS object ("
      "	experiment_id UUID,"
      "	plate_id UTINYINT,"
      "	well_id USMALLINT,"
      "	image_id UBIGINT,"
      "	channel_id UTINYINT,"
      "	object_id UINTEGER,"
      " values MAP(UINTEGER, DOUBLE),"
      " PRIMARY KEY (experiment_id, plate_id, well_id, image_id, channel_id, object_id)"
      //" FOREIGN KEY(experiment_id, plate_id, well_id, image_id, channel_id) REFERENCES channel(experiment_id, "
      //"plate_id, "
      //"well_id, "
      //"image_id, channel_id)"
      ");";

  mConnection->Query(create_table_sql);
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
void Database::createExperiment(const ExperimentMeta &meta)
{
  auto prepare = mConnection->Prepare(
      "INSERT INTO experiment (experiment_id, name, scientists, datetime, location, notes) VALUES (gen_random_uuid(), "
      "?, ?, ?, ?, ?)");
  // Convert it to time since epoch
  auto timeNowMs = (duckdb::timestamp_t) std::chrono::duration_cast<std::chrono::microseconds>(
                       std::chrono::high_resolution_clock::now().time_since_epoch())
                       .count();

  prepare->Execute(duckdb::Value(meta.name), duckdb::Value::LIST({meta.scientists.begin(), meta.scientists.end()}),
                   duckdb::Value::TIMESTAMP(timeNowMs), duckdb::Value(meta.location), duckdb::Value(meta.notes));
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createPlate(const PlateMeta &meta)
{
  auto prepare =
      mConnection->Prepare("INSERT INTO plate (experiment_id, plate_id, datetime, notes) VALUES (?, ?, ?, ?)");

  auto timestamp = duckdb::timestamp_t(std::chrono::duration_cast<std::chrono::microseconds>(
                                           std::chrono::high_resolution_clock::now().time_since_epoch())
                                           .count());
  prepare->Execute(meta.experimentId, meta.plateId, duckdb::Value::TIMESTAMP(timestamp), meta.notes);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createWell(const WellMeta &meta)
{
  auto prepare = mConnection->Prepare("INSERT INTO well (experiment_id, plate_id, well_id, notes) VALUES (?, ?, ?, ?)");
  prepare->Execute(meta.experimentId, meta.plateId, meta.wellId, meta.notes);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createImage(const ImageMeta &meta)
{
  auto prepare = mConnection->Prepare(
      "INSERT INTO image (experiment_id, plate_id, well_id, image_id, file_name, width, height) VALUES "
      "(?, ?, ?, ?, ?, ?, ?)");
  prepare->Execute(meta.experimentId, meta.plateId, meta.wellId, fnv1a(meta.imageName), meta.imageName, meta.width,
                   meta.height);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createChannel(const ChannelMeta &meta)
{
  auto prepare = mConnection->Prepare(
      "INSERT INTO channel (experiment_id, plate_id, well_id, image_id, channel_id) VALUES "
      "(?, ?, ?, ?, ?)");

  prepare->Execute(meta.experimentId, meta.plateId, meta.wellId, fnv1a(meta.imageName), meta.imageName, meta.channelId);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createObjects(const ObjectMeta &data)
{
  duckdb::Appender appender(*mConnection, "object");

  // Loop to insert 100 elements
  uint64_t imageId = fnv1a(data.imageName);

  auto id = DurationCount::start("loop");    // 30ms

  for(const auto &[objectKey, measureValues] : data.objects) {
    appender.BeginRow();
    appender.Append<duckdb::string_t>(data.experimentId.data());
    appender.Append<uint8_t>(data.plateId);
    appender.Append<uint16_t>(data.wellId);
    appender.Append<uint64_t>(imageId);
    appender.Append<uint16_t>(data.channelId);
    appender.Append<uint32_t>(objectKey);

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

}    // namespace joda::db

// SELECT SUM(element_at(values, 0)[1]) as val_sum FROM test_with_idx.main."object" WHERE plate_id=1 AND well_id=1 AND
// image_id=10585059649949508029 AND channel_id=1
