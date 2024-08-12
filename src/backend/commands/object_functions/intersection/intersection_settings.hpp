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
#include <set>
#include <vector>
#include "backend/enums/enums_channels.hpp"
#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct IntersectionSettings : public Setting
{
  struct IntersectingClasses
  {
    //
    // Calc the intersection only with objects of given classes
    //
    std::set<joda::enums::ClassId> objectClasses;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(IntersectingClasses, objectClasses);
  };

  //
  // List of channels to calc the intersection for
  //
  std::map<joda::enums::ChannelId, IntersectingClasses> intersectingSlots;

  //
  // Minimum intersection in [0-1]
  //
  float minIntersection = 0.1F;

  //
  // Resulting object class of the intersecting objects
  //
  joda::enums::ClassId objectClass = joda::enums::ClassId::NONE;

  /////////////////////////////////////////////////////
  void check() const override
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(IntersectionSettings, intersectingSlots, minIntersection, objectClass);
};

}    // namespace joda::settings
