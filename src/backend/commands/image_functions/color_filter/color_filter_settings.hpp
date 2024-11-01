#pragma once

#include <cstdint>
#include <set>
#include <utility>
#include "backend/enums/types.hpp"
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
    //
    // With three filter points (defining hue, saturation and val)
    // a triangle on the HSV color circle is defined.
    // All colors wihin this triangle are accepted
    //
    joda::enums::HsvColor filterPointA{.hue = 223, .sat = 172, .val = 0};
    joda::enums::HsvColor filterPointB{.hue = 287, .sat = 200, .val = 0};
    joda::enums::HsvColor filterPointC{.hue = 62, .sat = 71, .val = 255};

    void check() const
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Filter, filterPointA, filterPointB, filterPointC);
  };

  std::list<Filter> filter;

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
