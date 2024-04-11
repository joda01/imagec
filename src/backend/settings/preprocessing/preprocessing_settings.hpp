#pragma once

#include <cstdint>
#include <set>
#include "functions/channel_subtaction.hpp"
#include "functions/edge_detection.hpp"
#include "functions/gaussian_blur.hpp"
#include "functions/rank_filter_median.hpp"
#include "functions/rolling_ball.hpp"
#include "functions/smoothing.hpp"
#include "functions/zstack.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

#include "backend/helper/json_optional_parser_helper.hpp"

class PreprocessingSteps
{
public:
  std::optional<ChannelSubtraction> $subtractChannel                = std::nullopt;
  std::optional<EdgeDetection> $edgeDetection                       = std::nullopt;
  std::optional<GaussianBlur> $gaussianBlur                         = std::nullopt;
  std::optional<RankFilterMedianSubtract> $RankFilterMedianSubtract = std::nullopt;
  std::optional<RollingBall> $rollingBall                           = std::nullopt;
  std::optional<Smoothing> $smooth                                  = std::nullopt;
  std::optional<ZStackProcessing> $zStack                           = std::nullopt;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(PreprocessingSteps, $edgeDetection);
};
}    // namespace joda::settings
