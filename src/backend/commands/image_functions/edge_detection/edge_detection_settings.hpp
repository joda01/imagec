#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct EdgeDetectionSettings : public SettingBase
{
public:
  enum class Mode
  {
    CANNY,
    SOBEL
  };

  Mode mode = Mode::SOBEL;

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

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(EdgeDetectionSettings, mode, kernelSize, thresholdMin, thresholdMax);
};

NLOHMANN_JSON_SERIALIZE_ENUM(EdgeDetectionSettings::Mode, {
                                                              {EdgeDetectionSettings::Mode::CANNY, "Canny"},
                                                              {EdgeDetectionSettings::Mode::SOBEL, "Sobel"},
                                                          });

}    // namespace joda::settings
