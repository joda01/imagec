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
#include <string>
#include <vector>
#include "backend/results/results_image_meta.hpp"
#include <duckdb/main/connection.hpp>
#include <duckdb/main/database.hpp>
#include <duckdb.hpp>

namespace joda::db {

struct ExperimentMeta
{
  std::string name;
  std::vector<std::string> scientists;
  std::string location;
  std::string notes;
};

struct PlateMeta
{
  std::string experimentId;
  uint8_t plateId;
  std::string notes;
};

struct WellMeta
{
  std::string experimentId;
  uint8_t plateId;
  uint16_t wellId;
  std::string notes;
};

struct ImageMeta
{
  std::string experimentId;
  uint8_t plateId;
  uint16_t wellId;
  std::string imageName;
  uint64_t width;
  uint64_t height;
};

struct ChannelMeta
{
  std::string experimentId;
  uint8_t plateId;
  uint16_t wellId;
  std::string imageName;
  uint64_t channelId;
};

enum class MeasureChannels : uint32_t
{
  CONFIDENCE       = 0,
  AREA_SIZE        = 1,
  PERIMETER        = 2,
  CIRCULARITY      = 3,
  VALIDITY         = 4,
  INVALIDITY       = 5,
  CENTER_OF_MASS_X = 6,
  CENTER_OF_MASS_Y = 7,
  INTENSITY_AVG    = 8,
  INTENSITY_MIN    = 9,
  INTENSITY_MAX    = 10,
  CROSS_CHANNEL_INTENSITY_AVG_00,
  CROSS_CHANNEL_INTENSITY_MIN_00,
  CROSS_CHANNEL_INTENSITY_MAX_00,
  CROSS_CHANNEL_INTENSITY_AVG_01,
  CROSS_CHANNEL_INTENSITY_MIN_01,
  CROSS_CHANNEL_INTENSITY_MAX_01,
  CROSS_CHANNEL_INTENSITY_AVG_02,
  CROSS_CHANNEL_INTENSITY_MIN_02,
  CROSS_CHANNEL_INTENSITY_MAX_02,
  CROSS_CHANNEL_INTENSITY_AVG_03,
  CROSS_CHANNEL_INTENSITY_MIN_03,
  CROSS_CHANNEL_INTENSITY_MAX_03,
  CROSS_CHANNEL_INTENSITY_AVG_04,
  CROSS_CHANNEL_INTENSITY_MIN_04,
  CROSS_CHANNEL_INTENSITY_MAX_04,
  CROSS_CHANNEL_INTENSITY_AVG_05,
  CROSS_CHANNEL_INTENSITY_MIN_05,
  CROSS_CHANNEL_INTENSITY_MAX_05,
  CROSS_CHANNEL_INTENSITY_AVG_06,
  CROSS_CHANNEL_INTENSITY_MIN_06,
  CROSS_CHANNEL_INTENSITY_MAX_06,
  CROSS_CHANNEL_INTENSITY_AVG_07,
  CROSS_CHANNEL_INTENSITY_MIN_07,
  CROSS_CHANNEL_INTENSITY_MAX_07,
  CROSS_CHANNEL_INTENSITY_AVG_08,
  CROSS_CHANNEL_INTENSITY_MIN_08,
  CROSS_CHANNEL_INTENSITY_MAX_08,
  CROSS_CHANNEL_INTENSITY_AVG_09,
  CROSS_CHANNEL_INTENSITY_MIN_09,
  CROSS_CHANNEL_INTENSITY_MAX_09,
  CROSS_CHANNEL_INTENSITY_AVG_0A,
  CROSS_CHANNEL_INTENSITY_MIN_0A,
  CROSS_CHANNEL_INTENSITY_MAX_0A,
  CROSS_CHANNEL_INTENSITY_AVG_0B,
  CROSS_CHANNEL_INTENSITY_MIN_0B,
  CROSS_CHANNEL_INTENSITY_MAX_0B,
  CROSS_CHANNEL_INTENSITY_AVG_0C,
  CROSS_CHANNEL_INTENSITY_MIN_0C,
  CROSS_CHANNEL_INTENSITY_MAX_0C,
  CROSS_CHANNEL_INTENSITY_AVG_0D,
  CROSS_CHANNEL_INTENSITY_MIN_0D,
  CROSS_CHANNEL_INTENSITY_MAX_0D,
  CROSS_CHANNEL_INTENSITY_AVG_0E,
  CROSS_CHANNEL_INTENSITY_MIN_0E,
  CROSS_CHANNEL_INTENSITY_MAX_0E,
  CROSS_CHANNEL_INTENSITY_AVG_0F,
  CROSS_CHANNEL_INTENSITY_MIN_0F,
  CROSS_CHANNEL_INTENSITY_MAX_0F,
  CROSS_CHANNEL_COUNT_00,
  CROSS_CHANNEL_COUNT_01,
  CROSS_CHANNEL_COUNT_02,
  CROSS_CHANNEL_COUNT_03,
  CROSS_CHANNEL_COUNT_04,
  CROSS_CHANNEL_COUNT_05,
  CROSS_CHANNEL_COUNT_06,
  CROSS_CHANNEL_COUNT_07,
  CROSS_CHANNEL_COUNT_08,
  CROSS_CHANNEL_COUNT_09,
  CROSS_CHANNEL_COUNT_0A,
  CROSS_CHANNEL_COUNT_0B,
  CROSS_CHANNEL_COUNT_0C,
  CROSS_CHANNEL_COUNT_0D,
  CROSS_CHANNEL_COUNT_0E,
  CROSS_CHANNEL_COUNT_0F,
  ARRAY_MAX
};

using objects_t = std::map<uint32_t, std::map<MeasureChannels, float>>;

struct ObjectMeta
{
  std::string experimentId;
  uint8_t plateId;
  uint16_t wellId;
  std::string imageName;
  uint16_t channelId;
  objects_t &objects;
};

class Database
{
public:
  /////////////////////////////////////////////////////
  Database(const std::string &dbFile);
  void open();
  void close();
  void createExperiment(const ExperimentMeta &);
  void createPlate(const PlateMeta &);
  void createWell(const WellMeta &);
  void createImage(const ImageMeta &);
  void createChannel(const ChannelMeta &);

  void createObjects(const ObjectMeta &);

private:
  /////////////////////////////////////////////////////
  duckdb::DuckDB *mDb;
  duckdb::Connection *mConnection;

  std::string mDbFile;
};

}    // namespace joda::db
