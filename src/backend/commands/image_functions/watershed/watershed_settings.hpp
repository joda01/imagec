///
/// \file      watershed_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
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
#include "backend/settings/setting.hpp"
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
    CHECK_ERROR(maximumFinderTolerance >= 0 && maximumFinderTolerance <= 1,
                "Maximum finder tolerance mut be in range [0-1]");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(WatershedSettings, maximumFinderTolerance);
};

}    // namespace joda::settings
