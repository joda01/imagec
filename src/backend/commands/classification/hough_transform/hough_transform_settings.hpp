///
/// \file      object_math_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
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
  enum class Shape
  {
    LINE_TRANSFORM,
    CIRCLE_TRANSFORM
  };

  struct CircleProperties
  {
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

    void check() const
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(CircleProperties, minCircleDistance, minCircleRadius, maxCircleRadius, param01, param02);
  };

  enums::ClassIdIn outputClass = enums::ClassIdIn::$;

  //
  //
  //
  Shape shape = Shape::CIRCLE_TRANSFORM;

  //
  //
  //
  CircleProperties circleProperties = {};

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

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(HoughTransformSettings, outputClass, shape, circleProperties);
};

NLOHMANN_JSON_SERIALIZE_ENUM(HoughTransformSettings::Shape, {
                                                                {HoughTransformSettings::Shape::LINE_TRANSFORM, "Line"},
                                                                {HoughTransformSettings::Shape::CIRCLE_TRANSFORM, "Circle"},

                                                            });

}    // namespace joda::settings
