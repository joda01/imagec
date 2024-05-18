///
/// \file      database.hpp
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

#pragma once

#include <duckdb.h>
#include <chrono>
#include <filesystem>
#include <string>
#include <vector>
#include <duckdb/main/connection.hpp>
#include <duckdb/main/database.hpp>
#include "database_interface.hpp"
#include <duckdb.hpp>

namespace joda::results::db {

class Database
{
public:
  /////////////////////////////////////////////////////
  explicit Database(const std::filesystem::path &dbFile);
  ~Database();
  Database(const Database &)            = delete;
  Database(Database &&)                 = delete;
  Database &operator=(const Database &) = delete;
  Database &operator=(Database &&)      = delete;
  void open();
  void close();
  void createJob(const JobMeta &);
  void createPlate(const PlateMeta &);
  void createWell(const WellMeta &);
  void createImage(const ImageMeta &);
  void createChannel(const ChannelMeta &);
  void createObjects(const ObjectMeta &);

private:
  /////////////////////////////////////////////////////
  duckdb::DuckDB mDb;
  duckdb::Connection mConnection;
};

}    // namespace joda::results::db
