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
/// \brief     A short description what happens here.
///

#pragma once

#include "backend/commands/functions/image_loader/channel_loader_settings.hpp"
#include "backend/settings/pipeline/pipeline_meta.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"

namespace joda::settings {

class Pipeline : public joda::cmd::Setting
{
public:
  /////////////////////////////////////////////////////

  PipelineMeta meta;
  cmd::functions::ChannelLoaderSettings channelLoader;
  std::vector<PipelineStep> pipelineSteps;

  /////////////////////////////////////////////////////
  void check() const override
  {
    for(const auto &element : pipelineSteps) {
      element.check();
    }
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Pipeline, meta, channelLoader, pipelineSteps);
};

}    // namespace joda::settings
