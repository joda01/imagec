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
#include "backend/enums/enums_clusters.hpp"
#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ImageMathSettings : public SettingBase
{
  enum class Function
  {
    UNKNOWN,
    INVERT,
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    AND,
    OR,
    XOR,
    MIN,
    MAX,
    AVERAGE,
    DIFFERENCE
  };

  //
  // What should happen when an intersection was found
  //
  Function function = Function::INVERT;

  //
  // Objects to calc the intersection with
  //
  enums::ImageId inputImageSecond = {enums::ZProjection::$, {-1, -1, -1}};

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  settings::ObjectInputClusters getInputClusters() const override
  {
    return {};
  }

  [[nodiscard]] ObjectOutputClusters getOutputClasses() const override
  {
    return {};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ImageMathSettings, function, inputImageSecond);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ImageMathSettings::Function, {
                                                              {ImageMathSettings::Function::INVERT, "Invert"},
                                                              {ImageMathSettings::Function::ADD, "Add"},
                                                              {ImageMathSettings::Function::SUBTRACT, "Subtract"},
                                                              {ImageMathSettings::Function::MULTIPLY, "Multiply"},
                                                              {ImageMathSettings::Function::DIVIDE, "Divide"},
                                                              {ImageMathSettings::Function::AND, "AND"},
                                                              {ImageMathSettings::Function::OR, "OR"},
                                                              {ImageMathSettings::Function::XOR, "XOR"},
                                                              {ImageMathSettings::Function::MIN, "Min"},
                                                              {ImageMathSettings::Function::MAX, "Max"},
                                                              {ImageMathSettings::Function::AVERAGE, "Average"},
                                                              {ImageMathSettings::Function::DIFFERENCE, "Difference"},
                                                          });

}    // namespace joda::settings
