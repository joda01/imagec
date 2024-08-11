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
/// \brief     A short description what happens here.
///

#pragma once

#include <memory>
#include "../pipeline_steps/functions/edge_detection.hpp"
#include "../pipeline_steps/functions/gaussian_blur.hpp"
#include "../pipeline_steps/functions/median_subtract.hpp"
#include "../pipeline_steps/functions/rolling_ball.hpp"
#include "../pipeline_steps/reader/load_image.hpp"
#include "backend/commands/classification/ai_classifier/ai_classifier_settings.hpp"
#include "backend/commands/classification/classifier/classifier_settings.hpp"
#include "backend/commands/command.hpp"
#include "backend/commands/functions/blur/blur_settings.hpp"
#include "backend/commands/functions/image_from_class/image_from_class_settings.hpp"
#include "backend/commands/functions/image_saver/image_saver_settings.hpp"
#include "backend/commands/functions/threshold/threshold_settings.hpp"
#include "backend/commands/functions/watershed/watershed_settings.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/json_optional_parser_helper.hpp"
#include "backend/settings/pipeline_steps/functions/calculator.hpp"
#include "backend/settings/pipeline_steps/functions/store_slot.hpp"
#include "backend/settings/pipeline_steps/functions/voronoi.hpp"
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

namespace joda::settings {

struct PipelineStep : public joda::cmd::Setting
{
public:
  //
  // The input slot used to execute the algorithm.
  // Use "$" to take the slot(s) from the step before.
  // Use a $store pipeline step to save an interim result.
  //
  joda::enums::Slot input = joda::enums::Slot::$;

  //
  // Common
  //
  // std::optional<StoreSlot> $store       = std::nullopt;
  // std::optional<Calculator> $calculator = std::nullopt;
  // std::optional<Voronoi> $voronoi = std::nullopt;    // Input of a voronoi is a set of points
  // std::optional<EdgeDetection> $edgeDetection                          = std::nullopt;
  // std::optional<GaussianBlur> $gaussianBlur                            = std::nullopt;
  // std::optional<MedianSubtraction> $medianSubtract                     = std::nullopt;
  // std::optional<RollingBall> $rollingBall                              = std::nullopt;

  std::optional<::joda::cmd::functions::BlurSettings> $blur                     = std::nullopt;
  std::optional<::joda::cmd::functions::ImageSaverSettings> $saveImage          = std::nullopt;
  std::optional<::joda::cmd::functions::ThresholdSettings> $threshold           = std::nullopt;
  std::optional<::joda::cmd::functions::WatershedSettings> $watershed           = std::nullopt;
  std::optional<::joda::cmd::functions::ImageFromClassSettings> $imageFromClass = std::nullopt;
  std::optional<::joda::cmd::functions::ClassifierSettings> $classify           = std::nullopt;
  std::optional<::joda::cmd::functions::AiClassifierSettings> $aiClassify       = std::nullopt;

  //
  // Measurement
  //
  // std::optional<MeasureSingle> $measure = std::nullopt;

  /////////////////////////////////////////////////////
  void operator()(processor::ProcessContext &context, cv::Mat &image, cmd::ObjectsListMap &result) const;
  void check() const override;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PipelineStep, input, $blur, $saveImage, $threshold, $watershed,
                                              $imageFromClass, $classify, $aiClassify);
};

}    // namespace joda::settings
