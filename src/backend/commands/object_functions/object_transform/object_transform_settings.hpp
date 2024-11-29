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
    SCALE
  };

  //
  // What should happen when an intersection was found
  //
  Function function = Function::SCALE;

  //
  // Objects to use for intersection calculation
  //
  ObjectInputClasss inputClasses;

  //
  // Factor
  //
  float scaleFactor = 1;

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
    return out;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ObjectTransformSettings, function, inputClasses, scaleFactor);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ObjectTransformSettings::Function, {
                                                                    {ObjectTransformSettings::Function::SCALE, "Scale"},
                                                                });

}    // namespace joda::settings
