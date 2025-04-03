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

struct MorphologicalTransformSettings : public SettingBase
{
  enum class Function
  {
    UNKNOWN,
    ERODE,
    DILATE,
    OPEN,
    CLOSE,
    GRADIENT,
    TOPHAT,
    BLACKHAT,
    HITMISS
  };

  enum class Shape
  {
    RECTANGLE,
    CROSS,
    ELLIPSE
  };

  //
  // Method to use for morphological transformation
  //
  Function function = Function::CLOSE;

  //
  // Shape to use
  //
  Shape shape = Shape::ELLIPSE;

  //
  // Kernel size
  //
  int32_t kernelSize = 3;

  //
  // Iterations
  //
  int32_t iterations = -1;

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
    return {};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(MorphologicalTransformSettings, function, shape, kernelSize, iterations);
};

NLOHMANN_JSON_SERIALIZE_ENUM(MorphologicalTransformSettings::Function, {
                                                                           {MorphologicalTransformSettings::Function::UNKNOWN, "Unknown"},
                                                                           {MorphologicalTransformSettings::Function::ERODE, "Erode"},
                                                                           {MorphologicalTransformSettings::Function::DILATE, "Dilate"},
                                                                           {MorphologicalTransformSettings::Function::OPEN, "Open"},
                                                                           {MorphologicalTransformSettings::Function::CLOSE, "Close"},
                                                                           {MorphologicalTransformSettings::Function::GRADIENT, "Gradient"},
                                                                           {MorphologicalTransformSettings::Function::TOPHAT, "TopHat"},
                                                                           {MorphologicalTransformSettings::Function::BLACKHAT, "BlackHat"},
                                                                           {MorphologicalTransformSettings::Function::HITMISS, "Hitmiss"},
                                                                       });

NLOHMANN_JSON_SERIALIZE_ENUM(MorphologicalTransformSettings::Shape, {
                                                                        {MorphologicalTransformSettings::Shape::RECTANGLE, "Rectangle"},
                                                                        {MorphologicalTransformSettings::Shape::CROSS, "Cross"},
                                                                        {MorphologicalTransformSettings::Shape::ELLIPSE, "Ellipse"},
                                                                    });

}    // namespace joda::settings
