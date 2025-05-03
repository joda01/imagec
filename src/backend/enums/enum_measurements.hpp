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
  NONE      = -1,
  OBJECT_ID = 0,
  ORIGIN_OBJECT_ID,
  PARENT_OBJECT_ID,
  TRACKING_ID,
  COUNT,
  CONFIDENCE,
  AREA_SIZE,
  PERIMETER,
  CIRCULARITY,
  INTENSITY_SUM,
  INTENSITY_AVG,
  INTENSITY_MIN,
  INTENSITY_MAX,
  CENTEROID_X,
  CENTEROID_Y,
  BOUNDING_BOX_WIDTH,
  BOUNDING_BOX_HEIGHT,
  INTERSECTING,
  DISTANCE_CENTER_TO_CENTER,
  DISTANCE_CENTER_TO_SURFACE_MIN,
  DISTANCE_CENTER_TO_SURFACE_MAX,
  DISTANCE_SURFACE_TO_SURFACE_MIN,
  DISTANCE_SURFACE_TO_SURFACE_MAX,

};

NLOHMANN_JSON_SERIALIZE_ENUM(Measurement, {
                                              {Measurement::NONE, "None"},
                                              {Measurement::COUNT, "Count"},
                                              {Measurement::CONFIDENCE, "Confidence"},
                                              {Measurement::AREA_SIZE, "AreaSize"},
                                              {Measurement::PERIMETER, "Perimeter"},
                                              {Measurement::CIRCULARITY, "Circularity"},
                                              {Measurement::INTENSITY_SUM, "IntensitySum"},
                                              {Measurement::INTENSITY_AVG, "IntensityAvg"},
                                              {Measurement::INTENSITY_MIN, "IntensityMin"},
                                              {Measurement::INTENSITY_MAX, "IntensityMax"},
                                              {Measurement::CENTEROID_X, "CenteroidX"},
                                              {Measurement::CENTEROID_Y, "CenteroidY"},
                                              {Measurement::OBJECT_ID, "ObjectId"},
                                              {Measurement::ORIGIN_OBJECT_ID, "OriginObjectId"},
                                              {Measurement::PARENT_OBJECT_ID, "ParentObjectId"},
                                              {Measurement::TRACKING_ID, "TrackingId"},
                                              {Measurement::BOUNDING_BOX_WIDTH, "BoxWidth"},
                                              {Measurement::BOUNDING_BOX_HEIGHT, "BoxHeight"},
                                              {Measurement::INTERSECTING, "Intersecting"},
                                              {Measurement::DISTANCE_CENTER_TO_CENTER, "DistanceCentroidToCentoid"},
                                              {Measurement::DISTANCE_CENTER_TO_SURFACE_MIN, "DistanceCentroidToSurfaceMin"},
                                              {Measurement::DISTANCE_CENTER_TO_SURFACE_MAX, "DistanceCentroidToSurfaceMax"},
                                              {Measurement::DISTANCE_SURFACE_TO_SURFACE_MIN, "DistanceSurfaceToSurfaceMin"},
                                              {Measurement::DISTANCE_SURFACE_TO_SURFACE_MAX, "DistanceSurfaceToSurfaceMax"},
                                          });

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
    case Measurement::CENTEROID_X:
      return "x";
    case Measurement::CENTEROID_Y:
      return "y";
    case Measurement::OBJECT_ID:
      return "Object ID";
    case Measurement::ORIGIN_OBJECT_ID:
      return "Origin object ID";
    case Measurement::PARENT_OBJECT_ID:
      return "Parent object ID";
    case Measurement::TRACKING_ID:
      return "Tracking ID";
    case Measurement::INTERSECTING:
      return "Intersection";
    case Measurement::DISTANCE_CENTER_TO_CENTER:
      return "Dist. center-center";
    case Measurement::DISTANCE_CENTER_TO_SURFACE_MIN:
      return "Dist. center-surface min";
    case Measurement::DISTANCE_CENTER_TO_SURFACE_MAX:
      return "Dist. center-surface max";
    case Measurement::DISTANCE_SURFACE_TO_SURFACE_MIN:
      return "Dist. surface-surface min";
    case Measurement::DISTANCE_SURFACE_TO_SURFACE_MAX:
      return "Dist. surface-surface max";
  }
  return "";
}

}    // namespace joda::enums
