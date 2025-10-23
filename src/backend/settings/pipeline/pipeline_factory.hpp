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
#include "backend/commands/image_functions/image_cache/image_cache.hpp"
#include "backend/commands/image_functions/image_cache/image_cache_settings_ui.hpp"
#include "backend/commands/image_functions/image_from_class/image_from_class.hpp"
#include "backend/commands/image_functions/image_math/image_math.hpp"
#include "backend/commands/image_functions/image_math/image_math_settings_ui.hpp"
#include "backend/commands/image_functions/image_saver/image_saver.hpp"
#include "backend/commands/image_functions/image_saver/image_saver_settings_ui.hpp"
#include "backend/commands/image_functions/intensity/intensity.hpp"
#include "backend/commands/image_functions/intensity/intensity_settings_ui.hpp"
#include "backend/commands/image_functions/margin_crop/margin_crop.hpp"
#include "backend/commands/image_functions/margin_crop/margin_crop_settings.hpp"
#include "backend/commands/image_functions/median_substraction/median_substraction.hpp"
#include "backend/commands/image_functions/median_substraction/median_substraction_settings_ui.hpp"
#include "backend/commands/image_functions/morphological_transformation/morphological_transformation.hpp"
#include "backend/commands/image_functions/morphological_transformation/morphological_transformation_settings_ui.hpp"
#include "backend/commands/image_functions/rank_filter/rank_filter.hpp"
#include "backend/commands/image_functions/rank_filter/rank_filter_settings.hpp"
#include "backend/commands/image_functions/rank_filter/rank_filter_ui.hpp"
#include "backend/commands/image_functions/rolling_ball/rolling_ball.hpp"
#include "backend/commands/image_functions/rolling_ball/rolling_ball_settings.hpp"
#include "backend/commands/image_functions/rolling_ball/rolling_ball_settings_ui.hpp"
#include "backend/commands/image_functions/skeletonize/skeletonize.hpp"
#include "backend/commands/image_functions/skeletonize/skeletonize_settings_ui.hpp"
#include "backend/commands/image_functions/threshold/threshold.hpp"
#include "backend/commands/image_functions/threshold/threshold_settings_ui.hpp"
#include "backend/commands/image_functions/threshold_adaptive/threshold_adaptive.hpp"
#include "backend/commands/image_functions/threshold_adaptive/threshold_adaptive_settings_ui.hpp"
#include "backend/commands/image_functions/watershed/watershed.hpp"
#include "backend/commands/image_functions/watershed/watershed_settings_ui.hpp"
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
    requires std::is_base_of<joda::ui::gui::Command, RET>::value
  {
    return generateIntern<RET>(analyzeSettings, step, parent);
  }

  static std::unique_ptr<joda::cmd::CommandFactory> generate(const settings::PipelineStep &step, QWidget *parent = nullptr)
    requires std::is_base_of<joda::cmd::Command, RET>::value
  {
    return generateIntern<joda::cmd::CommandFactory>(nullptr, step, parent);
  }

private:
  template <class FUNRET>
  static std::unique_ptr<FUNRET> generateIntern(joda::settings::AnalyzeSettings *analyzeSettings, const settings::PipelineStep &step,
                                                QWidget *parent = nullptr)

  {
    if(step.$blur) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::Blur, BlurSettings>>(step.$blur.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::Blur, BlurSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<BlurSettings &>(step.$blur.value()), parent));
      }
    }

    if(step.$colorFilter) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ColorFilter, ColorFilterSettings>>(step.$colorFilter.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::ColorFilter, ColorFilterSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<ColorFilterSettings &>(step.$colorFilter.value()), parent));
      }
    }

    if(step.$intensityTransform) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::IntensityTransformation, IntensityTransformationSettings>>(
            step.$intensityTransform.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::IntensityTransformation, IntensityTransformationSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step),
            const_cast<IntensityTransformationSettings &>(step.$intensityTransform.value()), parent));
      }
    }

    if(step.$saveImage) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ImageSaver, ImageSaverSettings>>(step.$saveImage.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::make_unique<joda::ui::gui::Factory<joda::ui::gui::ImageSaver, ImageSaverSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<ImageSaverSettings &>(step.$saveImage.value()), parent);
      }
    }

    if(step.$threshold) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::Threshold, ThresholdSettings>>(step.$threshold.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::make_unique<joda::ui::gui::Factory<joda::ui::gui::Threshold, ThresholdSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<ThresholdSettings &>(step.$threshold.value()), parent);
      }
    }

    if(step.$thresholdAdaptive) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ThresholdAdaptive, ThresholdAdaptiveSettings>>(step.$thresholdAdaptive.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::make_unique<joda::ui::gui::Factory<joda::ui::gui::ThresholdAdaptive, ThresholdAdaptiveSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<ThresholdAdaptiveSettings &>(step.$thresholdAdaptive.value()),
            parent);
      }
    }

    if(step.$pixelClassify) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::PixelClassifier, PixelClassifierSettings>>(step.$pixelClassify.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::make_unique<joda::ui::gui::Factory<joda::ui::gui::PixelClassifier, PixelClassifierSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<PixelClassifierSettings &>(step.$pixelClassify.value()), parent);
      }
    }

    if(step.$watershed) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::Watershed, WatershedSettings>>(step.$watershed.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::make_unique<joda::ui::gui::Factory<joda::ui::gui::Watershed, WatershedSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<WatershedSettings &>(step.$watershed.value()), parent);
      }
    }

    if(step.$imageFromClass) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ImageFromClass, ImageFromClassSettings>>(step.$imageFromClass.value());
      }
    } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
    }

    if(step.$classify) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::Classifier, ClassifierSettings>>(step.$classify.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::make_unique<joda::ui::gui::Factory<joda::ui::gui::Classifier, ClassifierSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<ClassifierSettings &>(step.$classify.value()), parent);
      }
    }

    if(step.$aiClassify) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::AiClassifier, AiClassifierSettings>>(step.$aiClassify.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::make_unique<joda::ui::gui::Factory<joda::ui::gui::AiClassifier, AiClassifierSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<AiClassifierSettings &>(step.$aiClassify.value()), parent);
      }
    }

    if(step.$colocalization) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::Colocalization, ColocalizationSettings>>(step.$colocalization.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::Colocalization, ColocalizationSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<ColocalizationSettings &>(step.$colocalization.value()), parent));
      }
    }

    if(step.$measureIntensity) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::MeasureIntensity, MeasureIntensitySettings>>(step.$measureIntensity.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::make_unique<joda::ui::gui::Factory<joda::ui::gui::MeasureIntensity, MeasureIntensitySettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<MeasureIntensitySettings &>(step.$measureIntensity.value()),
            parent);
      }
    }

    if(step.$measureDistance) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::MeasureDistance, MeasureDistanceSettings>>(step.$measureDistance.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::make_unique<joda::ui::gui::Factory<joda::ui::gui::MeasureDistance, MeasureDistanceSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<MeasureDistanceSettings &>(step.$measureDistance.value()),
            parent);
      }
    }

    if(step.$reclassify) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::Reclassify, ReclassifySettings>>(step.$reclassify.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::Reclassify, ReclassifySettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<ReclassifySettings &>(step.$reclassify.value()), parent));
      }
    }

    if(step.$rollingBall) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::RollingBallBackground, RollingBallSettings>>(step.$rollingBall.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::make_unique<joda::ui::gui::Factory<joda::ui::gui::RollingBallBackground, RollingBallSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<RollingBallSettings &>(step.$rollingBall.value()), parent);
      }
    }

    if(step.$medianSubtract) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::MedianSubtraction, MedianSubtractSettings>>(step.$medianSubtract.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::make_unique<joda::ui::gui::Factory<joda::ui::gui::MedianSubtraction, MedianSubtractSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<MedianSubtractSettings &>(step.$medianSubtract.value()), parent);
      }
    }

    if(step.$sobel) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::EdgeDetectionSobel, EdgeDetectionSobelSettings>>(step.$sobel.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::EdgeDetectionSobel, EdgeDetectionSobelSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<EdgeDetectionSobelSettings &>(step.$sobel.value()), parent));
      }
    }

    if(step.$canny) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::EdgeDetectionCanny, EdgeDetectionCannySettings>>(step.$canny.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::EdgeDetectionCanny, EdgeDetectionCannySettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<EdgeDetectionCannySettings &>(step.$canny.value()), parent));
      }
    }

    if(step.$crop) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::MarginCrop, MarginCropSettings>>(step.$crop.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
      }
    }

    if(step.$voronoi) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::VoronoiGrid, VoronoiGridSettings>>(step.$voronoi.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::VoronoiGrid, VoronoiGridSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<VoronoiGridSettings &>(step.$voronoi.value()), parent));
      }
    }

    if(step.$thresholdValidator) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ThresholdValidator, ThresholdValidatorSettings>>(step.$thresholdValidator.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::ThresholdValidator, ThresholdValidatorSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<ThresholdValidatorSettings &>(step.$thresholdValidator.value()),
            parent));
      }
    }

    if(step.$noiseValidator) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::NoiseValidator, NoiseValidatorSettings>>(step.$noiseValidator.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::NoiseValidator, NoiseValidatorSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<NoiseValidatorSettings &>(step.$noiseValidator.value()), parent));
      }
    }

    if(step.$objectsToImage) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ObjectsToImage, ObjectsToImageSettings>>(step.$objectsToImage.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::ObjectsToImage, ObjectsToImageSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<ObjectsToImageSettings &>(step.$objectsToImage.value()), parent));
      }
    }

    if(step.$objectTransform) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ObjectTransform, ObjectTransformSettings>>(step.$objectTransform.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::ObjectTransform, ObjectTransformSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<ObjectTransformSettings &>(step.$objectTransform.value()),
            parent));
      }
    }

    if(step.$imageMath) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ImageMath, ImageMathSettings>>(step.$imageMath.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::ImageMath, ImageMathSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<ImageMathSettings &>(step.$imageMath.value()), parent));
      }
    }

    if(step.$imageToCache) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ImageCache, ImageCacheSettings>>(step.$imageToCache.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::ImageCache, ImageCacheSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<ImageCacheSettings &>(step.$imageToCache.value()), parent));
      }
    }

    if(step.$morphologicalTransform) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::MorphologicalTransform, MorphologicalTransformSettings>>(
            step.$morphologicalTransform.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::MorphologicalTransform, MorphologicalTransformSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step),
            const_cast<MorphologicalTransformSettings &>(step.$morphologicalTransform.value()), parent));
      }
    }

    if(step.$fillHoles) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::FillHoles, FillHolesSettings>>(step.$fillHoles.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::FillHoles, FillHolesSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<FillHolesSettings &>(step.$fillHoles.value()), parent));
      }
    }

    if(step.$houghTransform) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::HoughTransform, HoughTransformSettings>>(step.$houghTransform.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::HoughTransform, HoughTransformSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<HoughTransformSettings &>(step.$houghTransform.value()), parent));
      }
    }

    if(step.$enhanceContrast) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::EnhanceContrast, EnhanceContrastSettings>>(step.$enhanceContrast.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::EnhanceContrast, EnhanceContrastSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<EnhanceContrastSettings &>(step.$enhanceContrast.value()),
            parent));
      }
    }

    if(step.$rank) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::RankFilter, RankFilterSettings>>(step.$rank.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::RankFilter, RankFilterSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<RankFilterSettings &>(step.$rank.value()), parent));
      }
    }

    if(step.$skeletonize) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::Skeletonize, SkeletonizeSettings>>(step.$skeletonize.value());
      } else if constexpr(std::is_base_of<joda::ui::gui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::gui::Factory<joda::ui::gui::Skeletonize, SkeletonizeSettings>>(
            analyzeSettings, const_cast<settings::PipelineStep &>(step), const_cast<SkeletonizeSettings &>(step.$skeletonize.value()), parent));
      }
    }

    /// \todo handle not supported commands
    return nullptr;
  }
};

}    // namespace joda::settings
