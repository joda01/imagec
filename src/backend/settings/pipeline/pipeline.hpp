///
/// \file      pipeline.hpp
/// \author    Joachim Danmayr
/// \date      2023-04-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include "backend/processor/initializer/pipeline_settings.hpp"
#include "backend/settings/pipeline/pipeline_meta.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"

namespace joda::settings {

class Pipeline : public Setting
{
public:
  /////////////////////////////////////////////////////

  PipelineMeta meta;
  PipelineSettings pipelineSetup;
  std::vector<PipelineStep> pipelineSteps;

  /////////////////////////////////////////////////////
  void check() const override
  {
    for(const auto &element : pipelineSteps) {
      element.check();
    }
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Pipeline, meta, pipelineSetup, pipelineSteps);
};

}    // namespace joda::settings
