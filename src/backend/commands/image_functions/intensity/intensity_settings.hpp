#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct IntensityTransformationSettings : public SettingBase
{
public:
  enum class Mode
  {
    MANUAL,
    AUTOMATIC,
  };

  Mode mode = Mode::MANUAL;

  //
  // Contrast in range of [1-256]
  //
  float contrast = 1;
  //
  // Brightness in range of [-32768, +32767]
  //
  int32_t brightness = 0;

  //
  // Gamma value
  //
  int32_t gamma = 0;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(contrast >= 1 && contrast <= 256, "Contrast value must be between [0-256]");
    CHECK_ERROR(brightness >= -32768 && brightness <= 32767, "Brightness value must be between [-32768, +32767]");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(IntensityTransformationSettings, mode, contrast, brightness, gamma);
};

NLOHMANN_JSON_SERIALIZE_ENUM(IntensityTransformationSettings::Mode, {
                                                                        {IntensityTransformationSettings::Mode::MANUAL, "Manual"},
                                                                        {IntensityTransformationSettings::Mode::AUTOMATIC, "Automatic"},
                                                                    });

}    // namespace joda::settings
