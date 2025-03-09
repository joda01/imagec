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

struct EnhanceContrastSettings : public SettingBase
{
  //
  //
  //
  float saturatedPixels = 0.35;

  //
  //
  //
  bool normalize = false;

  //
  //
  //
  bool equalizeHistogram = false;

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

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(EnhanceContrastSettings, saturatedPixels, normalize, equalizeHistogram);
};

}    // namespace joda::settings
