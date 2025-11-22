///
/// \file      pipeline_factory.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <memory>
#include <type_traits>
#include "backend/commands/classification/ai_classifier/ai_classifier.hpp"
#include "backend/commands/classification/ai_classifier/ai_classifier_settings_ui.hpp"
#include "backend/commands/classification/classifier/classifier.hpp"
#include "backend/commands/classification/classifier/classifier_settings_ui.hpp"
#include "backend/commands/classification/hough_transform/hough_transform.hpp"
#include "backend/commands/classification/hough_transform/hough_transform_settings.hpp"
#include "backend/commands/classification/hough_transform/hough_transform_settings_ui.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier_settings_ui.hpp"
#include "backend/commands/classification/reclassify/reclassify.hpp"
#include "backend/commands/classification/reclassify/reclassify_settings.hpp"
#include "backend/commands/classification/reclassify/reclassify_settings_ui.hpp"
#include "backend/commands/command.hpp"
#include "backend/commands/factory.hpp"
#include "backend/commands/image_functions/blur/blur.hpp"
#include "backend/commands/image_functions/blur/blur_settings_ui.hpp"
#include "backend/commands/image_functions/color_filter/color_filter.hpp"
#include "backend/commands/image_functions/color_filter/color_filter_settings.hpp"
#include "backend/commands/image_functions/color_filter/color_filter_settings_ui.hpp"
#include "backend/commands/image_functions/edge_detection_canny/edge_detection_canny.hpp"
#include "backend/commands/image_functions/edge_detection_canny/edge_detection_canny_settings_ui.hpp"
#include "backend/commands/image_functions/edge_detection_sobel/edge_detection_sobel.hpp"
#include "backend/commands/image_functions/edge_detection_sobel/edge_detection_sobel_settings_ui.hpp"
#include "backend/commands/image_functions/enhance_contrast/enhance_contrast.hpp"
#include "backend/commands/image_functions/enhance_contrast/enhance_contrast_settings_ui.hpp"
#include "backend/commands/image_functions/fill_holes/fill_holes.hpp"
#include "backend/commands/image_functions/fill_holes/fill_holes_settings_ui.hpp"
#include "backend/commands/image_functions/hessian/hessian.hpp"
#include "backend/commands/image_functions/hessian/hessian_settings_ui.hpp"
#include "backend/commands/image_functions/image_cache/image_cache.hpp"
#include "backend/commands/image_functions/image_cache/image_cache_settings_ui.hpp"
#include "backend/commands/image_functions/image_math/image_math.hpp"
#include "backend/commands/image_functions/image_math/image_math_settings_ui.hpp"
#include "backend/commands/image_functions/image_saver/image_saver.hpp"
#include "backend/commands/image_functions/image_saver/image_saver_settings_ui.hpp"
#include "backend/commands/image_functions/intensity/intensity.hpp"
#include "backend/commands/image_functions/intensity/intensity_settings_ui.hpp"
#include "backend/commands/image_functions/laplacian/laplacian.hpp"
#include "backend/commands/image_functions/laplacian/laplacian_settings_ui.hpp"
#include "backend/commands/image_functions/margin_crop/margin_crop.hpp"
#include "backend/commands/image_functions/margin_crop/margin_crop_settings.hpp"
#include "backend/commands/image_functions/median_substraction/median_substraction.hpp"
#include "backend/commands/image_functions/median_substraction/median_substraction_settings_ui.hpp"
#include "backend/commands/image_functions/morphological_transformation/morphological_transformation.hpp"
#include "backend/commands/image_functions/morphological_transformation/morphological_transformation_settings_ui.hpp"
#include "backend/commands/image_functions/nop/nop.hpp"
#include "backend/commands/image_functions/nop/nop_settings_ui.hpp"
#include "backend/commands/image_functions/rank_filter/rank_filter.hpp"
#include "backend/commands/image_functions/rank_filter/rank_filter_settings.hpp"
#include "backend/commands/image_functions/rank_filter/rank_filter_ui.hpp"
#include "backend/commands/image_functions/rolling_ball/rolling_ball.hpp"
#include "backend/commands/image_functions/rolling_ball/rolling_ball_settings.hpp"
#include "backend/commands/image_functions/rolling_ball/rolling_ball_settings_ui.hpp"
#include "backend/commands/image_functions/skeletonize/skeletonize.hpp"
#include "backend/commands/image_functions/skeletonize/skeletonize_settings_ui.hpp"
#include "backend/commands/image_functions/structur_tensor/structure_tensor.hpp"
#include "backend/commands/image_functions/structur_tensor/structure_tensor_settings_ui.hpp"
#include "backend/commands/image_functions/threshold/threshold.hpp"
#include "backend/commands/image_functions/threshold/threshold_settings_ui.hpp"
#include "backend/commands/image_functions/threshold_adaptive/threshold_adaptive.hpp"
#include "backend/commands/image_functions/threshold_adaptive/threshold_adaptive_settings_ui.hpp"
#include "backend/commands/image_functions/watershed/watershed.hpp"
#include "backend/commands/image_functions/watershed/watershed_settings_ui.hpp"
#include "backend/commands/image_functions/weighted_deviation/weighted_deviation.hpp"
#include "backend/commands/image_functions/weighted_deviation/weighted_deviation_settings.hpp"
#include "backend/commands/image_functions/weighted_deviation/weighted_deviation_settings_ui.hpp"
#include "backend/commands/object_functions/colocalization/colocalization.hpp"
#include "backend/commands/object_functions/colocalization/colocalization_settings_ui.hpp"
#include "backend/commands/object_functions/measure_distance/measure_distance.hpp"
#include "backend/commands/object_functions/measure_distance/measure_distance_settings_ui.hpp"
#include "backend/commands/object_functions/measure_intensity/measure_intensity.hpp"
#include "backend/commands/object_functions/measure_intensity/measure_intensity_settings_ui.hpp"
#include "backend/commands/object_functions/object_transform/object_transform.hpp"
#include "backend/commands/object_functions/object_transform/object_transform_settings_ui.hpp"
#include "backend/commands/object_functions/objects_to_image/objects_to_image.hpp"
#include "backend/commands/object_functions/objects_to_image/objects_to_image_settings_ui.hpp"
#include "backend/commands/object_functions/validator_noise/validator_noise.hpp"
#include "backend/commands/object_functions/validator_noise/validator_noise_settings_ui.hpp"
#include "backend/commands/object_functions/validator_threshold/validator_threshold.hpp"
#include "backend/commands/object_functions/validator_threshold/validator_threshold_settings_ui.hpp"
#include "backend/commands/object_functions/voronoi_grid/voronoi_grid.hpp"
#include "backend/commands/object_functions/voronoi_grid/voronoi_grid_settings_ui.hpp"
#include "ui/gui/editor/widget_pipeline/widget_command/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_command/factory.hpp"
#include "pipeline_step.hpp"

namespace joda::settings {

template <class T>
concept command_t = std::is_base_of<joda::cmd::Command, T>::value || std::is_base_of<joda::ui::gui::Command, T>::value;

#define REGISTER_COMMAND(COMMAND, NAME)                                                                                                           \
  if constexpr((std::is_same<cmd::ImageProcessingCommand, RET>::value && std::is_base_of<cmd::ImageProcessingCommand, joda::cmd::NAME>::value) || \
               std::is_same<cmd::Command, RET>::value || std::is_base_of<joda::ui::gui::Command, RET>::value) {                                   \
    if(step.$##COMMAND) {                                                                                                                         \
      if constexpr(std::is_same<joda::cmd::ImageProcessingCommand, RET>::value) {                                                                 \
        return std::make_unique<joda::cmd::FactoryImg<joda::cmd::NAME, NAME##Settings>>(step.$##COMMAND.value());                                 \
      } else if constexpr(std::is_same<joda::cmd::Command, RET>::value) {                                                                         \
        return std::make_unique<joda::cmd::Factory<joda::cmd::NAME, NAME##Settings>>(step.$##COMMAND.value());                                    \
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {                                                                  \
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::NAME, NAME##Settings>>(                                           \
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<NAME##Settings &>(step.$##COMMAND.value()), parent));         \
      }                                                                                                                                           \
    }                                                                                                                                             \
  }

///
/// \class
/// \author
/// \brief
///
template <command_t RET>
class PipelineFactory
{
public:
  static std::unique_ptr<RET> generate(joda::settings::AnalyzeSettings *analyzeSettings, const settings::PipelineStep &step,
                                       QWidget *parent = nullptr)
    requires std::is_same<joda::ui::gui::Command, RET>::value
  {
    return generateIntern<RET>(analyzeSettings, step, parent);
  }

  static std::unique_ptr<joda::cmd::CommandFactory> generate(const settings::PipelineStep &step, QWidget *parent = nullptr)
    requires std::is_same<joda::cmd::Command, RET>::value
  {
    return generateIntern<joda::cmd::CommandFactory>(nullptr, step, parent);
  }

  static std::unique_ptr<joda::cmd::ImageCommandFactory> generateImageCommand(const settings::PipelineStep &step, QWidget *parent = nullptr)
    requires std::is_same<joda::cmd::ImageProcessingCommand, RET>::value
  {
    return generateIntern<joda::cmd::ImageCommandFactory>(nullptr, step, parent);
  }

private:
  template <class FUNRET>
  static std::unique_ptr<FUNRET> generateIntern(joda::settings::AnalyzeSettings *analyzeSettings, const settings::PipelineStep &step,
                                                QWidget *parent = nullptr)

  {
    REGISTER_COMMAND(colorFilter, ColorFilter);
    REGISTER_COMMAND(saveImage, ImageSaver);
    REGISTER_COMMAND(threshold, Threshold);
    REGISTER_COMMAND(thresholdAdaptive, ThresholdAdaptive);
    REGISTER_COMMAND(pixelClassify, PixelClassifier);
    REGISTER_COMMAND(classify, Classifier);
    REGISTER_COMMAND(aiClassify, AiClassifier);
    REGISTER_COMMAND(colocalization, Colocalization);
    REGISTER_COMMAND(measureIntensity, MeasureIntensity);
    REGISTER_COMMAND(measureDistance, MeasureDistance);
    REGISTER_COMMAND(reclassify, Reclassify);
    REGISTER_COMMAND(voronoi, VoronoiGrid);
    REGISTER_COMMAND(thresholdValidator, ThresholdValidator);
    REGISTER_COMMAND(noiseValidator, NoiseValidator);
    REGISTER_COMMAND(objectsToImage, ObjectsToImage);
    REGISTER_COMMAND(imageMath, ImageMath);
    REGISTER_COMMAND(imageToCache, ImageCache);
    REGISTER_COMMAND(blur, Blur);
    REGISTER_COMMAND(intensityTransform, IntensityTransformation);
    REGISTER_COMMAND(watershed, Watershed);
    REGISTER_COMMAND(rollingBall, RollingBall);
    REGISTER_COMMAND(medianSubtract, MedianSubtract);
    REGISTER_COMMAND(sobel, EdgeDetectionSobel);
    REGISTER_COMMAND(canny, EdgeDetectionCanny);
    REGISTER_COMMAND(objectTransform, ObjectTransform);
    REGISTER_COMMAND(morphologicalTransform, MorphologicalTransform);
    REGISTER_COMMAND(fillHoles, FillHoles);
    REGISTER_COMMAND(houghTransform, HoughTransform);
    REGISTER_COMMAND(enhanceContrast, EnhanceContrast);
    REGISTER_COMMAND(rank, RankFilter);
    REGISTER_COMMAND(skeletonize, Skeletonize);
    REGISTER_COMMAND(laplacian, Laplacian);
    REGISTER_COMMAND(structureTensor, StructureTensor);
    REGISTER_COMMAND(hessian, Hessian);
    REGISTER_COMMAND(gaussianWeightedDev, WeightedDeviation);
    REGISTER_COMMAND(nop, Nop);

    //  REGISTER_COMMAND(crop, MarginCrop);

    /// \todo handle not supported commands
    return nullptr;
  }
};

}    // namespace joda::settings
