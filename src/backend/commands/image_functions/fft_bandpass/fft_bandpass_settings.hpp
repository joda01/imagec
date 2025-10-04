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

struct FFTBandpassSettings : public SettingBase
{
  enum class StripeMode
  {
    NOTHING,
    HORIZONTAL,
    VERTICAL
  };

  //
  // Which to paint
  //
  StripeMode stripesHorVert = StripeMode::NOTHING;

  float toleranceOfDirection = 0.05F;

  bool doScaling    = true;
  bool doSaturation = true;

  float filterLargeStructure = 40;
  float filerSmallStructure  = 3;

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

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(FFTBandpassSettings, stripesHorVert);
};

NLOHMANN_JSON_SERIALIZE_ENUM(FFTBandpassSettings::StripeMode, {
                                                                  {FFTBandpassSettings::StripeMode::NOTHING, "InnerAndOuter"},
                                                                  {FFTBandpassSettings::StripeMode::HORIZONTAL, "Outer"},
                                                                  {FFTBandpassSettings::StripeMode::VERTICAL, "Inner"},

                                                              });

}    // namespace joda::settings
