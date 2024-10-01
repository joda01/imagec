#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ColorFilterSettings : public SettingBase
{
public:
  std::string colorToKeep = "#FF0000";
  int32_t tolerance       = 30;

  //
  // Use 0.299 * R + 0.587 * G + 0.114 * B
  //
  bool useHumanPerceptionGrayscaleConversion = false;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ColorFilterSettings, colorToKeep, tolerance, useHumanPerceptionGrayscaleConversion);
};

}    // namespace joda::settings
