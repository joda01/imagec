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

namespace joda::settings {

struct IntersectionSettings : public SettingBase
{
  enum class Function
  {
    UNKNOWN,
    COUNT,
    RECLASSIFY,
  };

  struct IntersectingClasses
  {
    //
    // Input object to intersect with. Leaf empty to use imagePlane context store
    //
    joda::enums::ObjectStoreId objectIn;

    //
    // Cluster to calculate the intersection with
    //
    joda::enums::ClusterIdIn clusterIn = joda::enums::ClusterIdIn::NONE;

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
  // What should happen when an intersection was found
  //
  Function mode = Function::UNKNOWN;

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

  //
  // In case of reclassification this is the new class ID for intersecting elements
  //
  joda::enums::ClassId newClassId = joda::enums::ClassId::UNDEFINED;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK(mode != Function::UNKNOWN, "Define a intersection function!");
    CHECK(minIntersection >= 0, "Min intersection must be >=0.");
    if(mode == Function::RECLASSIFY) {
      CHECK(newClassId != joda::enums::ClassId::UNDEFINED,
            "Define a class the elements should be assigned for reclassification.");
    }
  }

  std::set<enums::ClusterIdIn> getInputClusters() const override
  {
    std::set<enums::ClusterIdIn> clusters;
    clusters.emplace(objectsIn.clusterIn);
    clusters.emplace(objectsInWith.clusterIn);
    return clusters;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(IntersectionSettings, mode, minIntersection, objectsIn,
                                                       objectsInWith, newClassId);
};

NLOHMANN_JSON_SERIALIZE_ENUM(IntersectionSettings::Function,
                             {
                                 {IntersectionSettings::Function::COUNT, "Count"},
                                 {IntersectionSettings::Function::RECLASSIFY, "Reclassify"},
                             });

}    // namespace joda::settings
