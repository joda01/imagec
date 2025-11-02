#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct NopSettings : public SettingBase
{
public:
  int32_t repeat = 0;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(repeat >= 0, "Repeat must be a positive number.");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(NopSettings, repeat);
};

}    // namespace joda::settings
