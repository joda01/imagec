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
#include "backend/enums/enums_classes.hpp"
#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ObjectsToImageSettings : public SettingBase
{
  enum class Function
  {
    UNKNOWN,
    NONE,
    NOT,
    AND,
    OR,
    XOR,
    AND_NOT
  };

  //
  // What should happen when an intersection was found
  //
  Function function = Function::AND;

  //
  // Objects to use for intersection calculation
  //
  enums::ClassIdIn inputClassesFirst;

  //
  // Objects to calc the intersection with
  //
  enums::ClassIdIn inputClassesSecond = joda::enums::ClassIdIn::UNDEFINED;

  /////////////////////////////////////////////////////
  void check() const
  {
    if(function != Function::NOT && function != Function::NONE) {
      CHECK_ERROR(inputClassesSecond != joda::enums::ClassIdIn::UNDEFINED, "Object to image needs a second operand!");
    }
  }

  settings::ObjectInputClasses getInputClasses() const override
  {
    settings::ObjectInputClasses classes;
    classes.emplace(inputClassesFirst);

    if(function != Function::NOT && function != Function::NONE) {
      classes.emplace(inputClassesSecond);
    }

    return classes;
  }

  [[nodiscard]] ObjectOutputClasses getOutputClasses() const override
  {
    ObjectOutputClasses out;
    return out;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ObjectsToImageSettings, function, inputClassesFirst, inputClassesSecond);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ObjectsToImageSettings::Function, {
                                                                   {ObjectsToImageSettings::Function::NONE, "NONE"},
                                                                   {ObjectsToImageSettings::Function::NOT, "NOT"},
                                                                   {ObjectsToImageSettings::Function::AND, "AND"},
                                                                   {ObjectsToImageSettings::Function::AND_NOT, "AND-NOT"},
                                                                   {ObjectsToImageSettings::Function::OR, "OR"},
                                                                   {ObjectsToImageSettings::Function::XOR, "XOR"},
                                                               });

}    // namespace joda::settings
