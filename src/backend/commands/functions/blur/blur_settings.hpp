#pragma once

#include <cstdint>
#include <set>
#include "backend/commands/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::cmd::functions {

struct BlurSettings : public Setting
{
public:
  int32_t kernelSize = 3;
  int32_t repeat     = 0;

  /////////////////////////////////////////////////////
  void check() const override
  {
    if(kernelSize % 2 == 0) {
      throwError("Kernel must be an od number!");
    }
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(BlurSettings, repeat, kernelSize);
};
}    // namespace joda::cmd::functions
