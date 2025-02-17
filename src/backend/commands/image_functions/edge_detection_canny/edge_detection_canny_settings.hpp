#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct EdgeDetectionCannySettings : public SettingBase
{
public:
  //
  // Kernel size
  //
  int32_t kernelSize = 3;

  //
  // Lower threshold for canny edge detection in range [0-255]
  //
  uint16_t thresholdMin = 10;

  //
  // Upper threshold for canny edge detection in range [0-255]
  //
  uint16_t thresholdMax = 150;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(EdgeDetectionCannySettings, kernelSize, thresholdMin, thresholdMax);
};

}    // namespace joda::settings
