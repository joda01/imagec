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
#include "backend/enums/enum_objects.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

#warning "Allow multi intersection"

namespace joda::settings {

struct IntersectionSettings
{
  struct IntersectingClasses
  {
    //
    // Input object to intersect with. Leaf empty to use imagePlane context store
    //
    joda::enums::ObjectStoreId objectIn;

    //
    // Cluster to calculate the intersection with
    //
    joda::enums::ClusterId clusterIn = joda::enums::ClusterId::NONE;

    //
    // Classes within the cluster to calc the calculation with
    //
    std::set<joda::enums::ClassId> classesIn;

    void check() const
    {
      CHECK(!classesIn.empty(), "At least one class id must be given.");
      // CHECK(clusterIn != joda::enums::ClusterId::NONE, "Input cluster ID must not be >NONE<.");
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(IntersectingClasses, objectIn, clusterIn, classesIn);
  };

  //
  // Minimum intersection in [0-1]
  //
  float minIntersection = 0.1F;

  //
  // Objects to use for intersection calculation
  //
  IntersectingClasses objectsIn;

  //
  // Objects to calc the intersection with
  //
  IntersectingClasses objectsInWith;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK(minIntersection >= 0, "Min intersection must be >=0.");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_EXTENDED(IntersectionSettings, objectsIn, objectsInWith);
};

}    // namespace joda::settings
