///
/// \file      export_heatmap_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-03-16
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
#include <nlohmann/json.hpp>

namespace joda::settings {

struct DensityMapSettings
{
  enum class ElementForm
  {
    CIRCLE,
    RECTANGLE
  };

  enum class HeatMapRangeMode
  {
    AUTO,
    MANUAL
  };

  //
  //
  //
  ElementForm form = ElementForm::CIRCLE;

  //
  //
  //
  HeatMapRangeMode heatmapRangeMode = HeatMapRangeMode::AUTO;

  //
  // If mode is manual, this is the min value
  //
  float heatmapRangeMin = -1;

  //
  // If mode is manual, this is the max value
  //
  float heatmapRangeMax = -1;

  //
  //
  //
  int32_t densityMapAreaSize = 4096;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(DensityMapSettings, form, heatmapRangeMode, heatmapRangeMin, heatmapRangeMax, densityMapAreaSize);
};

NLOHMANN_JSON_SERIALIZE_ENUM(DensityMapSettings::ElementForm, {
                                                                  {DensityMapSettings::ElementForm::CIRCLE, "Circle"},
                                                                  {DensityMapSettings::ElementForm::RECTANGLE, "Rectangle"},
                                                              });

NLOHMANN_JSON_SERIALIZE_ENUM(DensityMapSettings::HeatMapRangeMode, {
                                                                       {DensityMapSettings::HeatMapRangeMode::AUTO, "Automatic"},
                                                                       {DensityMapSettings::HeatMapRangeMode::MANUAL, "Manual"},
                                                                   });
}    // namespace joda::settings
