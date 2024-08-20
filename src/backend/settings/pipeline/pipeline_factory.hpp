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
#include "backend/commands/classification/classifier/classifier.hpp"
#include "backend/commands/command.hpp"
#include "backend/commands/factory.hpp"
#include "backend/commands/image_functions/blur/blur.hpp"
#include "backend/commands/image_functions/blur/blur_settings_ui.hpp"
#include "backend/commands/image_functions/edge_detection/edge_detection.hpp"
#include "backend/commands/image_functions/image_from_class/image_from_class.hpp"
#include "backend/commands/image_functions/image_saver/image_saver.hpp"
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
#include "backend/commands/object_functions/colocalization/colocalization.hpp"
#include "backend/commands/object_functions/intersection/intersection.hpp"
#include "backend/commands/object_functions/intersection/intersection_settings.hpp"
#include "backend/commands/object_functions/measure/measure.hpp"
#include "backend/commands/object_functions/validator_noise/validator_noise.hpp"
#include "backend/commands/object_functions/validator_threshold/validator_threshold.hpp"
#include "backend/commands/object_functions/voronoi_grid/voronoi_grid.hpp"
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
  static std::shared_ptr<RET> generate(const settings::PipelineStep &step, QWidget *parent = nullptr)
  {
    if(step.$blur) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::Blur, BlurSettings>>(step.$blur.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::make_shared<joda::ui::Factory<joda::ui::Blur, BlurSettings>>(
            const_cast<BlurSettings &>(step.$blur.value()), parent);
      }
    }

    if(step.$saveImage) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::ImageSaver, ImageSaverSettings>>(step.$saveImage.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
      }
    }

    if(step.$threshold) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::Threshold, ThresholdSettings>>(step.$threshold.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::make_shared<joda::ui::Factory<joda::ui::Threshold, ThresholdSettings>>(
            const_cast<ThresholdSettings &>(step.$threshold.value()), parent);
      }
    }

    if(step.$watershed) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::Watershed, WatershedSettings>>(step.$watershed.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
      }
    }

    if(step.$imageFromClass) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::ImageFromClass, ImageFromClassSettings>>(
            step.$imageFromClass.value());
      }
    } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
    }

    if(step.$classify) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::Classifier, ClassifierSettings>>(step.$classify.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
      }
    }

    if(step.$aiClassify) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::AiClassifier, AiClassifierSettings>>(
            step.$aiClassify.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
      }
    }

    if(step.$colocalization) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::Colocalization, ColocalizationSettings>>(
            step.$colocalization.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
      }
    }

    if(step.$measure) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::Measure, MeasureSettings>>(step.$measure.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
      }
    }

    if(step.$intersection) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::Intersection, IntersectionSettings>>(
            step.$intersection.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
      }
    }

    if(step.$rollingBall) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::RollingBallBackground, RollingBallSettings>>(
            step.$rollingBall.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::make_shared<joda::ui::Factory<joda::ui::RollingBallBackground, RollingBallSettings>>(
            const_cast<RollingBallSettings &>(step.$rollingBall.value()), parent);
      }
    }

    if(step.$medianSubtract) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::MedianSubtraction, MedianSubtractSettings>>(
            step.$medianSubtract.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
        return std::make_shared<joda::ui::Factory<joda::ui::MedianSubtraction, MedianSubtractSettings>>(
            const_cast<MedianSubtractSettings &>(step.$medianSubtract.value()), parent);
      }
    }

    if(step.$edgeDetection) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::EdgeDetection, EdgeDetectionSettings>>(
            step.$edgeDetection.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
      }
    }

    if(step.$crop) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::MarginCrop, MarginCropSettings>>(step.$crop.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
      }
    }

    if(step.$voronoi) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::VoronoiGrid, VoronoiGridSettings>>(step.$voronoi.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
      }
    }

    if(step.$thresholdValidator) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::ThresholdValidator, ThresholdValidatorSettings>>(
            step.$thresholdValidator.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
      }
    }

    if(step.$noiseValidator) {
      if constexpr(std::is_base_of<joda::cmd::Command, RET>::value) {
        return std::make_shared<joda::cmd::Factory<joda::cmd::NoiseValidator, NoiseValidatorSettings>>(
            step.$noiseValidator.value());
      } else if constexpr(std::is_base_of<joda::ui::Command, RET>::value) {
      }
    }

    return nullptr;
  }
};

}    // namespace joda::settings
