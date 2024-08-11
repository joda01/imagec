#pragma once

#include <cstdint>
#include <set>
#include <nlohmann/json.hpp>

namespace joda::settings {

class MarginCrop
{
public:
  int32_t marginSize = 0;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(MarginCrop, marginSize);
};
}    // namespace joda::settings
