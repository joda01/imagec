#pragma once

#include <cstdint>
#include <set>
#include <nlohmann/json.hpp>

namespace joda::settings {

class RollingBall
{
public:
  enum class BallType
  {
    NONE       = -1,
    BALL       = 1,
    PARABOLOID = 2
  };

  int32_t ballSize  = 0;
  BallType ballType = BallType::NONE;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(RollingBall, ballSize, ballType);

  NLOHMANN_JSON_SERIALIZE_ENUM(RollingBall::BallType, {{RollingBall::BallType::NONE, ""},
                                                       {RollingBall::BallType::BALL, "Ball"},
                                                       {RollingBall::BallType::PARABOLOID, "Paraboloid"}});
};
}    // namespace joda::settings
