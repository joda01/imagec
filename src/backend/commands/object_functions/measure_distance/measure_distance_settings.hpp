///
/// \file      measure_settings.hpp
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
#include <list>
#include <set>
#include <vector>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/global_enums.hpp"
#include "backend/processor/initializer/pipeline_settings.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct MeasureDistanceSettings : public SettingBase
{
  //
  // Objects to calc the distance from
  //
  ObjectInputClasss inputClasses;

  //
  // Objects to calc the distance to
  //
  ObjectInputClasss inputClassesSecond;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  settings::ObjectInputClasses getInputClasses() const override
  {
    settings::ObjectInputClasses classes;
    classes.emplace(inputClasses);
    classes.emplace(inputClassesSecond);

    return classes;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(MeasureDistanceSettings, inputClasses, inputClassesSecond);
};

}    // namespace joda::settings
