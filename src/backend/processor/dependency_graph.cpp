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
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/pipeline/pipeline.hpp"

namespace joda::processor {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool providesAllClustersAndClasses(const std::set<settings::ClassificatorSettingOut> &inputClusters,
                                   const std::set<settings::ClassificatorSettingOut> &outputClusters)
{
  for(const auto &element : inputClusters) {
    if(!outputClusters.contains(element)) {
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
auto whichOneAreProvided(const std::set<settings::ClassificatorSettingOut> &inputClusters,
                         const std::set<settings::ClassificatorSettingOut> &outputClusters) -> std::set<settings::ClassificatorSettingOut>
{
  std::set<settings::ClassificatorSettingOut> provides;
  for(const auto &element : inputClusters) {
    if(outputClusters.contains(element)) {
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
auto DependencyGraph::calcGraph(const joda::settings::AnalyzeSettings &settings, const settings::Pipeline *calcGraphFor) -> PipelineOrder_t
{
  // List all pipelines to process
  std::set<const settings::Pipeline *> pipelinesToAttache;
  for(const auto &pipelines : settings.pipelines) {
    pipelinesToAttache.emplace(reinterpret_cast<const settings::Pipeline *>(&pipelines));
  }

  // Find all dependencies
  Graph_t depGraph;
  {
    for(const settings::Pipeline *pipelineOne : pipelinesToAttache) {
      std::set<settings::ClassificatorSettingOut> inputClusters = pipelineOne->getInputClustersAndClasses();

      depGraph.push_back(Node{pipelineOne});

      if(inputClusters.empty()) {
        std::cout << pipelineOne->meta.name << " depends on nothing" << std::endl;
      } else {
        Node &inserted = depGraph.at(depGraph.size() - 1);

        // Look for pipeline providing the needed input cluster/classes
        for(const auto *pipelineTwo : pipelinesToAttache) {
          std::set<settings::ClassificatorSettingOut> outputClusters = pipelineTwo->getOutputClustersAndClasses();

          // This pipeline depends on more than one other pipeline
          auto provided = whichOneAreProvided(inputClusters, outputClusters);
          if(!provided.empty()) {
            inserted.addDependency(pipelineTwo);

            for(const auto &element : provided) {
              inputClusters.erase(element);    // Remove deps which are still covered
            }

            if(inputClusters.empty()) {
              std::cout << pipelineOne->meta.name << " depends only on " << pipelineTwo->meta.name << std::endl;
            } else {
              std::cout << pipelineOne->meta.name << " depends on " << pipelineTwo->meta.name << std::endl;
            }
          }
        }

        if(!inputClusters.empty()) {
          joda::log::logError("There is an unresolved dependency in pipeline " + pipelineOne->meta.name);
        }
      }
    }
  }
  std::cout << "-----" << std::endl;

  // Remove all nodes except the one we want to calc the graph for and this graph depends on
  if(calcGraphFor != nullptr) {
    std::set<const settings::Pipeline *> meDeps;

    for(const auto &me : depGraph) {
      if(me.getPipeline() == calcGraphFor) {
        meDeps = me.getDeps();
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
    if(--maxRuns <= 0) {
      // Cycle detected
      break;
    }
  }

  printOrder(finishedOrder);
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
