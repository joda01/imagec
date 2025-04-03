///
/// \file      dependency_graph.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-30
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <map>
#include <set>
#include <vector>
#include "backend/enums/enum_memory_idx.hpp"
#include "backend/enums/enums_classes.hpp"

class SettingParserLog;

namespace joda::settings {
class AnalyzeSettings;
class Pipeline;
}    // namespace joda::settings

namespace joda::processor {

class Node
{
public:
  /////////////////////////////////////////////////////
  Node(const settings::Pipeline *me) : pipeline(me)
  {
  }
  void addDependency(const settings::Pipeline *dep)
  {
    pipelinesProvidingMyDeps.emplace(dep);
  }
  bool isRootNode() const;
  [[nodiscard]] const settings::Pipeline *getPipeline() const
  {
    return pipeline;
  }
  void setPipelineIndex(int32_t index);

  void removePipeline(const std::set<const settings::Pipeline *> &pip)
  {
    for(const settings::Pipeline *toRemove : pip) {
      pipelinesProvidingMyDeps.erase(toRemove);
    }
  }

  auto getDeps() const -> std::set<const settings::Pipeline *>
  {
    return pipelinesProvidingMyDeps;
  }

private:
  /////////////////////////////////////////////////////
  const settings::Pipeline *pipeline;
  std::set<const settings::Pipeline *> pipelinesProvidingMyDeps;
};

using PipelineOrder_t = std::map<int, std::set<const settings::Pipeline *>>;
using Graph_t         = std::vector<Node>;

///
/// \class      DependencyGraph
/// \author     Joachim Danmayr
/// \brief      Calculates the pipeline dependency graph
///             based on the input and output classes of
///             the pipelines of an analyze setting
///
class DependencyGraph
{
public:
  static auto calcGraph(const joda::settings::AnalyzeSettings &, const settings::Pipeline *calcGRaphFor = nullptr,
                        std::vector<SettingParserLog> *settingParserLog = nullptr) -> PipelineOrder_t;
  static void printOrder(const PipelineOrder_t &order);
};

}    // namespace joda::processor
