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

enum class DistanceMeasureConditions
{
  ALL,
  ONLY_CHILDREN
};

struct MeasureDistanceSettings : public SettingBase
{
  //
  // Objects to calc the distance from
  //
  ObjectInputClasss inputClassFrom;

  //
  // Objects to calc the distance to
  //
  ObjectInputClasss inputClassTo;

  //
  // Condition for measuring the distance
  //
  DistanceMeasureConditions condition = DistanceMeasureConditions::ALL;

  //
  // Minimum intersection in [0-1]
  //
  float minIntersection = 0.1F;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  settings::ObjectInputClasses getInputClasses() const override
  {
    settings::ObjectInputClasses classes;
    classes.emplace(inputClassFrom);
    classes.emplace(inputClassTo);

    return classes;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(MeasureDistanceSettings, inputClassFrom, inputClassTo, condition, minIntersection);
};

NLOHMANN_JSON_SERIALIZE_ENUM(DistanceMeasureConditions, {
                                                            {DistanceMeasureConditions::ALL, "All"},
                                                            {DistanceMeasureConditions::ONLY_CHILDREN, "OnlyChildren"},
                                                        });

}    // namespace joda::settings
