#pragma once

#include <cstdint>
#include <set>
#include <vector>
#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ImageFromClassSettings : public SettingBase
{
public:
  //
  // Cluster to extract
  //
  joda::enums::ClusterIdIn clusterIn = joda::enums::ClusterIdIn::$;

  //
  // Object classes to extract
  //
  std::set<joda::enums::ClassId> classesIn;

  /////////////////////////////////////////////////////
  void check()
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ImageFromClassSettings, clusterIn, classesIn);
};
}    // namespace joda::settings
