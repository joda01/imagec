#pragma once

#include <cstdint>
#include <set>
#include <nlohmann/json.hpp>

namespace joda::settings {

class Blur
{
public:
  int32_t kernelSize = 3;
  int32_t repeat     = 0;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Blur, repeat, kernelSize);
};
}    // namespace joda::settings
