///
/// \file      watershed_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct WatershedSettings : public SettingBase
{
  //
  //
  //
  float maximumFinderTolerance = 0.5f;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(maximumFinderTolerance >= 0 && maximumFinderTolerance <= 1, "Maximum finder tolerance mut be in range [0-1]");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(WatershedSettings, maximumFinderTolerance);
};

}    // namespace joda::settings
