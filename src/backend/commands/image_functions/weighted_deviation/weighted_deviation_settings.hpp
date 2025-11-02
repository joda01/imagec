#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct WeightedDeviationSettings : public SettingBase
{
public:
  int32_t kernelSize = 5;
  double sigma       = 1.0;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(kernelSize % 2 == 1, "Kernel size must be an odd number.");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(WeightedDeviationSettings, sigma, kernelSize);
};

}    // namespace joda::settings
