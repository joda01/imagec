///
/// \file      pipeline_indexes.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <cstdint>
#include "backend/enums/bigtypes.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {

using tile_t   = std::tuple<int32_t, int32_t>;
using zStack_t = int32_t;
using tStack_t = int32_t;
using cStack_t = int32_t;

struct PlaneId
{
  tStack_t tStack = -1;
  zStack_t zStack = -1;
  cStack_t cStack = -1;

  bool operator<(const PlaneId &in) const
  {
    return toInt(*this) < toInt(in);
  }

  void check() const
  {
  }

  auto toInt(const PlaneId &id) const -> stdi::uint128_t
  {
    stdi::uint128_t nr(static_cast<uint64_t>(id.cStack), static_cast<uint64_t>(id.zStack) << 32 | static_cast<uint64_t>(id.tStack));
    return nr;
  };

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(PlaneId, tStack, zStack, cStack);
};

struct HsvColor
{
  int32_t hue = 0;    // Color
  int32_t sat = 0;    // Saturation
  int32_t val = 0;    // Brightness

  void check() const
  {
    CHECK_ERROR(hue >= 0 && hue <= 359, "Hue must be in range [0-359]");
    CHECK_ERROR(sat >= 0 && sat <= 255, "Saturation must be in range [0-255]");
    CHECK_ERROR(val >= 0 && val <= 255, "Value must be in range [0-255]");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(HsvColor, hue, sat, val);
};

enum class ZProjection
{
  $             = -2,
  UNDEFINED     = -1,
  NONE          = 0,
  MAX_INTENSITY = 1,
  MIN_INTENSITY = 2,
  AVG_INTENSITY = 3,
  TAKE_MIDDLE   = 4,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ZProjection, {
                                              {ZProjection::$, "$"},
                                              {ZProjection::NONE, "None"},
                                              {ZProjection::MAX_INTENSITY, "MaxIntensity"},
                                              {ZProjection::MIN_INTENSITY, "MinIntensity"},
                                              {ZProjection::AVG_INTENSITY, "AvgIntensity"},
                                              {ZProjection::TAKE_MIDDLE, "TakeMiddle"},
                                          });

}    // namespace joda::enums
