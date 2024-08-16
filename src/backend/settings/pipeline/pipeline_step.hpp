///
/// \file      pipeline_step.hpp
/// \author    Joachim Danmayr
/// \date      2023-04-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <memory>
#include "backend/commands/classification/ai_classifier/ai_classifier_settings.hpp"
#include "backend/commands/classification/classifier/classifier_settings.hpp"
#include "backend/commands/image_functions/blur/blur_settings.hpp"
#include "backend/commands/image_functions/edge_detection/edge_detection_settings.hpp"
#include "backend/commands/image_functions/image_from_class/image_from_class_settings.hpp"
#include "backend/commands/image_functions/image_saver/image_saver_settings.hpp"
#include "backend/commands/image_functions/margin_crop/margin_crop_settings.hpp"
#include "backend/commands/image_functions/median_substraction/median_substraction_settings.hpp"
#include "backend/commands/image_functions/rolling_ball/rolling_ball_settings.hpp"
#include "backend/commands/image_functions/threshold/threshold_settings.hpp"
#include "backend/commands/image_functions/watershed/watershed_settings.hpp"
#include "backend/commands/object_functions/colocalization/colocalization_settings.hpp"
#include "backend/commands/object_functions/intersection/intersection_settings.hpp"
#include "backend/commands/object_functions/measure/measure_settings.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/json_optional_parser_helper.hpp"
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

namespace joda::processor {
class ProcessContext;
}

namespace joda::atom {
class ObjectList;
}

namespace joda::settings {

struct PipelineStep
{
public:
  //
  // Common
  //
  // std::optional<Voronoi> $voronoi = std::nullopt;    // Input of a voronoi is a set of points
  //

  std::optional<BlurSettings> $blur                     = std::nullopt;
  std::optional<ImageSaverSettings> $saveImage          = std::nullopt;
  std::optional<ThresholdSettings> $threshold           = std::nullopt;
  std::optional<WatershedSettings> $watershed           = std::nullopt;
  std::optional<ImageFromClassSettings> $imageFromClass = std::nullopt;
  std::optional<ClassifierSettings> $classify           = std::nullopt;
  std::optional<AiClassifierSettings> $aiClassify       = std::nullopt;
  std::optional<ColocalizationSettings> $colocalization = std::nullopt;
  std::optional<IntersectionSettings> $intersection     = std::nullopt;
  std::optional<MeasureSettings> $measure               = std::nullopt;
  std::optional<RollingBallSettings> $rollingBall       = std::nullopt;
  std::optional<MedianSubtractSettings> $medianSubtract = std::nullopt;
  std::optional<EdgeDetectionSettings> $edgeDetection   = std::nullopt;
  std::optional<MarginCropSettings> $crop               = std::nullopt;

  /////////////////////////////////////////////////////
  void operator()(processor::ProcessContext &context, cv::Mat &image, joda::atom::ObjectList &result) const;
  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(PipelineStep, $blur, $saveImage, $threshold, $watershed,
                                                       $imageFromClass, $classify, $aiClassify, $colocalization,
                                                       $intersection, $measure, $rollingBall, $medianSubtract,
                                                       $edgeDetection, $crop);
};

}    // namespace joda::settings
