///
/// \file      object_math_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <ATen/core/jit_type.h>
#include <cstdint>
#include <set>
#include <vector>
#include "backend/enums/enum_memory_idx.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct HoughTransformSettings : public SettingBase
{
  enum class HughMode
  {
    LINE_TRANSFORM,
    CIRCLE_TRANSFORM
  };

  enums::ClassIdIn outputClass = enums::ClassIdIn::$;

  //
  //
  //
  HughMode mode = HughMode::CIRCLE_TRANSFORM;

  //
  //
  //
  int32_t minCircleDistance = 20;

  //
  //
  //
  int32_t minCircleRadius = 0;

  //
  //
  //
  int32_t maxCircleRadius = 60;

  //
  //
  //
  float param01 = 150;

  //
  //
  //
  float param02 = 50;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  settings::ObjectInputClasses getInputClasses() const override
  {
    return {};
  }

  [[nodiscard]] ObjectOutputClasses getOutputClasses() const override
  {
    return {outputClass};
  }

  [[nodiscard]] std::set<enums::MemoryIdx::Enum> getInputImageCache() const override
  {
    return {};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(HoughTransformSettings, outputClass, mode, minCircleDistance, minCircleRadius, maxCircleRadius,
                                                       param01, param02);
};

NLOHMANN_JSON_SERIALIZE_ENUM(HoughTransformSettings::HughMode, {
                                                                   {HoughTransformSettings::HughMode::LINE_TRANSFORM, "LineTransform"},
                                                                   {HoughTransformSettings::HughMode::CIRCLE_TRANSFORM, "CircleTransform"},

                                                               });

}    // namespace joda::settings
