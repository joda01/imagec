///
/// \file      pipeline_step.hpp
/// \author    Joachim Danmayr
/// \date      2023-04-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <memory>
#include <optional>
#include "backend/commands/classification/ai_classifier/ai_classifier_settings.hpp"
#include "backend/commands/classification/classifier/classifier_settings.hpp"
#include "backend/commands/classification/hough_transform/hough_transform_settings.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier_settings.hpp"
#include "backend/commands/classification/reclassify/reclassify_settings.hpp"
#include "backend/commands/image_functions/blur/blur_settings.hpp"
#include "backend/commands/image_functions/color_filter/color_filter_settings.hpp"
#include "backend/commands/image_functions/edge_detection_canny/edge_detection_canny_settings.hpp"
#include "backend/commands/image_functions/edge_detection_sobel/edge_detection_sobel_settings.hpp"
#include "backend/commands/image_functions/enhance_contrast/enhance_contrast_settings.hpp"
#include "backend/commands/image_functions/fill_holes/fill_holes_settings.hpp"
#include "backend/commands/image_functions/hessian/hessian_settings.hpp"
#include "backend/commands/image_functions/image_cache/image_cache_settings.hpp"
#include "backend/commands/image_functions/image_math/image_math_settings.hpp"
#include "backend/commands/image_functions/image_saver/image_saver_settings.hpp"
#include "backend/commands/image_functions/intensity/intensity_settings.hpp"
#include "backend/commands/image_functions/laplacian/laplacian_settings.hpp"
#include "backend/commands/image_functions/margin_crop/margin_crop_settings.hpp"
#include "backend/commands/image_functions/median_substraction/median_substraction_settings.hpp"
#include "backend/commands/image_functions/morphological_transformation/morphological_transformation_settings.hpp"
#include "backend/commands/image_functions/nop/nop_settings.hpp"
#include "backend/commands/image_functions/rank_filter/rank_filter_settings.hpp"
#include "backend/commands/image_functions/rolling_ball/rolling_ball_settings.hpp"
#include "backend/commands/image_functions/skeletonize/skeletonize_settings.hpp"
#include "backend/commands/image_functions/structur_tensor/structure_tensor_settings.hpp"
#include "backend/commands/image_functions/threshold/threshold_settings.hpp"
#include "backend/commands/image_functions/threshold_adaptive/threshold_adaptive_settings.hpp"
#include "backend/commands/image_functions/watershed/watershed_settings.hpp"
#include "backend/commands/image_functions/weighted_deviation/weighted_deviation_settings.hpp"
#include "backend/commands/object_functions/colocalization/colocalization_settings.hpp"
#include "backend/commands/object_functions/measure_distance/measure_distance_settings.hpp"
#include "backend/commands/object_functions/measure_intensity/measure_intensity_settings.hpp"
#include "backend/commands/object_functions/object_transform/object_transform_settings.hpp"
#include "backend/commands/object_functions/objects_to_image/objects_to_image_settings.hpp"
#include "backend/commands/object_functions/validator_noise/validator_noise_settings.hpp"
#include "backend/commands/object_functions/validator_threshold/validator_threshold_settings.hpp"
#include "backend/commands/object_functions/voronoi_grid/voronoi_grid_settings.hpp"
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
  // Disabled pipeline steps are not executed
  //
  bool disabled = false;
  //
  // Locked pipeline steps can not be edited
  //
  bool locked = false;

  //
  // Stop at this point
  //
  bool breakPoint = false;

  //
  // Exact one command to execute
  //
  std::optional<BlurSettings> $blur                                     = std::nullopt;
  std::optional<IntensityTransformationSettings> $intensityTransform    = std::nullopt;
  std::optional<WatershedSettings> $watershed                           = std::nullopt;
  std::optional<RollingBallSettings> $rollingBall                       = std::nullopt;
  std::optional<MedianSubtractSettings> $medianSubtract                 = std::nullopt;
  std::optional<EdgeDetectionSobelSettings> $sobel                      = std::nullopt;
  std::optional<EdgeDetectionCannySettings> $canny                      = std::nullopt;
  std::optional<MarginCropSettings> $crop                               = std::nullopt;
  std::optional<ObjectTransformSettings> $objectTransform               = std::nullopt;
  std::optional<MorphologicalTransformSettings> $morphologicalTransform = std::nullopt;
  std::optional<FillHolesSettings> $fillHoles                           = std::nullopt;
  std::optional<HoughTransformSettings> $houghTransform                 = std::nullopt;
  std::optional<EnhanceContrastSettings> $enhanceContrast               = std::nullopt;
  std::optional<RankFilterSettings> $rank                               = std::nullopt;
  std::optional<SkeletonizeSettings> $skeletonize                       = std::nullopt;
  std::optional<LaplacianSettings> $laplacian                           = std::nullopt;
  std::optional<WeightedDeviationSettings> $gaussianWeightedDev         = std::nullopt;
  std::optional<StructureTensorSettings> $structureTensor               = std::nullopt;
  std::optional<HessianSettings> $hessian                               = std::nullopt;
  std::optional<NopSettings> $nop                                       = std::nullopt;

  // Object commands
  std::optional<ImageCacheSettings> $imageToCache               = std::nullopt;
  std::optional<ImageMathSettings> $imageMath                   = std::nullopt;
  std::optional<ThresholdValidatorSettings> $thresholdValidator = std::nullopt;
  std::optional<NoiseValidatorSettings> $noiseValidator         = std::nullopt;
  std::optional<ColorFilterSettings> $colorFilter               = std::nullopt;
  std::optional<ObjectsToImageSettings> $objectsToImage         = std::nullopt;
  std::optional<VoronoiGridSettings> $voronoi                   = std::nullopt;
  std::optional<ClassifierSettings> $classify                   = std::nullopt;
  std::optional<AiClassifierSettings> $aiClassify               = std::nullopt;
  std::optional<PixelClassifierSettings> $pixelClassify         = std::nullopt;
  std::optional<ColocalizationSettings> $colocalization         = std::nullopt;
  std::optional<ReclassifySettings> $reclassify                 = std::nullopt;
  std::optional<MeasureIntensitySettings> $measureIntensity     = std::nullopt;
  std::optional<MeasureDistanceSettings> $measureDistance       = std::nullopt;
  std::optional<ThresholdAdaptiveSettings> $thresholdAdaptive   = std::nullopt;
  std::optional<ThresholdSettings> $threshold                   = std::nullopt;
  std::optional<ImageSaverSettings> $saveImage                  = std::nullopt;

  /////////////////////////////////////////////////////
  void operator()(processor::ProcessContext &context, cv::Mat &image, joda::atom::ObjectList &result) const;
  void operator()(cv::Mat &image) const;

  void check() const;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(PipelineStep, $blur, $saveImage, $threshold, $thresholdAdaptive, $watershed, $classify,
                                                       $aiClassify, $colocalization, $reclassify, $measureIntensity, $measureDistance, $rollingBall,
                                                       $medianSubtract, $sobel, $canny, $crop, $voronoi, $thresholdValidator, $noiseValidator,
                                                       $intensityTransform, $colorFilter, $objectsToImage, $imageMath, $objectTransform,
                                                       $imageToCache, $morphologicalTransform, $fillHoles, $houghTransform, $enhanceContrast, $rank,
                                                       $skeletonize, $pixelClassify, $laplacian, $gaussianWeightedDev, $structureTensor, $hessian,
                                                       $nop, disabled, locked);
};

}    // namespace joda::settings
