#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct BlurSettings
{
public:
  int32_t kernelSize = 3;
  int32_t repeat     = 0;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK(kernelSize % 2 == 1, "Kernel size must be an odd number.");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(BlurSettings, repeat, kernelSize);
};
}    // namespace joda::settings
