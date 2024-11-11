///
/// \file      pipeline_factory.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <memory>
#include <type_traits>
#include "backend/commands/classification/ai_classifier/ai_classifier.hpp"
#include "backend/commands/classification/ai_classifier/ai_classifier_settings_ui.hpp"
#include "backend/commands/classification/classifier/classifier.hpp"
#include "backend/commands/classification/classifier/classifier_settings_ui.hpp"
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
#include "backend/commands/image_functions/edge_detection/edge_detection.hpp"
#include "backend/commands/image_functions/edge_detection/edge_detection_settings_ui.hpp"
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
#include "backend/commands/image_functions/rolling_ball/rolling_ball.hpp"
#include "backend/commands/image_functions/rolling_ball/rolling_ball_settings.hpp"
#include "backend/commands/image_functions/rolling_ball/rolling_ball_settings_ui.hpp"
#include "backend/commands/image_functions/threshold/threshold.hpp"
#include "backend/commands/image_functions/threshold/threshold_settings_ui.hpp"
#include "backend/commands/image_functions/watershed/watershed.hpp"
#include "backend/commands/image_functions/watershed/watershed_settings_ui.hpp"
#include "backend/commands/object_functions/colocalization/colocalization.hpp"
#include "backend/commands/object_functions/colocalization/colocalization_settings_ui.hpp"
#include "backend/commands/object_functions/measure/measure.hpp"
#include "backend/commands/object_functions/measure/measure_settings_ui.hpp"
#include "backend/commands/object_functions/object_math/object_math.hpp"
#include "backend/commands/object_functions/object_math/object_math_settings_ui.hpp"
#include "backend/commands/object_functions/object_transform/object_transform.hpp"
#include "backend/commands/object_functions/object_transform/object_transform_settings_ui.hpp"
#include "backend/commands/object_functions/validator_noise/validator_noise.hpp"
#include "backend/commands/object_functions/validator_noise/validator_noise_settings_ui.hpp"
#include "backend/commands/object_functions/validator_threshold/validator_threshold.hpp"
#include "backend/commands/object_functions/validator_threshold/validator_threshold_settings_ui.hpp"
#include "backend/commands/object_functions/voronoi_grid/voronoi_grid.hpp"
#include "backend/commands/object_functions/voronoi_grid/voronoi_grid_settings_ui.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/command/factory.hpp"
#include "pipeline_step.hpp"

namespace joda::settings {

template <class T>
concept command_t = std::is_base_of<joda::cmd::Command, T>::value || std::is_base_of<joda::ui::Command, T>::value;

///
/// \class
/// \author
/// \brief
///
template <command_t RET>
class PipelineFactory
{
public:
  static std::unique_ptr<RET> generate(const settings::PipelineStep &step, QWidget *parent = nullptr)
    requires std::is_base_of<joda::ui::Command, RET>::value
  {
    return generateIntern<RET>(step, parent);
  }

  static std::unique_ptr<joda::cmd::CommandFactory> generate(const settings::PipelineStep &step, QWidget *parent = nullptr)
    requires std::is_base_of<joda::cmd::Command, RET>::value
  {
    return generateIntern<joda::cmd::CommandFactory>(step, parent);
  }

private:
  template <class FUNRET>
  static std::unique_ptr<FUNRET> generateIntern(const settings::PipelineStep &step, QWidget *parent = nullptr)

  {
    if(step.$blur) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::Blur, BlurSettings>>(step.$blur.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::Factory<joda::ui::Blur, BlurSettings>>(const_cast<settings::PipelineStep &>(step),
                                                                                           const_cast<BlurSettings &>(step.$blur.value()), parent));
      }
    }

    if(step.$colorFilter) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ColorFilter, ColorFilterSettings>>(step.$colorFilter.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::Factory<joda::ui::ColorFilter, ColorFilterSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<ColorFilterSettings &>(step.$colorFilter.value()), parent));
      }
    }

    if(step.$intensityTransform) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::IntensityTransformation, IntensityTransformationSettings>>(
            step.$intensityTransform.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::Factory<joda::ui::IntensityTransformation, IntensityTransformationSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<IntensityTransformationSettings &>(step.$intensityTransform.value()), parent));
      }
    }

    if(step.$saveImage) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ImageSaver, ImageSaverSettings>>(step.$saveImage.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::make_unique<joda::ui::Factory<joda::ui::ImageSaver, ImageSaverSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<ImageSaverSettings &>(step.$saveImage.value()), parent);
      }
    }

    if(step.$threshold) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::Threshold, ThresholdSettings>>(step.$threshold.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::make_unique<joda::ui::Factory<joda::ui::Threshold, ThresholdSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<ThresholdSettings &>(step.$threshold.value()), parent);
      }
    }

    if(step.$watershed) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::Watershed, WatershedSettings>>(step.$watershed.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::make_unique<joda::ui::Factory<joda::ui::Watershed, WatershedSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<WatershedSettings &>(step.$watershed.value()), parent);
      }
    }

    if(step.$imageFromClass) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ImageFromClass, ImageFromClassSettings>>(step.$imageFromClass.value());
      }
    } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
    }

    if(step.$classify) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::Classifier, ClassifierSettings>>(step.$classify.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::make_unique<joda::ui::Factory<joda::ui::Classifier, ClassifierSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<ClassifierSettings &>(step.$classify.value()), parent);
      }
    }

    if(step.$aiClassify) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::AiClassifier, AiClassifierSettings>>(step.$aiClassify.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::make_unique<joda::ui::Factory<joda::ui::AiClassifier, AiClassifierSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<AiClassifierSettings &>(step.$aiClassify.value()), parent);
      }
    }

    if(step.$colocalization) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::Colocalization, ColocalizationSettings>>(step.$colocalization.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::Factory<joda::ui::Colocalization, ColocalizationSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<ColocalizationSettings &>(step.$colocalization.value()), parent));
      }
    }

    if(step.$measure) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::Measure, MeasureSettings>>(step.$measure.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::make_unique<joda::ui::Factory<joda::ui::Measure, MeasureSettings>>(const_cast<settings::PipelineStep &>(step),
                                                                                       const_cast<MeasureSettings &>(step.$measure.value()), parent);
      }
    }

    if(step.$reclassify) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::Reclassify, ReclassifySettings>>(step.$reclassify.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::Factory<joda::ui::Reclassify, ReclassifySettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<ReclassifySettings &>(step.$reclassify.value()), parent));
      }
    }

    if(step.$rollingBall) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::RollingBallBackground, RollingBallSettings>>(step.$rollingBall.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::make_unique<joda::ui::Factory<joda::ui::RollingBallBackground, RollingBallSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<RollingBallSettings &>(step.$rollingBall.value()), parent);
      }
    }

    if(step.$medianSubtract) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::MedianSubtraction, MedianSubtractSettings>>(step.$medianSubtract.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::make_unique<joda::ui::Factory<joda::ui::MedianSubtraction, MedianSubtractSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<MedianSubtractSettings &>(step.$medianSubtract.value()), parent);
      }
    }

    if(step.$edgeDetection) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::EdgeDetection, EdgeDetectionSettings>>(step.$edgeDetection.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::Factory<joda::ui::EdgeDetection, EdgeDetectionSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<EdgeDetectionSettings &>(step.$edgeDetection.value()), parent));
      }
    }

    if(step.$crop) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::MarginCrop, MarginCropSettings>>(step.$crop.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
      }
    }

    if(step.$voronoi) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::VoronoiGrid, VoronoiGridSettings>>(step.$voronoi.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::Factory<joda::ui::VoronoiGrid, VoronoiGridSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<VoronoiGridSettings &>(step.$voronoi.value()), parent));
      }
    }

    if(step.$thresholdValidator) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ThresholdValidator, ThresholdValidatorSettings>>(step.$thresholdValidator.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::Factory<joda::ui::ThresholdValidator, ThresholdValidatorSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<ThresholdValidatorSettings &>(step.$thresholdValidator.value()), parent));
      }
    }

    if(step.$noiseValidator) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::NoiseValidator, NoiseValidatorSettings>>(step.$noiseValidator.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::Factory<joda::ui::NoiseValidator, NoiseValidatorSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<NoiseValidatorSettings &>(step.$noiseValidator.value()), parent));
      }
    }

    if(step.$objectMath) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ObjectMath, ObjectMathSettings>>(step.$objectMath.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::Factory<joda::ui::ObjectMath, ObjectMathSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<ObjectMathSettings &>(step.$objectMath.value()), parent));
      }
    }

    if(step.$objectTransform) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ObjectTransform, ObjectTransformSettings>>(step.$objectTransform.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::Factory<joda::ui::ObjectTransform, ObjectTransformSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<ObjectTransformSettings &>(step.$objectTransform.value()), parent));
      }
    }

    if(step.$imageMath) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_unique<joda::cmd::Factory<joda::cmd::ImageMath, ImageMathSettings>>(step.$imageMath.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::move(std::make_unique<joda::ui::Factory<joda::ui::ImageMath, ImageMathSettings>>(
            const_cast<settings::PipelineStep &>(step), const_cast<ImageMathSettings &>(step.$imageMath.value()), parent));
      }
    }

    return nullptr;
  }
};

}    // namespace joda::settings
