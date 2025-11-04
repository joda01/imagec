///
/// \file      pipeline.hpp
/// \author    Joachim Danmayr
/// \date      2023-04-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <chrono>
#include <functional>
#include <tuple>
#include <vector>
#include "backend/enums/enum_history.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/processor/initializer/pipeline_settings.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "backend/settings/settings_meta.hpp"
#include "backend/settings/settings_types.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::settings {

class PipelineHistoryEntry
{
public:
  //
  // Snapshot category
  //
  enums::HistoryCategory category = enums::HistoryCategory::OTHER;

  //
  // The snap shotted pipeline steps
  //
  std::list<PipelineStep> pipelineSteps = {};

  //
  // Short comment to this snap shpt
  //
  std::string commitMessage = {};

  //
  // A tagged snap shot, else empty
  //
  std::string tagMessage = {};

  //
  // It ends at Sunday, February 7, 2106 6:28:16 AM
  // If someone in the future reads this line of code:
  // Sorry for that but the actual used JSON lib does not
  // support serializing 64 bit integers.
  //
  uint32_t timeStamp = {};

  // We don't want to do a error check for the history
  void getErrorLogRecursive(SettingParserLog_t & /*settingsParserLog*/) const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PipelineHistoryEntry, category, pipelineSteps, commitMessage, tagMessage, timeStamp);
};

class Pipeline
{
public:
  int32_t index = 0;    // Pipeline index, this is temporary and must not be saved. The index is given during the dependency graph generation
  /////////////////////////////////////////////////////
  SettingsMeta meta                     = {};
  PipelineSettings pipelineSetup        = {};
  std::list<PipelineStep> pipelineSteps = {};

  //
  // Disabled pipelines are not executed.
  //
  bool disabled = false;

  //
  // Locked pipelines can not be edited
  //
  bool locked = false;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED_CONDITIONAL_CHECK(Pipeline, disabled, meta, pipelineSetup, pipelineSteps, disabled, locked);

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  void check() const;

  ObjectInputClassesExp getInputClasses() const;
  ObjectOutputClassesExp getOutputClasses() const;
  std::set<enums::MemoryIdx> getInputImageCache() const;
  std::set<enums::MemoryIdx> getOutputImageCache() const;
  enums::ClassId getOutputClass() const;

  auto createSnapShot(enums::HistoryCategory category, const std::string &note) -> std::optional<PipelineHistoryEntry>;
  auto restoreSnapShot(size_t idex) const -> Pipeline;
  auto undo() const -> Pipeline;
  void tag(const std::string &tagName, size_t index = 0);
  void clearHistory();
  void eraseHistory();
  auto getHistory() const -> const std::vector<PipelineHistoryEntry> &
  {
    return history;
  }
  void setHistory(const std::vector<PipelineHistoryEntry> &in, size_t idx, const Pipeline &initialPipeline)
  {
    history         = in;
    actHistoryIndex = static_cast<int>(idx);
    if(history.empty()) {
      history.emplace_back(PipelineHistoryEntry{.pipelineSteps = initialPipeline.pipelineSteps, .commitMessage = "Created"});
    }
  }
  size_t getHistoryIndex() const
  {
    return static_cast<size_t>(actHistoryIndex);
  }

  void registerHistoryChangeCallback(const std::function<void()> &func)
  {
    mHistoryChangeCallback.emplace_back(func);
  }

  std::vector<PipelineHistoryEntry> history{{.commitMessage = "Created"}};

private:
  //
  // Changes of the pipeline steps over time
  //
  void triggerHistoryChanged() const;
  int32_t actHistoryIndex = 0;
  std::vector<std::function<void()>> mHistoryChangeCallback;
};

}    // namespace joda::settings
