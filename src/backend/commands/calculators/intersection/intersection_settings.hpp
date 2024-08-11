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
/// \brief     A short description what happens here.
///

#pragma once

#include <cstdint>
#include <set>
#include "backend/commands/setting.hpp"
#include "backend/global_enums.hpp"
#include <nlohmann/json.hpp>

namespace joda::cmd::functions {

struct IntersectionSettings : public Setting
{
  enum Mode
  {
    STORE_INTERSECTING_OBJECTS_AS_RESULT,
    REMOVE_INTERSECTING_OBJECTS_FROM_ORIGIN
  };

  //
  // List of channels to calc the intersection for
  //
  std::set<joda::enums::Slot> intersectingSlots;

  //
  // Minimum intersection in [0-1]
  //
  float minIntersection = 0.1F;

  /////////////////////////////////////////////////////
  void check() const override
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(IntersectionSettings, intersectingSlots, minIntersection);
};

}    // namespace joda::cmd::functions
