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
#include "backend/commands/factory.hpp"
#include "backend/commands/functions/blur/blur.hpp"
#include "backend/commands/functions/image_saver/image_saver.hpp"
#include "backend/commands/functions/image_saver/image_saver_settings.hpp"

namespace joda::settings {

void PipelineStep::operator()(processor::ProcessContext &context, cv::Mat &image, cmd::ObjectsListMap &result) const
{
  if($blur) {
    joda::cmd::Factory<joda::cmd::functions::Blur, cmd::functions::BlurSettings> a($blur.value());
    a.execute(context, image, result);
  }

  if($saveImage) {
    joda::cmd::Factory<joda::cmd::functions::ImageSaver, cmd::functions::ImageSaverSettings> a($saveImage.value());
    a.execute(context, image, result);
  }
}

void PipelineStep::check() const
{
}

}    // namespace joda::settings