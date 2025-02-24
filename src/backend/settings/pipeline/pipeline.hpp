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

#include <chrono>
#include <tuple>
#include "backend/enums/enums_classes.hpp"
#include "backend/processor/initializer/pipeline_settings.hpp"
#include "backend/settings/pipeline/pipeline_meta.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "backend/settings/settings_types.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::settings {

class PipelineHistoryEntry
{
public:
  //
  // The snap shotted pipeline steps
  //
  std::list<PipelineStep> pipelineSteps;

  //
  // Short comment to this snap shpt
  //
  std::string commitMessage;

  //
  // A tagged snap shot, else empty
  //
  std::string tagMessage;

  //
  // It ends at Sunday, February 7, 2106 6:28:16 AM
  // If someone in the future reads this line of code:
  // Sorry for that but the actual used JSON lib does not
  // support serializing 64 bit integers.
  //
  uint32_t timeStamp;

  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(PipelineHistoryEntry, pipelineSteps, commitMessage, tagMessage, timeStamp);
};

class Pipeline
{
public:
  int32_t index = 0;    // Pipeline index, this is temporary and must not be saved. The index is given during the dependency graph generation
  /////////////////////////////////////////////////////
  PipelineMeta meta;
  PipelineSettings pipelineSetup;
  std::list<PipelineStep> pipelineSteps;

  //
  // Changes of the pipeline steps over time
  //
  std::vector<PipelineHistoryEntry> history;

  /////////////////////////////////////////////////////
  void check() const;

  ObjectInputClassesExp getInputClasses() const;
  ObjectOutputClassesExp getOutputClasses() const;
  std::set<enums::MemoryIdx> getInputImageCache() const;
  std::set<enums::MemoryIdx> getOutputImageCache() const;
  enums::ClassId getOutputClass() const;

  auto createSnapShot(const std::string &note) -> std::optional<PipelineHistoryEntry>;
  auto restoreSnapShot(int32_t idex) const -> Pipeline;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Pipeline, meta, pipelineSetup, pipelineSteps, history);
};

}    // namespace joda::settings
