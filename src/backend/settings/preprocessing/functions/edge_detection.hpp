#pragma once

#include <cstdint>
#include <set>
#include <nlohmann/json.hpp>

namespace joda::settings {

class EdgeDetection
{
public:
  /////////////////////////////////////////////////////
  enum class Mode
  {
    NONE,
    SOBEL,
    CANNY
  };

  enum class Direction
  {
    XY,
    X,
    Y
  };

  Mode mode           = Mode::NONE;
  Direction direction = Direction::X;

private:
  NLOHMANN_JSON_SERIALIZE_ENUM(Mode, {{Mode::NONE, ""}, {Mode::SOBEL, "Sobel"}, {Mode::CANNY, "Canny"}});
  NLOHMANN_JSON_SERIALIZE_ENUM(Direction, {{Direction::XY, "XY"}, {Direction::X, "X"}, {Direction::Y, "Y"}});
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(EdgeDetection, mode, direction);
};
}    // namespace joda::settings
