///
/// \file      pipeline_indexes.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include "backend/enums/types.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {

enum class ZProjection
{
  $             = -2,
  UNDEFINED     = -1,
  NONE          = 0,
  MAX_INTENSITY = 1,
  MIN_INTENSITY = 2,
  AVG_INTENSITY = 3
};

struct ImageId
{
  ZProjection zProjection = ZProjection::$;
  joda::enums::PlaneId imagePlane;

  bool operator<(const ImageId &in) const
  {
    __uint128_t plane1 = (imagePlane.toInt(imagePlane) << 8) | static_cast<uint8_t>(zProjection);
    __uint128_t plane2 = (in.imagePlane.toInt(in.imagePlane) << 8) | static_cast<uint8_t>(in.zProjection);

    return plane1 < plane2;
  }

  void check() const
  {
    CHECK_ERROR(zProjection != enums::ZProjection::UNDEFINED, "Define the z-projection mode for image loading!");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ImageId, zProjection, imagePlane);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ZProjection, {
                                              {ZProjection::$, "$"},
                                              {ZProjection::NONE, "None"},
                                              {ZProjection::MAX_INTENSITY, "MaxIntensity"},
                                              {ZProjection::MIN_INTENSITY, "MinIntensity"},
                                              {ZProjection::AVG_INTENSITY, "AvgIntensity"},
                                          });

}    // namespace joda::enums
