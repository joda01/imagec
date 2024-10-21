///
/// \file      settings_types.hpp
/// \author    Joachim Danmayr
/// \date      2024-09-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "setting.hpp"

namespace joda::settings {

struct ClassificatorSetting
{
  //
  // Cluster the objects should be assigned if filter matches
  //
  joda::enums::ClusterIdIn clusterId = joda::enums::ClusterIdIn::$;

  //
  // Class the objects should be assigned if filter matches
  //
  joda::enums::ClassIdIn classId = joda::enums::ClassIdIn::$;

  bool operator<(const ClassificatorSetting &input) const
  {
    auto toUint32 = [](enums::ClusterIdIn clu, enums::ClassIdIn cl) -> uint32_t {
      uint32_t out = (((uint16_t) clu) << 16) | (((uint16_t) cl));
      return out;
    };

    return toUint32(clusterId, classId) < toUint32(input.clusterId, input.classId);
  }

  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ClassificatorSetting, clusterId, classId);
};

struct ClassificatorSettingOut
{
  //
  // Cluster the objects should be assigned if filter matches
  //
  joda::enums::ClusterId clusterId = joda::enums::ClusterId::UNDEFINED;

  //
  // Class the objects should be assigned if filter matches
  //
  joda::enums::ClassId classId = joda::enums::ClassId::NONE;

  bool operator<(const ClassificatorSettingOut &input) const
  {
    return toUint32(clusterId, classId) < toUint32(input.clusterId, input.classId);
  }

  bool operator==(const ClassificatorSettingOut &input) const
  {
    return toUint32(clusterId, classId) == toUint32(input.clusterId, input.classId);
  }

  void check() const
  {
  }

  auto toUint32(enums::ClusterId clu, enums::ClassId cl) const -> uint32_t
  {
    uint32_t out = (((uint16_t) clu) << 16) | (((uint16_t) cl));
    return out;
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ClassificatorSettingOut, clusterId, classId);
};

using ObjectOutputClusters    = std::set<ClassificatorSetting>;
using ObjectInputClusters     = std::set<ClassificatorSetting>;
using ObjectInputCluster      = ClassificatorSetting;
using ObjectOutputCluster     = ClassificatorSetting;
using ObjectInputClustersExp  = std::set<ClassificatorSettingOut>;
using ObjectOutputClustersExp = std::set<ClassificatorSettingOut>;

}    // namespace joda::settings
