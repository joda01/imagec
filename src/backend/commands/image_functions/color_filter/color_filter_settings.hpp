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
  enum class GrayscaleMode
  {
    LINEAR,
    HUMAN
  };

  struct Filter
  {
    std::string lowerColor  = "#000000";
    std::string targetColor = "#000000";
    std::string upperColor  = "#000000";

    void check() const
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Filter, lowerColor, targetColor, upperColor);
  };

  std::vector<Filter> filter;

  //
  // Use 0.299 * R + 0.587 * G + 0.114 * B
  //
  GrayscaleMode grayScaleConvertMode = GrayscaleMode::LINEAR;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ColorFilterSettings, filter, grayScaleConvertMode);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ColorFilterSettings::GrayscaleMode, {
                                                                     {ColorFilterSettings::GrayscaleMode::LINEAR, "Linear"},
                                                                     {ColorFilterSettings::GrayscaleMode::HUMAN, "HumanEye"},
                                                                 });

}    // namespace joda::settings
