///
/// \file      database_interface.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include "backend/results/db_column_ids.hpp"
#include <duckdb/common/types.hpp>
#include <duckdb.hpp>

namespace joda::results::db {

struct AnalyzeMeta
{
  std::string runId;
  std::string analyzeId;
  std::string name;
  std::vector<std::string> scientists;
  std::chrono::system_clock::time_point timestamp;
  std::string location;
  std::string notes;
};

struct PlateMeta
{
  std::string analyzeId;
  uint8_t plateId;
  std::string notes;
};

struct WellMeta
{
  std::string analyzeId;
  uint8_t plateId;
  WellId wellId;
  uint8_t wellPosX;
  uint8_t wellPosY;
  std::string notes;
};

struct ImageMeta
{
  std::string analyzeId;
  uint8_t plateId;
  WellId wellId;
  uint64_t imageId;
  uint32_t imageIdx;
  std::filesystem::path originalImagePath;
  uint64_t width;
  uint64_t height;
};

struct ChannelMeta
{
  std::string analyzeId;
  ChannelIndex channelId;
  std::string name;
  std::vector<MeasureChannelId> measurements;
};

struct ImageChannelMeta
{
  std::string analyzeId;
  uint64_t imageId         = 0;
  ChannelIndex channelId   = ChannelIndex::ME;
  ChannelValidity validity = {};
  bool invalidateAll       = false;
  std::filesystem::path controlImagePath;
};

struct Data
{
  ObjectValidity validity = {};
  duckdb::vector<duckdb::Value> keys;
  duckdb::vector<duckdb::Value> vals;
};

using objects_t = std::map<uint32_t, Data>;

struct ObjectMeta
{
  std::string analyzeId;
  uint64_t imageId;
  ChannelIndex channelId;
  uint16_t tileId;
  objects_t &objects;
};
}    // namespace joda::results::db
