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
#include "backend/duration_count/duration_count.h"
#include "backend/helper/fnv1a.hpp"
#include "backend/logger/console_logger.hpp"
#include <duckdb/common/bind_helpers.hpp>
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
  // Close the database connection
  duckdb_disconnect(&con);
  duckdb_close(&db);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createExperiment(const ExperimentMeta &meta)
{
  // Create SQL statement
  duckdb_prepared_statement stmt;
  duckdb_result result;
  if(duckdb_prepare(con,
                    "INSERT INTO experiment (experiment_id, name, scientists, datetime, location, notes) VALUES "
                    "(gen_random_uuid(), ?, "
                    "?, ?, ?, ?)",
                    &stmt) == DuckDBError) {
    throw ::std::runtime_error("Can't prepare statement!");
  }

  // Convert it to time since epoch
  auto timeNowMs = std::chrono::duration_cast<std::chrono::microseconds>(
                       std::chrono::high_resolution_clock::now().time_since_epoch())
                       .count();

  duckdb_bind_varchar(stmt, 1, meta.name.data());    // Name

  // Create a DuckDB list value for the array

  duckdb_logical_type listLogicType = duckdb_create_logical_type(DUCKDB_TYPE_VARCHAR);
  duckdb_value *listValuesEntry     = new duckdb_value[meta.scientists.size()];
  for(int n = 0; n < meta.scientists.size(); n++) {
    listValuesEntry[n] = duckdb_create_varchar(meta.scientists[n].data());
  }
  duckdb_value listValues = duckdb_create_list_value(listLogicType, listValuesEntry, meta.scientists.size());
  duckdb_bind_value(stmt, 2, listValues);    // Scientists
  duckdb_timestamp timestamp{.micros = timeNowMs};
  duckdb_bind_timestamp(stmt, 3, timestamp);             // Datetime
  duckdb_bind_varchar(stmt, 4, meta.location.data());    // Location
  duckdb_bind_varchar(stmt, 5, meta.notes.data());       // Notes

  if(duckdb_execute_prepared(stmt, NULL) == DuckDBError) {
    throw ::std::runtime_error("Cannot execute!");
  }

  duckdb_destroy_prepare(&stmt);

  duckdb_destroy_logical_type(&listLogicType);
  for(int n = 0; n < meta.scientists.size(); n++) {
    duckdb_destroy_value(&listValuesEntry[n]);
  }
  duckdb_destroy_value(&listValues);
  delete[] listValuesEntry;
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createPlate(const PlateMeta &meta)
{
  // Create SQL statement
  duckdb_prepared_statement stmt;
  duckdb_result result;
  if(duckdb_prepare(con, "INSERT INTO plate (experiment_id, plate_id, datetime, notes) VALUES (?, ?, ?, ?)", &stmt) ==
     DuckDBError) {
    throw ::std::runtime_error("Cannot prepare!");
  }

  duckdb_timestamp timestamp{.micros = std::chrono::duration_cast<std::chrono::microseconds>(
                                           std::chrono::high_resolution_clock::now().time_since_epoch())
                                           .count()};

  duckdb_bind_varchar(stmt, 1, meta.experimentId.data());    // Exp ID
  duckdb_bind_uint8(stmt, 2, meta.plateId);                  // Plate ID
  duckdb_bind_timestamp(stmt, 3, timestamp);                 // Datetime
  duckdb_bind_varchar(stmt, 4, meta.notes.data());           // Notes

  duckdb_result out_result;
  if(duckdb_execute_prepared(stmt, &out_result) == DuckDBError) {
    throw ::std::runtime_error(duckdb_result_error(&out_result));
  }
  duckdb_destroy_prepare(&stmt);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createWell(const WellMeta &meta)
{
  duckdb_prepared_statement stmt;
  duckdb_result result;
  // Create SQL statement
  if(duckdb_prepare(con, "INSERT INTO well (experiment_id, plate_id, well_id, notes) VALUES (?, ?, ?, ?)", &stmt) ==
     DuckDBError) {
  }

  duckdb_bind_varchar(stmt, 1, meta.experimentId.data());    // Exp ID
  duckdb_bind_uint8(stmt, 2, meta.plateId);                  // Plate ID
  duckdb_bind_uint16(stmt, 3, meta.wellId);                  // Datetime
  duckdb_bind_varchar(stmt, 4, meta.notes.data());           // Notes

  duckdb_result out_result;
  if(duckdb_execute_prepared(stmt, &out_result) == DuckDBError) {
    throw ::std::runtime_error(duckdb_result_error(&out_result));
  }
  duckdb_destroy_prepare(&stmt);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createImage(const ImageMeta &meta)
{
  duckdb_prepared_statement stmt;
  duckdb_result result;
  // Create SQL statement
  if(duckdb_prepare(con,
                    "INSERT INTO image (experiment_id, plate_id, well_id, image_id, file_name, width, height) VALUES "
                    "(?, ?, ?, ?, ?, ?, ?)",
                    &stmt) == DuckDBError) {
    throw ::std::runtime_error(duckdb_prepare_error(stmt));
  }

  duckdb_bind_varchar(stmt, 1, meta.experimentId.data());    // Exp ID
  duckdb_bind_uint8(stmt, 2, meta.plateId);                  // Plate ID
  duckdb_bind_uint16(stmt, 3, meta.wellId);                  // Datetime
  duckdb_bind_uint64(stmt, 4, fnv1a(meta.imageName));        // Datetime
  duckdb_bind_varchar(stmt, 5, meta.imageName.data());       // Notes
  duckdb_bind_uint64(stmt, 6, meta.width);                   // Datetime
  duckdb_bind_uint64(stmt, 7, meta.height);                  // Datetime

  duckdb_result out_result;
  if(duckdb_execute_prepared(stmt, &out_result) == DuckDBError) {
    throw ::std::runtime_error(duckdb_result_error(&out_result));
  }
  duckdb_destroy_prepare(&stmt);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createChannel(const ChannelMeta &meta)
{
  duckdb_prepared_statement stmt;
  duckdb_result result;
  // Create SQL statement
  if(duckdb_prepare(con,
                    "INSERT INTO channel (experiment_id, plate_id, well_id, image_id, channel_id) VALUES "
                    "(?, ?, ?, ?, ?)",
                    &stmt) == DuckDBError) {
    throw ::std::runtime_error(duckdb_prepare_error(stmt));
  }

  duckdb_bind_varchar(stmt, 1, meta.experimentId.data());    // Exp ID
  duckdb_bind_uint8(stmt, 2, meta.plateId);                  // Plate ID
  duckdb_bind_uint16(stmt, 3, meta.wellId);                  // Datetime
  duckdb_bind_uint64(stmt, 4, fnv1a(meta.imageName));        // Datetime
  duckdb_bind_uint16(stmt, 5, meta.channelId);               // Notes

  duckdb_result out_result;
  if(duckdb_execute_prepared(stmt, &out_result) == DuckDBError) {
    throw ::std::runtime_error(duckdb_result_error(&out_result));
  }
  duckdb_destroy_prepare(&stmt);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createObjects(const ObjectMeta &data)
{
  duckdb_appender appender;
  if(duckdb_appender_create(con, NULL, "object", &appender) == DuckDBError) {
    throw ::std::runtime_error(duckdb_appender_error(appender));
  }

  // Loop to insert 100 elements
  uint64_t imageId = fnv1a(data.imageName);

  // auto id = DurationCount::start("loop");    // 30ms

  for(const auto &[objectKey, measureValues] : data.objects) {
    duckdb_append_varchar(appender, data.experimentId.data());
    duckdb_append_uint8(appender, data.plateId);
    duckdb_append_uint16(appender, data.wellId);
    duckdb_append_uint64(appender, imageId);
    duckdb_append_uint16(appender, data.channelId);
    duckdb_append_uint32(appender, objectKey);

    auto channelSize = static_cast<uint32_t>(MeasureChannels::ARRAY_MAX);
    for(int n = 0; n < channelSize; n++) {
      if(measureValues.contains(static_cast<MeasureChannels>(n))) {
        duckdb_append_double(appender, measureValues.at(static_cast<MeasureChannels>(n)));
      } else {
        duckdb_append_null(appender);
      }
    }

    if(duckdb_appender_end_row(appender) == DuckDBError) {
      std::cout << duckdb_appender_error(appender) << std::endl;
    }
  }
  // DurationCount::stop(id);

  // id = DurationCount::start("Destroy");    // 80ms

  if(duckdb_appender_destroy(&appender) == DuckDBError) {
    std::cout << "Destroy error" << std::endl;
  }
  // DurationCount::stop(id);
}

}    // namespace joda::db
