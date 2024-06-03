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

namespace joda::results::db {

Database::Database(const std::filesystem::path &dbFile)
{
  // cfg.SetOption("memory_limit", "10GB");
  // cfg.SetOption("external_threads", 1);
  // mDbCfg.SetOption("threads", 1);
  mDbCfg.SetOption("temp_directory", dbFile.parent_path().string());
  mDb = std::make_unique<duckdb::DuckDB>(dbFile.string(), &mDbCfg);
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

      "CREATE TABLE IF NOT EXISTS group ("
      "	analyze_id UUID,"
      "	plate_id UTINYINT,"
      "	group_id USMALLINT,"
      "	well_pos_x USMALLINT,"
      "	well_pos_y USMALLINT,"
      " name TEXT, "
      " notes TEXT,"
      " PRIMARY KEY (analyze_id, plate_id, group_id),"
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

      "CREATE TABLE IF NOT EXISTS image_group ("
      "	analyze_id UUID,"
      "	image_id UHUGEINT,"
      "	plate_id UTINYINT,"
      "	group_id USMALLINT,"
      " PRIMARY KEY (analyze_id, image_id),"
      " FOREIGN KEY(analyze_id, image_id) REFERENCES image(analyze_id, image_id),"
      " FOREIGN KEY(analyze_id, plate_id, group_id) REFERENCES group(analyze_id, plate_id, group_id),"
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
      " validity UHUGEINT,"
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
      " validity UHUGEINT,"
      " values MAP(UINTEGER, DOUBLE)"
      ");";
  //"CREATE INDEX object_idx ON object (image_id, channel_id);";
  auto connection = acquire();
  auto result     = connection->Query(create_table_sql);
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
  auto connection = acquire();
  auto prepare    = connection->Prepare(
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
  auto connection = acquire();
  auto prepare    = connection->Prepare("INSERT INTO plate (analyze_id, plate_id, notes) VALUES (?, ?, ?)");

  auto timestamp = duckdb::timestamp_t(std::chrono::duration_cast<std::chrono::microseconds>(
                                           std::chrono::high_resolution_clock::now().time_since_epoch())
                                           .count());
  prepare->Execute(duckdb::Value::UUID(meta.analyzeId), meta.plateId, meta.notes);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createGroup(const GroupMeta &meta)
{
  auto connection = acquire();
  auto prepare    = connection->Prepare(
      "INSERT INTO group (analyze_id, plate_id, group_id,well_pos_x,well_pos_y, name, notes) VALUES (?, ?, ?, ?, ?, ?, "
         "?)");
  prepare->Execute(duckdb::Value::UUID(meta.analyzeId), meta.plateId, meta.groupId, meta.wellPosX, meta.wellPosY,
                   meta.name, meta.notes);
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createImage(const ImageMeta &meta)
{
  auto connection = acquire();
  {
    auto prepare = connection->Prepare(
        "INSERT INTO image (analyze_id, image_id, image_idx, file_name, original_image_path, width, height) VALUES "
        "(?, ?, ?, ?, ?, ?, ?)");
    prepare->Execute(duckdb::Value::UUID(meta.analyzeId), meta.imageId, meta.imageIdx,
                     convertPath(meta.originalImagePath.filename()), convertPath(meta.originalImagePath), meta.width,
                     meta.height);
  }

  {
    auto prepare = connection->Prepare(
        "INSERT INTO image_group (analyze_id, image_id, plate_id, group_id) VALUES "
        "(?, ?, ?, ?)");
    prepare->Execute(duckdb::Value::UUID(meta.analyzeId), meta.imageId, meta.plateId, meta.groupId);
  }
}

///
/// \brief      Close database connection
/// \author     Joachim Danmayr
///
void Database::createChannel(const ChannelMeta &meta)
{
  auto connection = acquire();

  auto prepare = connection->Prepare(
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
  auto connection = acquire();
  auto prepare    = connection->Prepare(
      "INSERT INTO channel_image (analyze_id, image_id, channel_id, control_image_path, validity, invalidateAll) "
         "VALUES "
         "(?, ?, ?, ?, ?, ?)");

  prepare->Execute(duckdb::Value::UUID(meta.analyzeId), meta.imageId, static_cast<uint8_t>(meta.channelId),
                   convertPath(meta.controlImagePath), duckdb::Value::UHUGEINT(meta.validity.to_ulong()),
                   meta.invalidateAll);
}

std::string Database::convertPath(const std::filesystem::path &pathIn)
{
  std::string path = pathIn.string();
  std::replace(path.begin(), path.end(), '\\', '/');
  return path;
}

}    // namespace joda::results::db

// SELECT SUM(element_at(values, 0)[1]) as val_sum FROM test_with_idx.main."object" WHERE plate_id=1 AND group_id=1 AND
// image_id=10585059649949508029 AND channel_id=1

/*

SELECT SUM(element_at(values, 65536)[1]) as val_sum  FROM object INNER JOIN image_group ON
object.image_id=image_group.image_id WHERE object.image_id=4261282133957314495
*/

/*
SELECT SUM(element_at(values, 65536)[1]) as val_sum  FROM object INNER JOIN image_group ON
object.image_id=image_group.image_id WHERE image_group.group_id=266
*/
