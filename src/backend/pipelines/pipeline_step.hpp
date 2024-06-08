///
/// \file      pipeline_step.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-31
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A pipeline step
///

#pragma once

#include <map>
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/settings/analze_settings.hpp"

namespace joda::pipeline {

class PipelineStep
{
public:
  virtual auto execute(const joda::settings::AnalyzeSettings &,
                       const std::map<joda::settings::ChannelIndex, image::detect::DetectionResponse> &) const
      -> image::detect::DetectionResponse = 0;
};

}    // namespace joda::pipeline
