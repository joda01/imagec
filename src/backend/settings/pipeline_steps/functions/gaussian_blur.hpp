#pragma once

#include <cstdint>
#include <set>
#include <nlohmann/json.hpp>

namespace joda::settings {

class GaussianBlur
{
public:
  int32_t kernelSize = 0;
  int32_t repeat     = 1;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(GaussianBlur, kernelSize, repeat);
};
}    // namespace joda::settings
