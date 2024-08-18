///
/// \file      pipeline_step.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "pipeline_step.hpp"
#include <memory>
#include "backend/commands/classification/ai_classifier/ai_classifier.hpp"
#include "backend/commands/classification/classifier/classifier.hpp"
#include "backend/commands/factory.hpp"
#include "backend/commands/image_functions/blur/blur.hpp"
#include "backend/commands/image_functions/edge_detection/edge_detection.hpp"
#include "backend/commands/image_functions/image_from_class/image_from_class.hpp"
#include "backend/commands/image_functions/image_saver/image_saver.hpp"
#include "backend/commands/image_functions/margin_crop/margin_crop.hpp"
#include "backend/commands/image_functions/margin_crop/margin_crop_settings.hpp"
#include "backend/commands/image_functions/median_substraction/median_substraction.hpp"
#include "backend/commands/image_functions/rolling_ball/rolling_ball.hpp"
#include "backend/commands/image_functions/rolling_ball/rolling_ball_settings.hpp"
#include "backend/commands/image_functions/threshold/threshold.hpp"
#include "backend/commands/image_functions/watershed/watershed.hpp"
#include "backend/commands/object_functions/colocalization/colocalization.hpp"
#include "backend/commands/object_functions/intersection/intersection.hpp"
#include "backend/commands/object_functions/intersection/intersection_settings.hpp"
#include "backend/commands/object_functions/measure/measure.hpp"
#include "backend/commands/object_functions/validator_noise/validator_noise.hpp"
#include "backend/commands/object_functions/validator_threshold/validator_threshold.hpp"
#include "backend/commands/object_functions/voronoi_grid/voronoi_grid.hpp"

namespace joda::settings {

void PipelineStep::operator()(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) const
{
  if($blur) {
    joda::cmd::Factory<joda::cmd::Blur, BlurSettings> a($blur.value());
    a.execute(context, image, result);
  }

  if($saveImage) {
    joda::cmd::Factory<joda::cmd::ImageSaver, ImageSaverSettings> a($saveImage.value());
    a.execute(context, image, result);
  }

  if($threshold) {
    joda::cmd::Factory<joda::cmd::Threshold, ThresholdSettings> a($threshold.value());
    a.execute(context, image, result);
  }

  if($watershed) {
    joda::cmd::Factory<joda::cmd::Watershed, WatershedSettings> a($watershed.value());
    a.execute(context, image, result);
  }

  if($imageFromClass) {
    joda::cmd::Factory<joda::cmd::ImageFromClass, ImageFromClassSettings> a($imageFromClass.value());
    a.execute(context, image, result);
  }

  if($classify) {
    joda::cmd::Factory<joda::cmd::Classifier, ClassifierSettings> a($classify.value());
    a.execute(context, image, result);
  }

  if($aiClassify) {
    joda::cmd::Factory<joda::cmd::AiClassifier, AiClassifierSettings> a($aiClassify.value());
    a.execute(context, image, result);
  }

  if($colocalization) {
    joda::cmd::Factory<joda::cmd::Colocalization, ColocalizationSettings> a($colocalization.value());
    a.execute(context, image, result);
  }

  if($measure) {
    joda::cmd::Factory<joda::cmd::Measure, MeasureSettings> a($measure.value());
    a.execute(context, image, result);
  }

  if($intersection) {
    joda::cmd::Factory<joda::cmd::Intersection, IntersectionSettings> a($intersection.value());
    a.execute(context, image, result);
  }

  if($rollingBall) {
    joda::cmd::Factory<joda::cmd::RollingBallBackground, RollingBallSettings> a($rollingBall.value());
    a.execute(context, image, result);
  }

  if($medianSubtract) {
    joda::cmd::Factory<joda::cmd::MedianSubtraction, MedianSubtractSettings> a($medianSubtract.value());
    a.execute(context, image, result);
  }

  if($edgeDetection) {
    joda::cmd::Factory<joda::cmd::EdgeDetection, EdgeDetectionSettings> a($edgeDetection.value());
    a.execute(context, image, result);
  }

  if($crop) {
    joda::cmd::Factory<joda::cmd::MarginCrop, MarginCropSettings> a($crop.value());
    a.execute(context, image, result);
  }

  if($voronoi) {
    joda::cmd::Factory<joda::cmd::VoronoiGrid, VoronoiGridSettings> a($voronoi.value());
    a.execute(context, image, result);
  }

  if($thresholdValidator) {
    joda::cmd::Factory<joda::cmd::ThresholdValidator, ThresholdValidatorSettings> a($thresholdValidator.value());
    a.execute(context, image, result);
  }

  if($noiseValidator) {
    joda::cmd::Factory<joda::cmd::NoiseValidator, NoiseValidatorSettings> a($noiseValidator.value());
    a.execute(context, image, result);
  }
}

}    // namespace joda::settings
