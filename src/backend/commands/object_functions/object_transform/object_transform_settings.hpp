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

#include <cstdint>
#include <set>
#include <vector>
#include "backend/enums/enum_objects.hpp"

#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ObjectTransformSettings : public SettingBase
{
  enum class Function
  {
    UNKNOWN,
    SCALE,
    SNAP_AREA,       // Use the object size and paint a circle with object_size+factor around it.
    MIN_CIRCLE,      // Paint a circle around the object with at least this radius. If the object is bigger, the object size is used.
    EXACT_CIRCLE,    // Paint a circle with exact the given radius.
    FIT_ELLIPSE      // Paint a ellipse fitting the contour of the element.

  };

  //
  // What should happen when an intersection was found
  //
  Function function = Function::SCALE;

  //
  // Objects to use for intersection calculation
  //
  ObjectInputClasss inputClasses = enums::ClassIdIn::$;

  //
  // Objects to use for intersection calculation
  //
  ObjectInputClasss outputClasses = enums::ClassIdIn::$;

  //
  // Factor
  //
  float factor = 1;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  settings::ObjectInputClasses getInputClasses() const override
  {
    settings::ObjectInputClasses classes;
    classes.emplace(inputClasses);

    return classes;
  }

  [[nodiscard]] ObjectOutputClasses getOutputClasses() const override
  {
    ObjectOutputClasses out;
    out.emplace(outputClasses);
    return out;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ObjectTransformSettings, function, inputClasses, outputClasses, factor);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ObjectTransformSettings::Function, {
                                                                    {ObjectTransformSettings::Function::SCALE, "Scale"},
                                                                    {ObjectTransformSettings::Function::SNAP_AREA, "SnapArea"},
                                                                    {ObjectTransformSettings::Function::MIN_CIRCLE, "CircleMin"},
                                                                    {ObjectTransformSettings::Function::EXACT_CIRCLE, "Circle"},
                                                                    {ObjectTransformSettings::Function::FIT_ELLIPSE, "FitEllipse"},
                                                                });

}    // namespace joda::settings
