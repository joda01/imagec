#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct EdgeDetectionSettings : public SettingBase
{
public:
  enum class Mode
  {
    OFF,
    CANNY,
    SOBEL
  };

  Mode mode = Mode::SOBEL;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(EdgeDetectionSettings, mode);
};

NLOHMANN_JSON_SERIALIZE_ENUM(EdgeDetectionSettings::Mode, {
                                                              {EdgeDetectionSettings::Mode::OFF, "Off"},
                                                              {EdgeDetectionSettings::Mode::CANNY, "Canny"},
                                                              {EdgeDetectionSettings::Mode::SOBEL, "Sobel"},
                                                          });

}    // namespace joda::settings
