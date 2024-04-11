#pragma once

#include <cstdint>
#include <set>
#include <nlohmann/json.hpp>

namespace joda::settings {

class RankFilterMedianSubtract
{
public:
  int32_t kernelSize = 0;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(RankFilterMedianSubtract, kernelSize);
};
}    // namespace joda::settings
