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

class Node
{
public:
  Node(const settings::Pipeline *me) : me(me)
  {
  }

  enums::ClusterId provides() const
  {
    return me->getOutputCluster();
  }

  std::set<enums::ClusterId> consumes() const
  {
    return me->getInputClusters();
  }

  bool attacheNode(Node node)
  {
    if(node.consumes().contains(provides())) {
      // This node provides data for the other node
      parents.emplace_back(node);
      return true;
    } else {
      for(auto &parent : parents) {
        if(parent.attacheNode(node)) {
          return true;
        }
      }
    }
    return false;
  }

  void printTree(int level = 0) const
  {
    std::string indent(level * 2, ' ');
    std::cout << indent << "L" << std::to_string(level) << ":" << me->meta.name << std::endl;

    for(const Node &parent : parents) {
      parent.printTree(level + 1);
    }
  }

  void orderPipeline(std::map<const settings::Pipeline *, int> &ordered, int level = 0) const
  {
    std::string indent(level * 2, ' ');
    std::cout << indent << "L" << std::to_string(level) << ":" << me->meta.name << std::endl;
    if(ordered.contains(me)) {
      if(ordered.at(me) < level) {
        ordered.at(me) = level;
      }
    } else {
      ordered.emplace(me, level);
    }

    for(const Node &parent : parents) {
      parent.orderPipeline(ordered, level + 1);
    }
  }

  const settings::Pipeline *me = nullptr;
  std::vector<Node> parents;
};

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto DependencyGraph::calcGraph(const joda::settings::AnalyzeSettings &settings) -> PipelineOrder_t
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
  std::vector<Node> rootNodes;
  {
    std::set<const settings::Pipeline *> attached;
    for(const auto *pipeline : pipelinesToAttache) {
      // If empty or input cluster is me
      if(pipeline->getInputClusters().empty() ||
         (pipeline->getInputClusters().size() == 1 &&
          *pipeline->getInputClusters().begin() == pipeline->pipelineSetup.defaultClusterId)) {
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
      for(const auto *pipeline : pipelinesToAttache) {
        bool isAttached = false;
        for(int n = 0; n < rootNodes.size(); n++) {
          auto &rootNode = rootNodes[n];
          if(rootNode.attacheNode({pipeline})) {
            attached.emplace(pipeline);
            isAttached = true;
          }
        }
        if(!isAttached) {
          notAttachedTrys[pipeline]++;
          if(notAttachedTrys[pipeline] > (pipelineCount * pipelineCount) * rootNodes.size()) {
            throw std::invalid_argument("Cycle detected for pipeline >" + pipeline->meta.name + "<!");
          }
        } else {
          notAttachedTrys.erase(pipeline);
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
  /*{
    for(auto &rootNode : rootNodes) {
      rootNode.printTree();
    }

    printOrder(finishedOrder);
  }*/
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
