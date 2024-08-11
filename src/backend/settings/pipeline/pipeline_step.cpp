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
/// \brief     A short description what happens here.
///

#include "pipeline_step.hpp"
#include <memory>
#include "backend/commands/classification/ai_classifier/ai_classifier.hpp"
#include "backend/commands/classification/classifier/classifier.hpp"
#include "backend/commands/factory.hpp"
#include "backend/commands/functions/blur/blur.hpp"
#include "backend/commands/functions/image_from_class/image_from_class.hpp"
#include "backend/commands/functions/image_saver/image_saver.hpp"
#include "backend/commands/functions/threshold/threshold.hpp"
#include "backend/commands/functions/watershed/watershed.hpp"

namespace joda::settings {

void PipelineStep::operator()(processor::ProcessContext &context, processor::ProcessorMemory &memory, cv::Mat &image,
                              cmd::ObjectsListMap &result) const
{
  if($blur) {
    joda::cmd::Factory<joda::cmd::functions::Blur, cmd::functions::BlurSettings> a($blur.value());
    a.execute(context, memory, image, result);
  }

  if($saveImage) {
    joda::cmd::Factory<joda::cmd::functions::ImageSaver, cmd::functions::ImageSaverSettings> a($saveImage.value());
    a.execute(context, memory, image, result);
  }

  if($threshold) {
    joda::cmd::Factory<joda::cmd::functions::Threshold, cmd::functions::ThresholdSettings> a($threshold.value());
    a.execute(context, memory, image, result);
  }

  if($watershed) {
    joda::cmd::Factory<joda::cmd::functions::Watershed, cmd::functions::WatershedSettings> a($watershed.value());
    a.execute(context, memory, image, result);
  }

  if($imageFromClass) {
    joda::cmd::Factory<joda::cmd::functions::ImageFromClass, cmd::functions::ImageFromClassSettings> a(
        $imageFromClass.value());
    a.execute(context, memory, image, result);
  }

  if($classify) {
    joda::cmd::Factory<joda::cmd::functions::Classifier, cmd::functions::ClassifierSettings> a($classify.value());
    a.execute(context, memory, image, result);
  }

  if($aiClassify) {
    joda::cmd::Factory<joda::cmd::functions::AiClassifier, cmd::functions::AiClassifierSettings> a($aiClassify.value());
    a.execute(context, memory, image, result);
  }
}

void PipelineStep::check() const
{
}

}    // namespace joda::settings
