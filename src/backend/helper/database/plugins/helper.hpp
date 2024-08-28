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

/*
inline std::string getStatsString(Stats stats)
{
  std::string statsStr;
  switch(stats) {
    case Stats::AVG:
      statsStr = "  AVG(element_at(values, $1)[1]) as val_img ";
      break;
    case Stats::MEDIAN:
      statsStr = "  MEDIAN(element_at(values, $1)[1]) as val_img ";
      break;
    case Stats::SUM:
      statsStr = "  SUM(element_at(values, $1)[1]) as val_img ";
      break;
    case Stats::MIN:
      statsStr = "  MIN(element_at(values, $1)[1]) as val_img ";
      break;
    case Stats::MAX:
      statsStr = "  MAX(element_at(values, $1)[1]) as val_img ";
      break;
    case Stats::STDDEV:
      statsStr = "  STDDEV(element_at(values, $1)[1]) as val_img ";
      break;
    case Stats::CNT:
      statsStr = "  COUNT(element_at(values, $1)[1]) as val_img ";
      break;
  };
  return statsStr;
}

inline std::string getAvgStatsFromStats(Stats stats)
{
  std::string statsStr;
  switch(stats) {
    case Stats::AVG:
      statsStr = "  AVG(element_at(avg, $1)[1]) as avg_of_avgs_per_group ";
      break;
    case Stats::MEDIAN:
      statsStr = "  AVG(element_at(median, $1)[1]) as avg_of_avgs_per_group ";
      break;
    case Stats::SUM:
      statsStr = "  AVG(element_at(sum, $1)[1]) as avg_of_avgs_per_group ";
      break;
    case Stats::MIN:
      statsStr = "  AVG(element_at(min, $1)[1]) as avg_of_avgs_per_group ";
      break;
    case Stats::MAX:
      statsStr = "  AVG(element_at(max, $1)[1]) as avg_of_avgs_per_group ";
      break;
    case Stats::STDDEV:
      statsStr = "  AVG(element_at(stddev, $1)[1]) as avg_of_avgs_per_group ";
      break;
    case Stats::CNT:
      statsStr = "  AVG(element_at(cnt, $1)[1]) as avg_of_avgs_per_group ";
      break;
  };
  return statsStr;
}*/

}    // namespace joda::db
