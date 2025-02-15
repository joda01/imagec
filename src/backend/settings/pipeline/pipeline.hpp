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

#include "backend/enums/enums_classes.hpp"
#include "backend/processor/initializer/pipeline_settings.hpp"
#include "backend/settings/pipeline/pipeline_meta.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "backend/settings/settings_types.hpp"

namespace joda::settings {

class Pipeline
{
public:
  int32_t index = 0;    // Pipeline index, this is temporary and must not be saved. The index is given during the dependency graph generation
  /////////////////////////////////////////////////////
  PipelineMeta meta;
  PipelineSettings pipelineSetup;
  std::list<PipelineStep> pipelineSteps;

  /////////////////////////////////////////////////////
  void check() const;

  ObjectInputClassesExp getInputClasses() const;
  ObjectOutputClassesExp getOutputClasses() const;
  std::set<enums::MemoryIdx> getInputImageCache() const;
  std::set<enums::MemoryIdx> getOutputImageCache() const;

  enums::ClassId getOutputClass() const;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Pipeline, meta, pipelineSetup, pipelineSteps);
};

}    // namespace joda::settings
