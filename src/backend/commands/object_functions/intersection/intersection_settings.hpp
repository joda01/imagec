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
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct IntersectionSettings : public SettingBase
{
  enum class Function
  {
    UNKNOWN,
    COUNT,
    RECLASSIFY,
    RECLASSIFY_COPY,
  };

  struct IntersectingClasses
  {
    //
    // Cluster to calculate the intersection with
    //
    ObjectInputClusters inputClusters;

    void check()
    {
      CHECK_ERROR(!inputClusters.empty(), "At least one class id must be given.");
      // CHECK_ERROR(clusterIn != joda::enums::ClusterId::NONE, "Input cluster ID must not be >NONE<.");
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(IntersectingClasses, inputClusters);
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
  IntersectingClasses inputObjects;

  //
  // Objects to calc the intersection with
  //
  IntersectingClasses inputObjectsIntersectWith;

  //
  // In case of reclassification this is the new class ID for intersecting elements
  //
  joda::enums::ClassId newClassId = joda::enums::ClassId::UNDEFINED;

  /////////////////////////////////////////////////////
  void check()
  {
    CHECK_ERROR(mode != Function::UNKNOWN, "Define a intersection function!");
    CHECK_ERROR(minIntersection >= 0, "Min intersection must be >=0.");
    if(mode == Function::RECLASSIFY || mode == Function::RECLASSIFY_COPY) {
      CHECK_ERROR(newClassId != joda::enums::ClassId::UNDEFINED,
                  "Define a class the elements should be assigned for reclassification.");
    }
  }

  std::set<enums::ClusterIdIn> getInputClusters() const override
  {
    std::set<enums::ClusterIdIn> clusters;
    for(const auto &in : inputObjects.inputClusters) {
      clusters.emplace(in.clusterId);
    }

    for(const auto &in : inputObjectsIntersectWith.inputClusters) {
      clusters.emplace(in.clusterId);
    }
    return clusters;
  }

  [[nodiscard]] ObjectOutputClusters getOutputClasses() const override
  {
    ObjectOutputClusters out;
    if(mode == Function::RECLASSIFY || mode == Function::RECLASSIFY_COPY) {
      for(const auto &in : inputObjectsIntersectWith.inputClusters) {
        out.emplace(in.clusterId, newClassId);
      }
    }
    return out;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(IntersectionSettings, mode, minIntersection, inputObjects,
                                                       inputObjectsIntersectWith, newClassId);
};

NLOHMANN_JSON_SERIALIZE_ENUM(IntersectionSettings::Function,
                             {
                                 {IntersectionSettings::Function::COUNT, "Count"},
                                 {IntersectionSettings::Function::RECLASSIFY, "ReclassifyMove"},
                                 {IntersectionSettings::Function::RECLASSIFY_COPY, "ReclassifyCopy"},
                             });

}    // namespace joda::settings
