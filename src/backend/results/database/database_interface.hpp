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
#include "nlohmann/json.hpp"
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

struct Temp
{
  std::vector<std::vector<int32_t>> order;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Temp, order);
};
inline auto matrixStringToArrayOrder(const std::string &stringMatrix) -> std::vector<std::vector<int32_t>>
{
  std::vector<std::vector<int32_t>> wellImageOrder;
  try {
    nlohmann::json wellImageOrderJson = nlohmann::json::parse(stringMatrix);
    nlohmann::json obj;
    obj["order"]   = wellImageOrderJson;
    Temp tm        = nlohmann::json::parse(obj.dump());
    wellImageOrder = tm.order;
  } catch(...) {
    wellImageOrder.clear();
  }
  return wellImageOrder;
}

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
  std::string name;
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
}    // namespace joda::results::db
