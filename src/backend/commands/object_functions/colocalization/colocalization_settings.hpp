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
#include "backend/enums/enums_clusters.hpp"
#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

// #warning "Allow multi intersection"

namespace joda::settings {

struct ColocalizationSettings : public SettingBase
{
  //
  // Input object to intersect with. Leaf empty to use imagePlane context store
  //
  joda::enums::ObjectStoreId objectIn;

  //
  // Clusters to calculate the intersection with
  //
  ObjectInputClusters inputClusters;

  //
  // Minimum intersection in [0-1]
  //
  float minIntersection = 0.1F;

  //
  // Resulting object cluster of the intersecting objects
  //
  ObjectOutputClass outputCluster;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK(inputClusters.size() > 1, "At least two input objects must be given!");
    CHECK(minIntersection >= 0, "Min intersection must be >=0.");
  }

  std::set<enums::ClusterIdIn> getInputClusters() const override
  {
    std::set<enums::ClusterIdIn> clusters;
    for(const auto cluster : inputClusters) {
      clusters.emplace(cluster.clusterId);
    }

    return clusters;
  }

  [[nodiscard]] ObjectOutputClusters getOutputClasses() const override
  {
    return {{outputCluster}};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_EXTENDED(ColocalizationSettings, objectIn, inputClusters, minIntersection,
                                          outputCluster);
};

}    // namespace joda::settings
