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
#include <iostream>
#include <random>
#include <string>
#include "backend/database/duckdb.hpp"

namespace joda::db {
Database::Database(const std::string &dbFile) : mDbFile(dbFile)
{
}

void Database::open()
{
  duckdb::DBConfig cfg;

  if(duckdb_open(mDbFile.data(), &db) == DuckDBError) {
    // handle error
    std::cout << "Error connect" << std::endl;
  }
  if(duckdb_connect(db, &con) == DuckDBError) {
    // handle error
    std::cout << "Error connect" << std::endl;
  }

  // SQLite command to create a table
  const char *create_table_sql =
      "CREATE TABLE IF NOT EXISTS experiment ("
      "	id integer,"
      "	name TEXT"
      ");"
      "CREATE TABLE IF NOT EXISTS image ("
      "	id INTEGER,"
      "	experiment_id INTEGER,"
      "	name TEXT"
      //"  FOREIGN KEY(experiment_id) REFERENCES experiment(id)"
      ");"
      "CREATE TABLE IF NOT EXISTS channel ("
      "  image_id INTEGER,"
      "	channel_id INTEGER,"
      "	id INTEGER,"
      "	name TEXT,"
      //"  FOREIGN KEY(image_id) REFERENCES image(id)"
      "  PRIMARY KEY (image_id, channel_id)"
      ");"
      "CREATE TABLE object ("
      "  object_id INTEGER,"
      "  experiment_id INTEGER,"
      "  image_id INTEGER,"
      "  channel_id INTEGER,"
      "  val_confidence FLOAT,"
      "  val_areasize FLOAT,"
      "  val_perimeter FLOAT,"
      "  val_circularity FLOAT,"
      "  val_validity FLOAT,"
      "  val_x FLOAT,"
      "  val_y FLOAT,"
      "  val_box_width FLOAT,"
      "  val_box_height FLOAT,"
      "  val_intensity_min FLOAT,"
      "  val_intensity_max FLOAT,"
      "  val_intensity_avg FLOAT,"
      "  val_intensity_cross_min_01 FLOAT,"
      "  val_intensity_cross_max_01 FLOAT,"
      "  val_intensity_cross_avg_01 FLOAT,"
      "  val_intensity_cross_min_02 FLOAT,"
      "  val_intensity_cross_max_02 FLOAT,"
      "  val_intensity_cross_avg_02 FLOAT,"
      "  val_intensity_cross_min_03 FLOAT,"
      "  val_intensity_cross_max_03 FLOAT,"
      "  val_intensity_cross_avg_03 FLOAT,"
      "  val_intensity_cross_min_04 FLOAT,"
      "  val_intensity_cross_max_04 FLOAT,"
      "  val_intensity_cross_avg_04 FLOAT,"
      "  val_intensity_cross_min_05 FLOAT,"
      "  val_intensity_cross_max_05 FLOAT,"
      "  val_intensity_cross_avg_05 FLOAT,"
      "  val_intensity_cross_min_06 FLOAT,"
      "  val_intensity_cross_max_06 FLOAT,"
      "  val_intensity_cross_avg_06 FLOAT,"
      "  val_intensity_cross_min_07 FLOAT,"
      "  val_intensity_cross_max_07 FLOAT,"
      "  val_intensity_cross_avg_07 FLOAT,"
      "  val_intensity_cross_min_08 FLOAT,"
      "  val_intensity_cross_max_08 FLOAT,"
      "  val_intensity_cross_avg_08 FLOAT,"
      "  val_intensity_cross_min_09 FLOAT,"
      "  val_intensity_cross_max_09 FLOAT,"
      "  val_intensity_cross_avg_09 FLOAT,"
      "  val_intensity_cross_min_0A FLOAT,"
      "  val_intensity_cross_max_0A FLOAT,"
      "  val_intensity_cross_avg_0A FLOAT,"
      "  val_intensity_cross_min_0B FLOAT,"
      "  val_intensity_cross_max_0B FLOAT,"
      "  val_intensity_cross_avg_0B FLOAT,"
      "  val_intensity_cross_min_0C FLOAT,"
      "  val_intensity_cross_max_0C FLOAT,"
      "  val_intensity_cross_avg_0C FLOAT,"
      "  val_intensity_cross_min_0D FLOAT,"
      "  val_intensity_cross_max_0D FLOAT,"
      "  val_intensity_cross_avg_0D FLOAT,"
      "  val_intensity_cross_min_0E FLOAT,"
      "  val_intensity_cross_max_0E FLOAT,"
      "  val_intensity_cross_avg_0E FLOAT,"
      "  val_intensity_cross_min_0F FLOAT,"
      "  val_intensity_cross_max_0F FLOAT,"
      "  val_intensity_cross_avg_0F FLOAT,"
      "  val_count_cross_min_01 FLOAT,"
      "  val_count_cross_max_01 FLOAT,"
      "  val_count_cross_avg_01 FLOAT,"
      "  val_count_cross_min_02 FLOAT,"
      "  val_count_cross_max_02 FLOAT,"
      "  val_count_cross_avg_02 FLOAT,"
      "  val_count_cross_min_03 FLOAT,"
      "  val_count_cross_max_03 FLOAT,"
      "  val_count_cross_avg_03 FLOAT,"
      "  val_count_cross_min_04 FLOAT,"
      "  val_count_cross_max_04 FLOAT,"
      "  val_count_cross_avg_04 FLOAT,"
      "  val_count_cross_min_05 FLOAT,"
      "  val_count_cross_max_05 FLOAT,"
      "  val_count_cross_avg_05 FLOAT,"
      "  val_count_cross_min_06 FLOAT,"
      "  val_count_cross_max_06 FLOAT,"
      "  val_count_cross_avg_06 FLOAT,"
      "  val_count_cross_min_07 FLOAT,"
      "  val_count_cross_max_07 FLOAT,"
      "  val_count_cross_avg_07 FLOAT,"
      "  val_count_cross_min_08 FLOAT,"
      "  val_count_cross_max_08 FLOAT,"
      "  val_count_cross_avg_08 FLOAT,"
      "  val_count_cross_min_09 FLOAT,"
      "  val_count_cross_max_09 FLOAT,"
      "  val_count_cross_avg_09 FLOAT,"
      "  val_count_cross_min_0A FLOAT,"
      "  val_count_cross_max_0A FLOAT,"
      "  val_count_cross_avg_0A FLOAT,"
      "  val_count_cross_min_0B FLOAT,"
      "  val_count_cross_max_0B FLOAT,"
      "  val_count_cross_avg_0B FLOAT,"
      "  val_count_cross_min_0C FLOAT,"
      "  val_count_cross_max_0C FLOAT,"
      "  val_count_cross_avg_0C FLOAT,"
      "  val_count_cross_min_0D FLOAT,"
      "  val_count_cross_max_0D FLOAT,"
      "  val_count_cross_avg_0D FLOAT,"
      "  val_count_cross_min_0E FLOAT,"
      "  val_count_cross_max_0E FLOAT,"
      "  val_count_cross_avg_0E FLOAT,"
      "  val_count_cross_min_0F FLOAT,"
      "  val_count_cross_max_0F FLOAT,"
      "  val_count_cross_avg_0F FLOAT,"
      //"  FOREIGN KEY (channel_id) REFERENCES channel(id),"
      //"  FOREIGN KEY (image_id) REFERENCES channel(image_id)"
      "  PRIMARY KEY (experiment_id, image_id, channel_id, object_id)"
      ");";
  //"CREATE INDEX my_image_idx on image (id);"
  //"CREATE INDEX my_channel_idx on channel (id);"

  auto state = duckdb_query(con, create_table_sql, NULL);
  if(state == DuckDBError) {
    // handle error
    std::cout << "Error create db" << std::endl;
  }
}

void Database::close()
{
  // Close the database connection
  duckdb_disconnect(&con);
  duckdb_close(&db);
}

void Database::addExperiment(int id, const std::string &name)
{
  // Create SQL statement
  duckdb_prepared_statement stmt;
  duckdb_result result;
  if(duckdb_prepare(con, "INSERT INTO experiment (id, name) VALUES (?, ?)", &stmt) == DuckDBError) {
    // handle error
  }

  duckdb_bind_int32(stmt, 1, id);    // Assuming the id is 1
  duckdb_bind_varchar(stmt, 2, name.data());

  duckdb_execute_prepared(stmt, NULL);
  duckdb_destroy_prepare(&stmt);
}
void Database::addImage(int experimentId, int id, const std::string &name)
{
  // Create SQL statement
  duckdb_prepared_statement stmt;
  duckdb_result result;
  if(duckdb_prepare(con, "INSERT INTO image (id, experiment_id, name) VALUES (?, ?, ?)", &stmt) == DuckDBError) {
    // handle error
  }

  duckdb_bind_int32(stmt, 1, id);              // Assuming the id is 1
  duckdb_bind_int32(stmt, 2, experimentId);    // Assuming the id is 1
  duckdb_bind_varchar(stmt, 3, name.data());

  duckdb_execute_prepared(stmt, NULL);
  duckdb_destroy_prepare(&stmt);
}
void Database::addChannel(int imageId, int id, const std::string &name)
{
  duckdb_prepared_statement stmt;
  duckdb_result result;
  // Create SQL statement
  if(duckdb_prepare(con, "INSERT INTO channel (channel_id, image_id, name) VALUES (?, ?, ?)", &stmt) == DuckDBError) {
  }

  duckdb_bind_int32(stmt, 1, id);         // Assuming the id is 1
  duckdb_bind_int32(stmt, 2, imageId);    // Assuming the id is 1
  duckdb_bind_varchar(stmt, 3, name.data());

  duckdb_execute_prepared(stmt, NULL);
  duckdb_destroy_prepare(&stmt);
}

void Database::addObject(int experimentId, int imageinId, int channelinId, int num)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(1.0f, 1000.0f);

  duckdb_appender appender;
  if(duckdb_appender_create(con, NULL, "object", &appender) == DuckDBError) {
    std::cout << "Error appender db" << std::endl;
    std::cout << duckdb_appender_error(appender) << std::endl;
    return;
  }

  // Loop to insert 100 elements

  for(int i = 0; i < num; ++i) {
    float randNr = dis(gen);

    duckdb_append_int32(appender, i);
    duckdb_append_int32(appender, experimentId);
    duckdb_append_int32(appender, imageinId);
    duckdb_append_int32(appender, channelinId);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    duckdb_append_float(appender, randNr);
    if(duckdb_appender_end_row(appender) == DuckDBError) {
      std::cout << "Error end row" << std::endl;
      std::cout << duckdb_appender_error(appender) << std::endl;
    }

    // sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
  }

  if(duckdb_appender_destroy(&appender) == DuckDBError) {
    std::cout << "Error insert" << std::endl;
  }
}

}    // namespace joda::db
