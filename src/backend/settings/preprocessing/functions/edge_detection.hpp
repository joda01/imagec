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
    NONE  = -1,
    SOBEL = 1,
    CANNY = 2
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
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(EdgeDetection, mode, direction);
};
NLOHMANN_JSON_SERIALIZE_ENUM(EdgeDetection::Mode, {{EdgeDetection::Mode::NONE, ""},
                                                   {EdgeDetection::Mode::SOBEL, "Sobel"},
                                                   {EdgeDetection::Mode::CANNY, "Canny"}});
NLOHMANN_JSON_SERIALIZE_ENUM(EdgeDetection::Direction, {{EdgeDetection::Direction::XY, "XY"},
                                                        {EdgeDetection::Direction::X, "X"},
                                                        {EdgeDetection::Direction::Y, "Y"}});

}    // namespace joda::settings
