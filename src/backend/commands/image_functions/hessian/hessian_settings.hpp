#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct HessianSettings : public SettingBase
{
public:
  enum class Mode
  {
    Determinant,
    EigenvaluesX,
    EigenvaluesY,
  };

  //
  //
  //
  Mode mode = Mode::Determinant;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(HessianSettings, mode);
};

NLOHMANN_JSON_SERIALIZE_ENUM(HessianSettings::Mode, {
                                                        {HessianSettings::Mode::Determinant, "Determinant"},
                                                        {HessianSettings::Mode::EigenvaluesX, "EigenvaluesX"},
                                                        {HessianSettings::Mode::EigenvaluesY, "EigenvaluesY"},
                                                    });

}    // namespace joda::settings
