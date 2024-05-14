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

namespace joda::db {
Database::Database(const std::string &dbFile) : mDbFile(dbFile)
{
}

void Database::open()
{
  int rc = sqlite3_open(mDbFile.data(), &mDb);

  // Setting pragma for synchronous mode
  rc = sqlite3_exec(mDb, "PRAGMA synchronous = 0;", 0, 0, 0);
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
      "	`id` integer primary key NOT NULL UNIQUE,"
      "	`name` TEXT NOT NULL"
      ");"
      "CREATE TABLE IF NOT EXISTS `image` ("
      "	`id` INTEGER NOT NULL UNIQUE,"
      "	`experiment_id` INTEGER NOT NULL,"
      "	`name` TEXT NOT NULL,"
      "  FOREIGN KEY(`experiment_id`) REFERENCES `experiment`(`id`)"
      ");"
      "CREATE TABLE IF NOT EXISTS `channel` ("
      "	`id` INTEGER NOT NULL,"
      "	`image_id` INTEGER NOT NULL,"
      "	`name` TEXT NOT NULL,"
      "  FOREIGN KEY(`image_id`) REFERENCES `image`(`id`)"
      ");"
      "CREATE TABLE object ("
      "  object_id INTEGER NOT NULL PRIMARY KEY,"
      "  measure_ch_id INTEGER NOT NULL,"
      "  channel_id INTEGER NOT NULL,"
      "  image_id INTEGER NOT NULL,"
      "  value FLOAT NOT NULL,"
      "  FOREIGN KEY (channel_id) REFERENCES channel(id),"
      "  FOREIGN KEY (image_id) REFERENCES channel(image_id)"
      ");";

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
    std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(mDb) << std::endl;
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
    std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(mDb) << std::endl;
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
  std::string sql = "INSERT INTO channel (id, image_id, name) VALUES (?, ?, ?)";

  // Prepare the statement
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(mDb, sql.data(), -1, &stmt, nullptr);

  if(rc != SQLITE_OK) {
    std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(mDb) << std::endl;
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

void Database::addObject(int imageId, int channelId, int id, int measCh, double val)
{
  // Create SQL statement
  std::string sql = "INSERT INTO object (object_id, measure_ch_id, channel_id,image_id, value) VALUES (?, ?, ?, ?, ?)";

  // Prepare the statement
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(mDb, sql.data(), -1, &stmt, nullptr);

  if(rc != SQLITE_OK) {
    std::cerr << "Error preparing SQL statement: " << sqlite3_errmsg(mDb) << std::endl;
    sqlite3_close(mDb);
    return;
  }

  // Loop to insert 100 elements
  for(int i = 0; i < 40000000; ++i) {
    // Bind your data to the statement here
    // You would bind your data using sqlite3_bind_* functions

    sqlite3_bind_int(stmt, 1, i);
    sqlite3_bind_int(stmt, 2, measCh);
    sqlite3_bind_int(stmt, 3, channelId);
    sqlite3_bind_int(stmt, 4, imageId);
    sqlite3_bind_int(stmt, 5, val + i);

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
}

}    // namespace joda::db
