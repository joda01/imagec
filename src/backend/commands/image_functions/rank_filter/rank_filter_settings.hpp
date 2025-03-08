#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct RankFilterSettings : public SettingBase
{
public:
  enum class Mode
  {
    MEAN       = 0,
    MIN        = 1,
    MAX        = 2,
    VARIANCE   = 3,
    MEDIAN     = 4,
    OUTLIERS   = 5,
    DESPECKLE  = 6,
    REMOVE_NAN = 7,
    OPEN       = 8,
    CLOSE      = 9,
    TOP_HAT    = 10
  };

  //
  //
  //
  Mode mode = Mode::MEAN;

  //
  //
  //
  float radius = 3;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(radius >= 0, "Radius must be bigger than 0.");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(RankFilterSettings, mode, radius);
};

NLOHMANN_JSON_SERIALIZE_ENUM(RankFilterSettings::Mode, {{RankFilterSettings::Mode::MEAN, "Mean"},
                                                        {RankFilterSettings::Mode::MIN, "Min"},
                                                        {RankFilterSettings::Mode::MAX, "Max"},
                                                        {RankFilterSettings::Mode::VARIANCE, "Variance"},
                                                        {RankFilterSettings::Mode::MEDIAN, "Median"},
                                                        {RankFilterSettings::Mode::OUTLIERS, "Outliers"},
                                                        {RankFilterSettings::Mode::DESPECKLE, "Despeckle"},
                                                        {RankFilterSettings::Mode::REMOVE_NAN, "RemoveNaN"},
                                                        {RankFilterSettings::Mode::OPEN, "Open"},
                                                        {RankFilterSettings::Mode::CLOSE, "Close"},
                                                        {RankFilterSettings::Mode::TOP_HAT, "TopHat"}});

}    // namespace joda::settings
