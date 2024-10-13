///
/// \file      enum_measurements.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include "backend/enums/types.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {

enum class Measurement
{
  COUNT,
  CONFIDENCE,
  AREA_SIZE,
  PERIMETER,
  CIRCULARITY,
  INTENSITY_SUM,
  INTENSITY_AVG,
  INTENSITY_MIN,
  INTENSITY_MAX,
  CENTER_OF_MASS_X,
  CENTER_OF_MASS_Y,
  ORIGIN_OBJECT_ID,
  BOUNDING_BOX_WIDTH,
  BOUNDING_BOX_HEIGHT
};

enum class Stats
{
  OFF,
  CNT,
  AVG,
  MAX,
  MIN,
  SUM,
  MEDIAN,
  STDDEV,
};

inline std::string toString(const Stats &enumIn)
{
  switch(enumIn) {
    case Stats::CNT:
      return "cnt";
    case Stats::AVG:
      return "avg";
    case Stats::MAX:
      return "max";
    case Stats::MIN:
      return "min";
    case Stats::SUM:
      return "sum";
    case Stats::MEDIAN:
      return "median";
    case Stats::STDDEV:
      return "stddev";
  }
  return "";
}

inline std::string toString(const Measurement &enumIn)
{
  switch(enumIn) {
    case Measurement::COUNT:
      return "Count";
    case Measurement::CONFIDENCE:
      return "Confidence";
    case Measurement::AREA_SIZE:
      return "Area size";
    case Measurement::PERIMETER:
      return "Perimeter";
    case Measurement::CIRCULARITY:
      return "Circularity";
    case Measurement::INTENSITY_SUM:
      return "Intensity sum";
    case Measurement::INTENSITY_AVG:
      return "Intensity avg";
    case Measurement::INTENSITY_MIN:
      return "Intensity min";
    case Measurement::INTENSITY_MAX:
      return "Intensity max";
    case Measurement::CENTER_OF_MASS_X:
      return "x";
    case Measurement::CENTER_OF_MASS_Y:
      return "y";
    case Measurement::ORIGIN_OBJECT_ID:
      return "Origin object ID";
      break;
  }
  return "";
}

}    // namespace joda::enums
