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

namespace joda::db {
Database::Database(const std::string &dbFile) : mDbFile(dbFile)
{
}

void Database::open()
{
  int rc = sqlite3_open(mDbFile.data(), &mDb);

  // Setting pragma for synchronous mode
  rc = sqlite3_exec(mDb, "PRAGMA synchronous = OFF;", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "Error setting pragma: " << sqlite3_errmsg(mDb) << std::endl;
  }

  rc = sqlite3_exec(mDb, "PRAGMA journal_mode = OFF;", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "Error setting pragma: " << sqlite3_errmsg(mDb) << std::endl;
  }

  rc = sqlite3_exec(mDb, "PRAGMA cache_size = 1000000;", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "Error setting pragma: " << sqlite3_errmsg(mDb) << std::endl;
  }

  rc = sqlite3_exec(mDb, "PRAGMA locking_mode = EXCLUSIVE;", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "Error setting pragma: " << sqlite3_errmsg(mDb) << std::endl;
  }

  rc = sqlite3_exec(mDb, "PRAGMA temp_store = MEMORY;", 0, 0, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "Error setting pragma: " << sqlite3_errmsg(mDb) << std::endl;
  }

  /*

  PRAGMA journal_mode = OFF;
  PRAGMA synchronous = 0;
  PRAGMA cache_size = 1000000;
  PRAGMA locking_mode = EXCLUSIVE;
  PRAGMA temp_store = MEMORY;
  */

  if(rc) {
    std::cout << "Can't open database: " << sqlite3_errmsg(mDb) << std::endl;
    sqlite3_close(mDb);
    return;
  }

  // SQLite command to create a table
  const char *create_table_sql =
      "CREATE TABLE IF NOT EXISTS `experiment` ("
      "	`id` integer,"
      "	`name` TEXT"
      ");"
      "CREATE TABLE IF NOT EXISTS `image` ("
      "	`id` INTEGER,"
      "	`experiment_id` INTEGER,"
      "	`name` TEXT"
      //"  FOREIGN KEY(`experiment_id`) REFERENCES `experiment`(`id`)"
      ");"
      "CREATE TABLE IF NOT EXISTS `channel` ("
      "  image_id INTEGER,"
      "	`channel_id` INTEGER,"
      "	`id` INTEGER,"
      "	`name` TEXT,"
      //"  FOREIGN KEY(`image_id`) REFERENCES `image`(`id`)"
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
      ") WITHOUT ROWID;";
  //"CREATE INDEX my_image_idx on image (id);"
  //"CREATE INDEX my_channel_idx on channel (id);"

  // Execute the SQL command to create a table
  char *error_message;
  rc = sqlite3_exec(mDb, create_table_sql, nullptr, nullptr, &error_message);

  if(rc != SQLITE_OK) {
    std::cerr << "SQL error: " << error_message << std::endl;
    sqlite3_free(error_message);
  } else {
    std::cout << "Table created successfully" << std::endl;
  }
}

void Database::close()
{
  // Close the database connection
  sqlite3_close(mDb);
}

void Database::addExperiment(int id, const std::string &name)
{
  // Create SQL statement
  std::string sql = "INSERT INTO experiment (id, name) VALUES (?, ?)";

  // Prepare the statement
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(mDb, sql.data(), -1, &stmt, nullptr);

  if(rc != SQLITE_OK) {
    std::cerr << "Error preparing SQL statement exp: " << sqlite3_errmsg(mDb) << std::endl;
    sqlite3_close(mDb);
    return;
  }

  // Loop to insert 100 elements
  // Bind your data to the statement here
  // You would bind your data using sqlite3_bind_* functions

  sqlite3_bind_int(stmt, 1, id);
  sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);

  // Execute the statement
  rc = sqlite3_step(stmt);

  if(rc != SQLITE_DONE) {
    std::cerr << "Error inserting data: " << sqlite3_errmsg(mDb) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(mDb);
    return;
  }

  // Reset the statement for the next iteration
  sqlite3_reset(stmt);

  // Finalize the statement
  sqlite3_finalize(stmt);
}
void Database::addImage(int experimentId, int id, const std::string &name)
{
  // Create SQL statement
  std::string sql = "INSERT INTO image (id, experiment_id, name) VALUES (?, ?, ?)";

  // Prepare the statement
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(mDb, sql.data(), -1, &stmt, nullptr);

  if(rc != SQLITE_OK) {
    std::cerr << "Error preparing SQL statement img: " << sqlite3_errmsg(mDb) << std::endl;
    sqlite3_close(mDb);
    return;
  }

  // Loop to insert 100 elements
  // Bind your data to the statement here
  // You would bind your data using sqlite3_bind_* functions

  sqlite3_bind_int(stmt, 1, id);
  sqlite3_bind_int(stmt, 2, experimentId);
  sqlite3_bind_text(stmt, 3, name.c_str(), -1, SQLITE_TRANSIENT);

  // Execute the statement
  rc = sqlite3_step(stmt);

  if(rc != SQLITE_DONE) {
    std::cerr << "Error inserting data: " << sqlite3_errmsg(mDb) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(mDb);
    return;
  }

  // Reset the statement for the next iteration
  sqlite3_reset(stmt);

  // Finalize the statement
  sqlite3_finalize(stmt);
}
void Database::addChannel(int imageId, int id, const std::string &name)
{
  // Create SQL statement
  std::string sql = "INSERT INTO channel (channel_id, image_id, name) VALUES (?, ?, ?)";

  // Prepare the statement
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(mDb, sql.data(), -1, &stmt, nullptr);

  if(rc != SQLITE_OK) {
    std::cerr << "Error preparing SQL statement ch: " << sqlite3_errmsg(mDb) << std::endl;
    sqlite3_close(mDb);
    return;
  }

  // Loop to insert 100 elements
  // Bind your data to the statement here
  // You would bind your data using sqlite3_bind_* functions

  sqlite3_bind_int(stmt, 1, id);
  sqlite3_bind_int(stmt, 2, imageId);
  sqlite3_bind_text(stmt, 3, name.c_str(), -1, SQLITE_TRANSIENT);

  // Execute the statement
  rc = sqlite3_step(stmt);

  if(rc != SQLITE_DONE) {
    std::cerr << "Error inserting data: " << sqlite3_errmsg(mDb) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(mDb);
    return;
  }

  // Reset the statement for the next iteration
  sqlite3_reset(stmt);

  // Finalize the statement
  sqlite3_finalize(stmt);
}

void Database::addObject(int expId, int imageId, int channelId, int num)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(1.0f, 1000.0f);

  sqlite3_exec(mDb, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

  // Create SQL statement
  std::string sql =
      "INSERT INTO object (object_id,experiment_id, image_id, channel_id,val_confidence, val_areasize, val_perimeter "
      ", val_circularity , val_validity , val_x ,"
      "val_y , val_box_width , val_box_height , val_intensity_min , val_intensity_max ,"
      "val_intensity_avg, val_intensity_cross_min_01, val_intensity_cross_max_01,"
      "val_intensity_cross_avg_01, val_intensity_cross_min_02, val_intensity_cross_max_02,"
      "val_intensity_cross_avg_02, val_intensity_cross_min_03, val_intensity_cross_max_03,"
      "val_intensity_cross_avg_03, val_intensity_cross_min_04, val_intensity_cross_max_04,"
      "val_intensity_cross_avg_04, val_intensity_cross_min_05, val_intensity_cross_max_05,"
      "val_intensity_cross_avg_05, val_intensity_cross_min_06, val_intensity_cross_max_06,"
      "val_intensity_cross_avg_06, val_intensity_cross_min_07, val_intensity_cross_max_07,"
      "val_intensity_cross_avg_07, val_intensity_cross_min_08, val_intensity_cross_max_08,"
      "val_intensity_cross_avg_08, val_intensity_cross_min_09, val_intensity_cross_max_09,"
      "val_intensity_cross_avg_09, val_intensity_cross_min_0A, val_intensity_cross_max_0A,"
      "val_intensity_cross_avg_0A, val_intensity_cross_min_0B, val_intensity_cross_max_0B,"
      "val_intensity_cross_avg_0B, val_intensity_cross_min_0C, val_intensity_cross_max_0C,"
      "val_intensity_cross_avg_0C, val_intensity_cross_min_0D, val_intensity_cross_max_0D,"
      "val_intensity_cross_avg_0D, val_intensity_cross_min_0E, val_intensity_cross_max_0E,"
      "val_intensity_cross_avg_0E, val_intensity_cross_min_0F, val_intensity_cross_max_0F,"
      "val_intensity_cross_avg_0F, val_count_cross_min_01, val_count_cross_max_01,"
      "val_count_cross_avg_01, val_count_cross_min_02, val_count_cross_max_02,"
      "val_count_cross_avg_02, val_count_cross_min_03, val_count_cross_max_03,"
      "val_count_cross_avg_03, val_count_cross_min_04, val_count_cross_max_04,"
      "val_count_cross_avg_04, val_count_cross_min_05, val_count_cross_max_05,"
      "val_count_cross_avg_05, val_count_cross_min_06, val_count_cross_max_06,"
      "val_count_cross_avg_06, val_count_cross_min_07, val_count_cross_max_07,"
      "val_count_cross_avg_07, val_count_cross_min_08, val_count_cross_max_08,"
      "val_count_cross_avg_08, val_count_cross_min_09, val_count_cross_max_09,"
      "val_count_cross_avg_09, val_count_cross_min_0A, val_count_cross_max_0A,"
      "val_count_cross_avg_0A, val_count_cross_min_0B, val_count_cross_max_0B,"
      "val_count_cross_avg_0B, val_count_cross_min_0C, val_count_cross_max_0C,"
      "val_count_cross_avg_0C, val_count_cross_min_0D, val_count_cross_max_0D,"
      "val_count_cross_avg_0D, val_count_cross_min_0E, val_count_cross_max_0E,"
      "val_count_cross_avg_0E, val_count_cross_min_0F, val_count_cross_max_0F,"
      "val_count_cross_avg_0F ) "
      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

  // Prepare the statement
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(mDb, sql.data(), -1, &stmt, nullptr);

  if(rc != SQLITE_OK) {
    std::cerr << "Error preparing SQL statement OBJ: " << sqlite3_errmsg(mDb) << std::endl;
    sqlite3_close(mDb);
    return;
  }

  // Loop to insert 100 elements

  for(int i = 0; i < num; ++i) {
    // Bind your data to the statement here
    // You would bind your data using sqlite3_bind_* functions
    sqlite3_bind_int(stmt, 1, i);
    sqlite3_bind_int(stmt, 2, expId);
    sqlite3_bind_int(stmt, 3, channelId);
    sqlite3_bind_int(stmt, 4, imageId);

    for(int n = 0; n < 100; n++) {
      float random_number = dis(gen);
      sqlite3_bind_double(stmt, 53 + n, random_number);
    }

    // sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE) {
      std::cerr << "Error inserting data: " << sqlite3_errmsg(mDb) << std::endl;
      sqlite3_finalize(stmt);
      sqlite3_close(mDb);
      return;
    }

    // Reset the statement for the next iteration
    sqlite3_reset(stmt);
  }

  // Finalize the statement
  sqlite3_finalize(stmt);

  sqlite3_exec(mDb, "COMMIT", nullptr, nullptr, nullptr);
}

}    // namespace joda::db
