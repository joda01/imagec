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
      " CONFIDENCE       DOUBLE, "
      " AREA_SIZE        DOUBLE, "
      " PERIMETER        DOUBLE, "
      " CIRCULARITY      DOUBLE, "
      " VALIDITY         DOUBLE, "
      " INVALIDITY       DOUBLE, "
      " CENTER_OF_MASS_X DOUBLE, "
      " CENTER_OF_MASS_Y DOUBLE, "
      " INTENSITY_AVG    DOUBLE, "
      " INTENSITY_MIN    DOUBLE, "
      " INTENSITY_MAX    DOUBLE, "
      " CROSS_CHANNEL_INTENSITY_AVG_00 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_00 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_00 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_01 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_01 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_01 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_02 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_02 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_02 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_03 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_03 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_03 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_04 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_04 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_04 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_05 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_05 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_05 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_06 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_06 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_06 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_07 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_07 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_07 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_08 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_08 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_08 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_09 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_09 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_09 DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_0A DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_0A DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_0A DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_0B DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_0B DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_0B DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_0C DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_0C DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_0C DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_0D DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_0D DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_0D DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_0E DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_0E DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_0E DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_AVG_0F DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MIN_0F DOUBLE,"
      " CROSS_CHANNEL_INTENSITY_MAX_0F DOUBLE,"
      " CROSS_CHANNEL_COUNT_00 DOUBLE,"
      " CROSS_CHANNEL_COUNT_01 DOUBLE,"
      " CROSS_CHANNEL_COUNT_02 DOUBLE,"
      " CROSS_CHANNEL_COUNT_03 DOUBLE,"
      " CROSS_CHANNEL_COUNT_04 DOUBLE,"
      " CROSS_CHANNEL_COUNT_05 DOUBLE,"
      " CROSS_CHANNEL_COUNT_06 DOUBLE,"
      " CROSS_CHANNEL_COUNT_07 DOUBLE,"
      " CROSS_CHANNEL_COUNT_08 DOUBLE,"
      " CROSS_CHANNEL_COUNT_09 DOUBLE,"
      " CROSS_CHANNEL_COUNT_0A DOUBLE,"
      " CROSS_CHANNEL_COUNT_0B DOUBLE,"
      " CROSS_CHANNEL_COUNT_0C DOUBLE,"
      " CROSS_CHANNEL_COUNT_0D DOUBLE,"
      " CROSS_CHANNEL_COUNT_0E DOUBLE,"
      " CROSS_CHANNEL_COUNT_0F DOUBLE,"
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

  // auto id = DurationCount::start("loop");    // 30ms

  for(const auto &[objectKey, measureValues] : data.objects) {
    appender.BeginRow();
    appender.Append<duckdb::string_t>(data.experimentId.data());
    appender.Append<uint8_t>(data.plateId);
    appender.Append<uint16_t>(data.wellId);
    appender.Append<uint64_t>(imageId);
    appender.Append<uint16_t>(data.channelId);
    appender.Append<uint32_t>(objectKey);

    auto channelSize = static_cast<uint32_t>(MeasureChannels::ARRAY_MAX);
    for(int n = 0; n < channelSize; n++) {
      if(measureValues.contains(static_cast<MeasureChannels>(n))) {
        appender.Append<double>(measureValues.at(static_cast<MeasureChannels>(n)));
      } else {
        appender.Append<std::nullptr_t>(nullptr);
      }
    }

    appender.EndRow();
  }
  // DurationCount::stop(id);

  // id = DurationCount::start("Destroy");    // 80ms

  appender.Close();
  // DurationCount::stop(id);
}

}    // namespace joda::db
