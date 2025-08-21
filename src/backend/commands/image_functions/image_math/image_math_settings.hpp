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
#include "backend/enums/enum_memory_idx.hpp"
#include "backend/enums/enum_objects.hpp"

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
    DIFFERENCE_TYPE
  };

  enum class OperationOrder
  {
    AoB,
    BoA
  };

  //
  // What should happen when an intersection was found
  //
  Function function = Function::INVERT;

  //
  // If either A-B or B-A should be calculated
  //
  OperationOrder operatorOrder = OperationOrder::AoB;

  //
  // Objects to calc the intersection with
  //
  enums::ImageId inputImageSecond = {enums::ZProjection::$, joda::enums::PlaneId{-1, -1, -1}, enums::MemoryIdx::NONE};

  //
  //
  //
  enums::MemoryScope memoryScope = enums::MemoryScope::ITERATION;

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
    return {};
  }

  [[nodiscard]] std::set<enums::MemoryIdx::Enum> getInputImageCache() const override
  {
    if(inputImageSecond.memoryId != enums::MemoryIdx::NONE) {
      return {inputImageSecond.memoryId};
    }
    return {};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ImageMathSettings, function, operatorOrder, inputImageSecond, memoryScope);
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
                                                              {ImageMathSettings::Function::DIFFERENCE_TYPE, "Difference"},
                                                          });

NLOHMANN_JSON_SERIALIZE_ENUM(ImageMathSettings::OperationOrder, {
                                                                    {ImageMathSettings::OperationOrder::AoB, "AoB"},
                                                                    {ImageMathSettings::OperationOrder::BoA, "BoA"},

                                                                });

}    // namespace joda::settings
