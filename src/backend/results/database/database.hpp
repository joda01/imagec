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
#include <memory>
#include <string>
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
  void createAnalyze(const AnalyzeMeta &);
  void createPlate(const PlateMeta &);
  void createWell(const WellMeta &);
  void createImage(const ImageMeta &);
  void createChannel(const ChannelMeta &);
  void createImageChannel(const ImageChannelMeta &);
  void createObjects(const ObjectMeta &);

  template <typename... ARGS>
  std::unique_ptr<duckdb::QueryResult> select(const std::string &query, ARGS... args)
  {
    auto prep = mConnection->Prepare(query);
    return prep->Execute(std::forward<ARGS>(args)...);
  }

private:
  /////////////////////////////////////////////////////
  duckdb::DBConfig mDbCfg;
  std::unique_ptr<duckdb::DuckDB> mDb;
  std::unique_ptr<duckdb::Connection> mConnection;
};

}    // namespace joda::results::db
