///
/// \file      classifier_settings.hpp
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
#include <list>
#include <set>
#include <vector>
#include "backend/enums/enum_objects.hpp"

#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include "backend/settings/settings_types.hpp"
#include <nlohmann/json.hpp>

// #warning "Allow multi intersection"

namespace joda::settings {

struct ColocalizationSettings : public SettingBase
{
  //
  // Classes to calculate the intersection with
  //
  ObjectInputClasses inputClasses;

  //
  // Minimum intersection in [0-1]
  //
  float minIntersection = 0.1F;

  //
  // Resulting object classs of the intersecting objects
  //
  enums::ClassIdIn outputClass;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(inputClasses.size() > 1, "At least two input objects must be given!");
    CHECK_ERROR(minIntersection >= 0 && minIntersection <= 1, "Min intersection must be between [0-1].");
  }

  settings::ObjectInputClasses getInputClasses() const override
  {
    settings::ObjectInputClasses classes;
    for(const auto classs : inputClasses) {
      classes.emplace(classs);
    }

    return classes;
  }

  [[nodiscard]] ObjectOutputClasses getOutputClasses() const override
  {
    return {outputClass};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_EXTENDED(ColocalizationSettings, inputClasses, minIntersection, outputClass);
};

}    // namespace joda::settings
