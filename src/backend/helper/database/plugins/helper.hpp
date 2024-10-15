///
/// \file      common.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/settings/settings_types.hpp"

namespace joda::db {

class Database;

struct QueryFilter
{
  struct ObjectFilter
  {
    uint8_t plateId                                  = 0;
    uint16_t groupId                                 = 0;
    uint64_t imageId                                 = 0;
    uint16_t plateRows                               = 0;
    uint16_t plateCols                               = 0;
    uint32_t heatmapAreaSize                         = 200;
    std::vector<std::vector<int32_t>> wellImageOrder = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};
  };

  struct Channel
  {
    std::string clusterName;
    std::string className;
    std::map<enums::Measurement, std::set<enums::Stats>> measureChannels;
    std::map<int32_t, std::string> crossChannelStacksC;
  };

  using ChannelFilter  = std::pair<settings::ClassificatorSettingOut, Channel>;
  using ChannelFilters = std::map<settings::ClassificatorSettingOut, Channel>;

  db::Database *analyzer;
  ObjectFilter filter;
  ChannelFilters clustersToExport;
};

enum MeasureType
{
  OBJECT,
  INTENSITY
};

inline MeasureType getType(enums::Measurement measure)
{
  switch(measure) {
    case enums::Measurement::INTENSITY_SUM:
    case enums::Measurement::INTENSITY_AVG:
    case enums::Measurement::INTENSITY_MIN:
    case enums::Measurement::INTENSITY_MAX:
      return MeasureType::INTENSITY;
    case enums::Measurement::ORIGIN_OBJECT_ID:
    case enums::Measurement::CENTER_OF_MASS_X:
    case enums::Measurement::CENTER_OF_MASS_Y:
    case enums::Measurement::CONFIDENCE:
    case enums::Measurement::AREA_SIZE:
    case enums::Measurement::PERIMETER:
    case enums::Measurement::CIRCULARITY:
    case enums::Measurement::COUNT:
    case enums::Measurement::BOUNDING_BOX_WIDTH:
    case enums::Measurement::BOUNDING_BOX_HEIGHT:
      return MeasureType::OBJECT;
  }
  return MeasureType::OBJECT;
}

inline std::string getMeasurement(enums::Measurement measure)
{
  switch(measure) {
    case enums::Measurement::COUNT:
      return "1";
    case enums::Measurement::CENTER_OF_MASS_X:
      return "meas_center_x";
    case enums::Measurement::CENTER_OF_MASS_Y:
      return "meas_center_y";
    case enums::Measurement::CONFIDENCE:
      return "meas_confidence";
    case enums::Measurement::AREA_SIZE:
      return "meas_area_size";
    case enums::Measurement::PERIMETER:
      return "meas_perimeter";
    case enums::Measurement::CIRCULARITY:
      return "meas_circularity";
    case enums::Measurement::INTENSITY_SUM:
      return "meas_intensity_sum";
    case enums::Measurement::INTENSITY_AVG:
      return "meas_intensity_avg";
    case enums::Measurement::INTENSITY_MIN:
      return "meas_intensity_min";
    case enums::Measurement::INTENSITY_MAX:
      return "meas_intensity_max";
    case enums::Measurement::BOUNDING_BOX_WIDTH:
      return "meas_box_width";
    case enums::Measurement::BOUNDING_BOX_HEIGHT:
      return "meas_box_height";
    case enums::Measurement::ORIGIN_OBJECT_ID:
      return "meas_origin_object_id";
      break;
  }
  return "";
}

inline std::string getStatsString(enums::Stats stats)
{
  std::string statsStr;
  switch(stats) {
    case enums::Stats::AVG:
      statsStr = "  AVG";
      break;
    case enums::Stats::MEDIAN:
      statsStr = "  MEDIAN";
      break;
    case enums::Stats::SUM:
      statsStr = "  SUM";
      break;
    case enums::Stats::MIN:
      statsStr = "  MIN";
      break;
    case enums::Stats::MAX:
      statsStr = "  MAX";
      break;
    case enums::Stats::STDDEV:
      statsStr = "  STDDEV";
      break;
    case enums::Stats::CNT:
      statsStr = "  COUNT";
      break;
    case enums::Stats::OFF:
      statsStr = "  ANY_VALUE";
      break;
  };
  return statsStr;
}

inline std::map<uint32_t, std::string> createHeader(const QueryFilter::Channel &channel)
{
  std::map<uint32_t, std::string> columnHeaders;
  uint32_t col = 0;
  for(const auto &[measurement, stats] : channel.measureChannels) {
    for(const auto stat : stats) {
      if(getType(measurement) == MeasureType::INTENSITY) {
        for(const auto [cStack, _] : channel.crossChannelStacksC) {
          columnHeaders.emplace(col, channel.className + "-" + toString(measurement) + "[" + enums::toString(stat) + "] " + "(CH" +
                                         std::to_string(cStack) + ")");
          col++;
        }
      } else {
        columnHeaders.emplace(col, channel.className + "-" + toString(measurement) + "[" + enums::toString(stat) + "]");
        col++;
      }
    }
  }

  return columnHeaders;
}

}    // namespace joda::db
