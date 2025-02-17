#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct EdgeDetectionSobelSettings : public SettingBase
{
public:
  enum class WeightFunction
  {
    MAGNITUDE,
    ABS,
  };

  //
  // Used weight function
  //
  WeightFunction weighFunction = WeightFunction::MAGNITUDE;

  //
  // Kernel size
  //
  int32_t kernelSize = 3;

  //
  //
  //
  uint16_t derivativeOrderX = 1;

  //
  //
  //
  uint16_t derivativeOrderY = 1;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(EdgeDetectionSobelSettings, weighFunction, kernelSize, derivativeOrderX, derivativeOrderY);
};

NLOHMANN_JSON_SERIALIZE_ENUM(EdgeDetectionSobelSettings::WeightFunction, {
                                                                             {EdgeDetectionSobelSettings::WeightFunction::MAGNITUDE, "Magnitude"},
                                                                             {EdgeDetectionSobelSettings::WeightFunction::ABS, "Abs"},

                                                                         });

}    // namespace joda::settings
