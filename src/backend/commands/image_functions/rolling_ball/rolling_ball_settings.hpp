#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct RollingBallSettings : public SettingBase
{
public:
  enum class BallType
  {
    BALL,
    PARABOLOID
  };

  BallType ballType = BallType::BALL;
  int32_t ballSize  = 4;

  /////////////////////////////////////////////////////
  void check() const
  {
    CHECK_ERROR(ballSize > 0, "Ball size must be bigger than zero.");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(RollingBallSettings, ballType, ballSize);
};

NLOHMANN_JSON_SERIALIZE_ENUM(RollingBallSettings::BallType, {
                                                                {RollingBallSettings::BallType::BALL, "Ball"},
                                                                {RollingBallSettings::BallType::PARABOLOID, "Paraboloid"},
                                                            });

}    // namespace joda::settings
