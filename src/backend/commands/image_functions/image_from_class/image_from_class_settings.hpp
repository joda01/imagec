#pragma once

#include <cstdint>
#include <set>
#include <vector>
#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ImageFromClassSettings : public Setting
{
public:
  //
  // Object classes to extract
  //
  joda::enums::ClusterId objectCluster = joda::enums::ClusterId::$;

  //
  // Object classes to extract
  //
  std::set<joda::enums::ClassId> objectClasses;

  /////////////////////////////////////////////////////
  void check() const override
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ImageFromClassSettings, objectCluster, objectClasses);
};
}    // namespace joda::settings
