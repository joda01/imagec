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

///

#pragma once

#include <duckdb.h>
#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include "database_interface.hpp"

namespace joda::db {

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
  void createGroup(const GroupMeta &);
  void createImage(const ImageMeta &);
  void createChannel(const ChannelMeta &);
  void createImageChannel(const ImageChannelMeta &);

  template <typename... ARGS>
  std::unique_ptr<duckdb::QueryResult> select(const std::string &query, ARGS... args)
  {
    auto connection = acquire();
    auto prep       = connection->Prepare(query);
    return prep->Execute(std::forward<ARGS>(args)...);
  }

  std::shared_ptr<duckdb::Connection> acquire() const
  {
    std::shared_ptr<duckdb::Connection> connection = std::make_shared<duckdb::Connection>(*mDb);
    return connection;
  }

private:
  static std::string convertPath(const std::filesystem::path &);
  /////////////////////////////////////////////////////
  duckdb::DBConfig mDbCfg;
  std::unique_ptr<duckdb::DuckDB> mDb;
};

}    // namespace joda::db
