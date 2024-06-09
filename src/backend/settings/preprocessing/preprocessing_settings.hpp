#pragma once

#include <cstdint>
#include <set>
#include "backend/helper/json_optional_parser_helper.hpp"
#include "functions/blur.hpp"
#include "functions/channel_subtaction.hpp"
#include "functions/edge_detection.hpp"
#include "functions/gaussian_blur.hpp"
#include "functions/median_subtract.hpp"
#include "functions/rolling_ball.hpp"
#include "functions/zstack.hpp"
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

namespace joda::settings {

class PreprocessingPipelineSteps
{
public:
  std::optional<ChannelSubtraction> $subtractChannel = std::nullopt;
  std::optional<EdgeDetection> $edgeDetection        = std::nullopt;
  std::optional<GaussianBlur> $gaussianBlur          = std::nullopt;
  std::optional<MedianSubtraction> $medianSubtract   = std::nullopt;
  std::optional<RollingBall> $rollingBall            = std::nullopt;
  std::optional<Blur> $blur                          = std::nullopt;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PreprocessingPipelineSteps, $subtractChannel, $edgeDetection,
                                              $gaussianBlur, $medianSubtract, $rollingBall, $blur);
};
}    // namespace joda::settings
