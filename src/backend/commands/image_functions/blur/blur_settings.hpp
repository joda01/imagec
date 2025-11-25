#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct BlurSettings : public SettingBase
{
public:
  enum class Mode
  {
    CONVOLVE,
    BLUR_MORE,
    FIND_EDGES,
    GAUSSIAN
  };

  Mode mode          = Mode::BLUR_MORE;
  int32_t kernelSize = 3;
  int32_t repeat     = 1;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(repeat >= 0, "Repeat must be a positive number.");
    CHECK_ERROR(kernelSize % 2 == 1, "Kernel size must be an odd number.");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(BlurSettings, mode, repeat, kernelSize);
};

NLOHMANN_JSON_SERIALIZE_ENUM(BlurSettings::Mode, {
                                                     {BlurSettings::Mode::BLUR_MORE, "Blur"},
                                                     {BlurSettings::Mode::GAUSSIAN, "GaussianBlur"},
                                                 });

}    // namespace joda::settings
