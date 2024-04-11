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
    NONE,
    BALL,
    PARABOLOID
  };

  int32_t ballSize  = 0;
  BallType ballType = BallType::NONE;

private:
  NLOHMANN_JSON_SERIALIZE_ENUM(BallType,
                               {{BallType::NONE, ""}, {BallType::BALL, "Ball"}, {BallType::PARABOLOID, "Paraboloid"}});
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(RollingBall, ballSize, ballType);
};
}    // namespace joda::settings
