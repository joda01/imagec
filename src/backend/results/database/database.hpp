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
#include <sqlite3.h>
#include <chrono>
#include <filesystem>
#include <string>
#include <vector>
#include <duckdb/main/connection.hpp>
#include <duckdb/main/database.hpp>
#include <duckdb.hpp>

namespace joda::results::db {

struct JobMeta
{
  std::string experimentId;
  std::string jobId;
  std::string name;
  std::vector<std::string> scientists;
  std::string location;
  std::string notes;
};

struct PlateMeta
{
  std::string jobId;
  uint8_t plateId;
  std::string notes;
};

struct WellMeta
{
  std::string jobId;
  uint8_t plateId;
  uint16_t wellId;
  uint8_t wellPosX;
  uint8_t wellPosY;
  std::string notes;
};

struct ImageMeta
{
  std::string jobId;
  uint8_t plateId;
  uint16_t wellId;
  uint32_t imageId;
  std::string imageName;
  uint64_t width;
  uint64_t height;
};

struct ChannelMeta
{
  std::string jobId;
  uint8_t plateId;
  uint16_t wellId;
  uint32_t imageId;
  uint8_t channelId;
};

struct Data
{
  uint32_t validity;
  duckdb::vector<duckdb::Value> keys;
  duckdb::vector<duckdb::Value> vals;
};

using objects_t = std::map<uint32_t, Data>;

struct ObjectMeta
{
  std::string jobId;
  uint8_t plateId;
  uint16_t wellId;
  uint32_t imageId;
  uint8_t channelId;
  uint16_t tileId;
  objects_t &objects;
};

class Database
{
public:
  /////////////////////////////////////////////////////
  Database(const std::filesystem::path &dbFile);
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
