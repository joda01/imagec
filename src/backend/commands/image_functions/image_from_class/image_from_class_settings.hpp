#pragma once

#include <cstdint>
#include <set>
#include <vector>
#include "backend/commands/setting.hpp"
#include "backend/global_enums.hpp"
#include <nlohmann/json.hpp>

namespace joda::cmd::functions {

struct ImageFromClassSettings : public Setting
{
public:
  //
  // Object classes to extract
  //
  std::set<joda::enums::ObjectClassId> objectClasses;

  /////////////////////////////////////////////////////
  void check() const override
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ImageFromClassSettings, objectClasses);
};
}    // namespace joda::cmd::functions
