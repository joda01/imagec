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
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/fnv1a.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include <duckdb/common/bind_helpers.hpp>
#include <duckdb/common/types.hpp>
#include <duckdb/common/types/vector.hpp>
#include <duckdb/main/appender.hpp>
#include <duckdb/main/connection.hpp>
#include <duckdb/main/database.hpp>
#include <duckdb.hpp>

namespace joda::results::db {

Database::Database(const std::filesystem::path &dbFile) : mDb(dbFile.string()), mConnection(mDb)
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
      "CREATE TABLE IF NOT EXISTS job ("
      "	experiment_id UUID,"
      "	job_id UUID,"
      "	name TEXT,"
      " scientists TEXT[],"
      " datetime TIMESTAMP,"
      " location TEXT,"
      " notes TEXT,"
      " PRIMARY KEY (job_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS plate ("
      "	job_id UUID,"
      "	plate_id UTINYINT,"
      " datetime TIMESTAMP,"
      " notes TEXT,"
      " PRIMARY KEY (job_id, plate_id),"
      " FOREIGN KEY(job_id) REFERENCES job(job_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS well ("
      "	job_id UUID,"
      "	plate_id UTINYINT,"
      "	well_id USMALLINT,"
      "	well_pos_x UTINYINT,"
      "	well_pos_y UTINYINT,"
      " notes TEXT,"
      " PRIMARY KEY (job_id, plate_id, well_id),"
      " FOREIGN KEY(job_id, plate_id) REFERENCES plate(job_id, plate_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS image ("
      "	job_id UUID,"
      "	plate_id UTINYINT,"
      "	well_id USMALLINT,"
      "	image_id UINTEGER,"
      " file_name TEXT,"
      " width UHUGEINT,"
      " height UHUGEINT,"
      " PRIMARY KEY (job_id, plate_id, well_id, image_id),"
      " FOREIGN KEY(job_id, plate_id, well_id) REFERENCES well(job_id, plate_id, well_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS channel ("
      "	job_id UUID,"
      "	plate_id UTINYINT,"
      "	well_id USMALLINT,"
      "	image_id UINTEGER,"
      "	channel_id UTINYINT,"
      " PRIMARY KEY (job_id, plate_id, well_id, image_id, channel_id),"
      " FOREIGN KEY(job_id, plate_id, well_id, image_id) REFERENCES image(job_id, plate_id, well_id, "
      "image_id)"
      ");"

      "CREATE TABLE IF NOT EXISTS object ("
      "	job_id UUID,"
      "	plate_id UTINYINT,"
      "	well_id USMALLINT,"
      "	image_id UINTEGER,"
      "	channel_id UTINYINT,"
      "	object_id UINTEGER,"
      "	tile_id UTINYINT,"
      " validity UINTEGER,"
      " values MAP(UINTEGER, DOUBLE),"
      //" PRIMARY KEY (job_id, plate_id, well_id, image_id, channel_id, object_id)"
      //" FOREIGN KEY(job_id, plate_id, well_id, image_id, channel_id) REFERENCES channel(job_id, "
      //"plate_id, "
      //"well_id, "
      //"image_id, channel_id)"
      ");"
      "CREATE INDEX object_idx ON object (job_id, plate_id, well_id, image_id, channel_id);";

  mConnection.Query(create_table_sql);
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
void Database::createJob(const JobMeta &meta)
{
  auto prepare = mConnection.Prepare(
      "INSERT INTO job (experiment_id, job_id, name, scientists, datetime, location, notes) VALUES (?, ?, ?, ?, ?, ?, "
      "?)");
  // Convert it to time since epoch
  auto timeNowMs = (duckdb::timestamp_t) std::chrono::duration_cast<std::chrono::microseconds>(
                       std::chrono::high_resolution_clock::now().time_since_epoch())
                       .count();

  prepare->Execute(duckdb::Value(meta.experimentId), duckdb::Value(meta.jobId), duckdb::Value(meta.name),
                   duckdb::Value::LIST({meta.scientists.begin(), meta.scientists.end()}),
                   duckdb::Value::TIMESTAMP(timeNowMs), duckdb::Value(meta.location), duckdb::Value(meta.notes));
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createPlate(const PlateMeta &meta)
{
  auto prepare = mConnection.Prepare("INSERT INTO plate (job_id, plate_id, datetime, notes) VALUES (?, ?, ?, ?)");

  auto timestamp = duckdb::timestamp_t(std::chrono::duration_cast<std::chrono::microseconds>(
                                           std::chrono::high_resolution_clock::now().time_since_epoch())
                                           .count());
  prepare->Execute(meta.jobId, meta.plateId, duckdb::Value::TIMESTAMP(timestamp), meta.notes);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createWell(const WellMeta &meta)
{
  auto prepare = mConnection.Prepare(
      "INSERT INTO well (job_id, plate_id, well_id,well_pos_x,well_pos_y, notes) VALUES (?, ?, ?, ?)");
  prepare->Execute(meta.jobId, meta.plateId, meta.wellId, meta.wellPosX, meta.wellPosY, meta.notes);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createImage(const ImageMeta &meta)
{
  auto prepare = mConnection.Prepare(
      "INSERT INTO image (job_id, plate_id, well_id, image_id, file_name, width, height) VALUES "
      "(?, ?, ?, ?, ?, ?, ?)");
  prepare->Execute(meta.jobId, meta.plateId, meta.wellId, meta.imageId, meta.imageName, meta.width, meta.height);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createChannel(const ChannelMeta &meta)
{
  auto prepare = mConnection.Prepare(
      "INSERT INTO channel (job_id, plate_id, well_id, image_id, channel_id) VALUES "
      "(?, ?, ?, ?, ?)");

  prepare->Execute(meta.jobId, meta.plateId, meta.wellId, meta.imageId, meta.channelId);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createObjects(const ObjectMeta &data)
{
  duckdb::Appender appender(mConnection, "object");

  // Loop to insert 100 elements

  auto id = DurationCount::start("loop");    // 30ms

  for(const auto &[objectKey, measureValues] : data.objects) {
    appender.BeginRow();
    appender.Append<duckdb::string_t>(data.jobId.data());
    appender.Append<uint8_t>(data.plateId);
    appender.Append<uint16_t>(data.wellId);
    appender.Append<uint32_t>(data.imageId);
    appender.Append<uint16_t>(data.channelId);
    appender.Append<uint32_t>(objectKey);
    appender.Append<uint16_t>(data.tileId);
    appender.Append<uint32_t>(measureValues.validity);
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
