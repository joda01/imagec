#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct SkeletonizeSettings : public SettingBase
{
public:
  enum class Mode
  {
    ZHANGSUEN,
    GUOHALL
  };

  Mode mode = Mode::ZHANGSUEN;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(SkeletonizeSettings, mode);
};

NLOHMANN_JSON_SERIALIZE_ENUM(SkeletonizeSettings::Mode, {
                                                            {SkeletonizeSettings::Mode::ZHANGSUEN, "Zhangsuen"},
                                                            {SkeletonizeSettings::Mode::GUOHALL, "Guohall"},
                                                        });

}    // namespace joda::settings
