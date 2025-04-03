///
/// \file      enum_measurements.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include "backend/enums/types.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {

enum class Measurement
{
  NONE  = -1,
  COUNT = 0,
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
  OBJECT_ID,
  ORIGIN_OBJECT_ID,
  PARENT_OBJECT_ID,
  BOUNDING_BOX_WIDTH,
  BOUNDING_BOX_HEIGHT,
  INTERSECTING
};

NLOHMANN_JSON_SERIALIZE_ENUM(Measurement, {{Measurement::NONE, "None"},
                                           {Measurement::COUNT, "Count"},
                                           {Measurement::CONFIDENCE, "Confidence"},
                                           {Measurement::AREA_SIZE, "AreaSize"},
                                           {Measurement::PERIMETER, "Perimeter"},
                                           {Measurement::CIRCULARITY, "Circularity"},
                                           {Measurement::INTENSITY_SUM, "IntensitySum"},
                                           {Measurement::INTENSITY_AVG, "IntensityAvg"},
                                           {Measurement::INTENSITY_MIN, "IntensityMin"},
                                           {Measurement::INTENSITY_MAX, "IntensityMax"},
                                           {Measurement::CENTER_OF_MASS_X, "CenterOfMassX"},
                                           {Measurement::CENTER_OF_MASS_Y, "CenterOfMassY"},
                                           {Measurement::OBJECT_ID, "ObjectId"},
                                           {Measurement::ORIGIN_OBJECT_ID, "OriginObjectId"},
                                           {Measurement::PARENT_OBJECT_ID, "ParentObjectId"},
                                           {Measurement::BOUNDING_BOX_WIDTH, "BoxWidth"},
                                           {Measurement::BOUNDING_BOX_HEIGHT, "BoxHeight"},
                                           {Measurement::INTERSECTING, "Intersecting"}});

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

NLOHMANN_JSON_SERIALIZE_ENUM(Stats, {{Stats::OFF, "Off"},
                                     {Stats::CNT, "Cnt"},
                                     {Stats::AVG, "Avg"},
                                     {Stats::MAX, "Max"},
                                     {Stats::MIN, "Min"},
                                     {Stats::SUM, "Sum"},
                                     {Stats::MEDIAN, "Median"},
                                     {Stats::STDDEV, "Stddev"}});

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
    case Measurement::OBJECT_ID:
      return "Object ID";
    case Measurement::ORIGIN_OBJECT_ID:
      return "Origin object ID";
    case Measurement::PARENT_OBJECT_ID:
      return "Parent object ID";
    case Measurement::INTERSECTING:
      return "Intersection";
  }
  return "";
}

}    // namespace joda::enums
