#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct MarginCropSettings
{
public:
  int32_t marginSize = 0;

  /////////////////////////////////////////////////////
  void check() const {CHECK(marginSize >= 0, "Margin to crop must be >=0.")}

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(MarginCropSettings, marginSize);
};

}    // namespace joda::settings