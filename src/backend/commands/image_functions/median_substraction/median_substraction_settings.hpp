#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct MedianSubtractSettings : public SettingBase
{
public:
public:
  int32_t kernelSize = 3;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK(kernelSize % 2 == 1, "Kernel size must be an odd number.");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(MedianSubtractSettings, kernelSize);
};
}    // namespace joda::settings
