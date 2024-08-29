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

namespace joda::db {

class Database;

struct QueryFilter
{
  Database *analyzer;
  uint16_t plateRows  = 0;
  uint16_t plateCols  = 0;
  uint8_t plateId     = 0;
  uint16_t actGroupId = 0;
  uint64_t actImageId = 0;
  joda::enums::ClusterId clusterId;
  joda::enums::ClassId classId;
  std::string className;
  joda::enums::Measurement measurementChannel;
  joda::enums::Stats stats;
  std::vector<std::vector<int32_t>> wellImageOrder = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};
  uint32_t densityMapAreaSize                      = 200;

  uint32_t crossChanelStack_c;
  std::string crossChannelStack_cName;
  joda::enums::ClusterId crossChannelClusterId;
  std::string crossChannelClusterName;
  joda::enums::ClassId crossChannelClassId;
  std::string crossChannelClassName;
};

enum MeasureType
{
  OBJECT,
  INTENSITY,
  COUNT
};

inline MeasureType getType(enums::Measurement measure)
{
  switch(measure) {
    case enums::Measurement::INTENSITY_SUM:
    case enums::Measurement::INTENSITY_AVG:
    case enums::Measurement::INTENSITY_MIN:
    case enums::Measurement::INTENSITY_MAX:
      return MeasureType::INTENSITY;
    case enums::Measurement::INTERSECTING_CNT:
      return MeasureType::COUNT;
    default:
    case enums::Measurement::CENTER_OF_MASS_X:
    case enums::Measurement::CENTER_OF_MASS_Y:
    case enums::Measurement::CONFIDENCE:
    case enums::Measurement::AREA_SIZE:
    case enums::Measurement::PERIMETER:
    case enums::Measurement::CIRCULARITY:
      return MeasureType::OBJECT;
  }
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
  };
  return statsStr;
}

inline std::string createHeader(const QueryFilter &filter)
{
  std::string prefix;
  switch(getType(filter.measurementChannel)) {
    case OBJECT:
    case INTENSITY:
      prefix = " (CH" + std::to_string(filter.crossChanelStack_c) + ")";
      break;
    case COUNT:
      prefix = " (" + filter.crossChannelClusterName + "/" + filter.crossChannelClassName + ")";
      break;
  }

  return filter.className + " - " + toString(filter.measurementChannel) + " [" + enums::toString(filter.stats) + "]" +
         prefix;
}

}    // namespace joda::db
