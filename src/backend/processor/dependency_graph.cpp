///
/// \file      dependency_graph.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-30
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "dependency_graph.hpp"
#include <concepts>
#include <cstddef>
#include <exception>
#include <stdexcept>
#include <string>

#include "backend/enums/enum_memory_idx.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/setting.hpp"

namespace joda::processor {

class DependencyGraphKey
{
public:
  DependencyGraphKey(joda::enums::ClassId in) : mKey(static_cast<__uint128_t>(in))
  {
  }
  DependencyGraphKey(joda::enums::MemoryIdx in) : mKey(static_cast<__uint128_t>(in) << static_cast<__uint128_t>(16))
  {
  }

  bool operator<(const DependencyGraphKey &in) const
  {
    return mKey < in.mKey;
  }

  operator __uint128_t() const
  {
    return mKey;
  }

  std::string toString() const
  {
    __uint128_t key = mKey;
    if(key == 0)
      return "0";

    std::string result;
    while(key > 0) {
      // Get the last digit using modulo 10
      unsigned int digit = key % 10;
      result             = char('0' + digit) + result;
      key /= 10;
    }

    return result;
  }

private:
  __uint128_t mKey;
};

class DependencyGraphKeySet : public std::set<DependencyGraphKey>
{
public:
  using std::set<DependencyGraphKey>::set;

  DependencyGraphKeySet(const std::set<enums::ClassId> &in1, const std::set<enums::MemoryIdx> &in)
  {
    insert(in1);
    insert(in);
  }

  void insert(const std::set<enums::ClassId> &in)
  {
    for(const auto &id : in) {
      this->emplace(DependencyGraphKey(id));
    }
  }
  void insert(const std::set<enums::MemoryIdx> &in)
  {
    for(const auto &id : in) {
      this->emplace(DependencyGraphKey(id));
    }
  }
};

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool providesAllClassesAndClasses(const std::set<DependencyGraphKey> &inputClasses, const std::set<DependencyGraphKey> &outputClasses)
{
  for(const auto &element : inputClasses) {
    if(!outputClasses.contains(element)) {
      // This node depends on an other
      return false;
    }
  }
  return true;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto whichOneAreProvided(const std::set<DependencyGraphKey> &inputClasses, const std::set<DependencyGraphKey> &outputClasses)
    -> std::set<DependencyGraphKey>
{
  std::set<DependencyGraphKey> provides;
  for(const auto &element : inputClasses) {
    if(outputClasses.contains(element)) {
      provides.emplace(element);
    }
  }
  return provides;
}

bool Node::isRootNode() const
{
  // If there are no dpes or the only dep is me this is a root node
  return pipelinesProvidingMyDeps.empty() || (pipelinesProvidingMyDeps.size() == 1 && *pipelinesProvidingMyDeps.begin() == pipeline);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto DependencyGraph::calcGraph(const joda::settings::AnalyzeSettings &settings, const settings::Pipeline *calcGraphFor,
                                std::vector<SettingParserLog> *settingParserLog) -> PipelineOrder_t
{
  auto writeToLog = [&](SettingParserLog::Severity severity, const std::string &pipelineName, const std::string &what) {
    if(settingParserLog != nullptr) {
      const auto name = std::string(pipelineName);
      auto data       = SettingParserLog(severity, name, what);
      settingParserLog->emplace_back(data);
    }
  };

  // List all pipelines to process
  std::set<const settings::Pipeline *> pipelinesToAttache;
  for(const auto &pipelines : settings.pipelines) {
    pipelinesToAttache.emplace(reinterpret_cast<const settings::Pipeline *>(&pipelines));
  }

  // Find all dependencies
  Graph_t depGraph;
  {
    for(const settings::Pipeline *pipelineOne : pipelinesToAttache) {
      DependencyGraphKeySet inputClasses(pipelineOne->getInputClasses(), pipelineOne->getInputImageCache());

      depGraph.push_back(Node{pipelineOne});

      if(inputClasses.empty()) {
        // This pipeline depends on nothing
        //    std::cout << pipelineOne->meta.name << " depends on nothing" << std::endl;

      } else {
        Node &inserted = depGraph.at(depGraph.size() - 1);
        // Look for pipeline providing the needed input classs/classes
        for(const auto *pipelineTwo : pipelinesToAttache) {
          DependencyGraphKeySet outputClasses(pipelineTwo->getOutputClasses(), pipelineTwo->getOutputImageCache());

          // This pipeline depends on more than one other pipeline
          auto provided = whichOneAreProvided(inputClasses, outputClasses);
          if(!provided.empty()) {
            inserted.addDependency(pipelineTwo);

            for(const auto &element : provided) {
              inputClasses.erase(element);    // Remove deps which are still covered
            }
            //    std::cout << pipelineOne->meta.name << " depends on " << pipelineTwo->meta.name << std::endl;
          }
        }

        if(!inputClasses.empty()) {
          std::string unresolved;
          for(const auto &ele : inputClasses) {
            unresolved += ele.toString() + ",";
          }
          if(!unresolved.empty()) {
            unresolved.pop_back();
          }

          writeToLog(SettingParserLog::Severity::JODA_WARNING, pipelineOne->meta.name,
                     "There is an unresolved dependency in pipeline which needs following classs/memory but not found: [" + unresolved + "]!");
        }
      }

      //  std::cout << "###############" << std::endl;
    }
  }

  // Remove all nodes except the one we want to calc the graph for and this graph depends on
  if(calcGraphFor != nullptr) {
    std::set<const settings::Pipeline *> meDeps;

    std::stack<const settings::Pipeline *> stack;
    stack.push(calcGraphFor);

    // Find the dependencies of the current pipeline and the dependencies of the dependencies
    while(!stack.empty()) {
      const settings::Pipeline *current = stack.top();
      stack.pop();
      for(const auto &me : depGraph) {
        if(me.getPipeline() == current) {
          // Get dependencies of the current pipeline
          auto myDeps = me.getDeps();
          // Push dependencies onto the stack for further processing
          for(const settings::Pipeline *depthFor : myDeps) {
            if(!meDeps.contains(depthFor)) {
              stack.push(depthFor);
            }
          }
          meDeps.insert(myDeps.begin(), myDeps.end());
          break;    // Once the matching pipeline is processed, break out of the loop
        }
      }
    }

    // Remove not needed nodes
    for(int idx = (depGraph.size() - 1); idx >= 0; idx--) {
      const auto *pip = depGraph.at(idx).getPipeline();
      if(!meDeps.contains(pip) && pip != calcGraphFor) {
        depGraph.erase(depGraph.begin() + idx);
      }
    }
  }

  // Now reduce the graph
  int depth = 0;
  PipelineOrder_t finishedOrder;

  //
  // Repeat until the graph is empty
  // If there is a cycle this loop will never stop
  //
  int32_t maxRuns = depGraph.size() * depGraph.size();
  while(!depGraph.empty()) {
    //
    // Find the root nodes: Root nodes are nodes which have no dependency or only depends on itself
    //
    std::set<const settings::Pipeline *> toRemove;
    for(const auto &node : depGraph) {
      if(node.isRootNode()) {
        finishedOrder[depth].emplace(node.getPipeline());
        toRemove.emplace(node.getPipeline());
      }
    }

    //
    // Remove all the root nodes from all pipelines since this dependency has been resolved
    //
    for(auto &node : depGraph) {
      node.removePipeline(toRemove);
    }

    //
    // Remove the nodes from the graph
    //
    for(int idx = (depGraph.size() - 1); idx >= 0; idx--) {
      if(toRemove.contains(depGraph.at(idx).getPipeline())) {
        depGraph.erase(depGraph.begin() + idx);
      }
    }
    depth++;
    if(--maxRuns < 0) {
      // Cycle detected
      std::string pipelines;
      for(const auto &pip : depGraph) {
        pipelines += pip.getPipeline()->meta.name + ",";
      }
      if(!pipelines.empty()) {
        pipelines.pop_back();
      }
      writeToLog(SettingParserLog::Severity::JODA_ERROR, "", "Cycle detected in pipelines [" + pipelines + "]");
      break;
    }
  }

  // std::cout << "------ Finished ------" << std::endl;
  // printOrder(finishedOrder);
  // std::cout << "------ -------- ------" << std::endl;

  return finishedOrder;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DependencyGraph::printOrder(const PipelineOrder_t &finishedOrder)
{
  for(const auto &[order, pipelines] : finishedOrder) {
    std::cout << std::to_string(order) << ") ";
    for(const auto &pipeline : pipelines) {
      std::cout << pipeline->meta.name << ",";
    }
    std::cout << std::endl;
  }
}

}    // namespace joda::processor
