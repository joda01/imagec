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
auto DependencyGraph::calcGraph(const joda::settings::AnalyzeSettings &settings) -> std::pair<PipelineOrder_t, Graph_t>
{
  // List all pipelines to process
  std::set<const settings::Pipeline *> pipelinesToAttache;
  for(const auto &pipelines : settings.pipelines) {
    pipelinesToAttache.emplace(reinterpret_cast<const settings::Pipeline *>(&pipelines));
  }

  auto cleanup = [&pipelinesToAttache](std::set<const settings::Pipeline *> &attached) {
    for(const auto *toRemove : attached) {
      pipelinesToAttache.erase(toRemove);
    }
  };

  // Find root nodes
  Graph_t rootNodes;
  {
    std::set<const settings::Pipeline *> attached;
    for(const auto *pipeline : pipelinesToAttache) {
      // If empty or input cluster is me
      std::set<settings::ClassificatorSettingOut> clusters;
      for(const auto &element : pipeline->getInputClusters()) {
        clusters.emplace(element);
      }
      if(clusters.empty() ||
         (clusters.size() == 1 &&
          *clusters.begin() == settings::ClassificatorSettingOut{pipeline->pipelineSetup.defaultClusterId, pipeline->pipelineSetup.defaultClassId})) {
        // This is a root node
        rootNodes.emplace_back(pipeline);
        attached.emplace(pipeline);
      }
    }
    cleanup(attached);
    // There is no root, there must be a cycle
    if(rootNodes.empty()) {
      throw std::invalid_argument("No root node, cycle detected!");
    }
  }

  // Build up the dependency graph inclusive cycle detection
  {
    size_t pipelineCount = pipelinesToAttache.size();
    std::map<const settings::Pipeline *, int> notAttachedTrys;
    while(!pipelinesToAttache.empty()) {
      std::set<const settings::Pipeline *> attached;
      bool isAttached = false;
      for(int x = 0; x < pipelinesToAttache.size() + 1; x++) {
        for(const auto *pipeline : pipelinesToAttache) {
          for(int n = 0; n < rootNodes.size(); n++) {
            auto &rootNode = rootNodes[n];
            if(rootNode.attacheNode({pipeline})) {
              attached.emplace(pipeline);
              isAttached = true;
            }
          }
          if(pipelinesToAttache.size() == x) {
            if(!isAttached) {
              notAttachedTrys[pipeline]++;
              if(notAttachedTrys[pipeline] > (pipelineCount * pipelineCount) * rootNodes.size()) {
                throw std::invalid_argument("Cycle detected for pipeline >" + pipeline->meta.name + "<!");
              }
            } else {
              notAttachedTrys.erase(pipeline);
            }
          }
        }
      }
      cleanup(attached);
    }
  }
  // Order pipelines
  PipelineOrder_t finishedOrder;
  {
    std::map<const settings::Pipeline *, int> pipelineOrder;
    for(auto &rootNode : rootNodes) {
      rootNode.orderPipeline(pipelineOrder);
    }
    for(const auto &[pipeline, order] : pipelineOrder) {
      finishedOrder[order].emplace(pipeline);
    }
  }
  // Print result
  std::cout << "----------------" << std::endl;
  {
    for(auto &rootNode : rootNodes) {
      rootNode.printTree();
    }

    printOrder(finishedOrder);
  }
  std::cout << "----------------" << std::endl;

  return {finishedOrder, rootNodes};
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
