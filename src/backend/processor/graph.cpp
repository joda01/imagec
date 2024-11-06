///
/// \file      graph.cpp
/// \author    Joachim Danmayr
/// \date      2024-09-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "graph.hpp"
#include <string>

namespace joda::processor {

Node::Node(const settings::Pipeline *me) : me(me)
{
}

std::set<settings::ClassificatorSettingOut> Node::provides() const
{
  return me->getOutputClasses();
}

std::set<settings::ClassificatorSettingOut> Node::consumes() const
{
  std::set<settings::ClassificatorSettingOut> out;
  for(const auto &element : me->getInputClusters()) {
    out.emplace(element.clusterId);
  }

  return out;
}

bool Node::attacheNode(Node node)
{
  bool attached = false;
  for(auto &child : children) {
    if(child.attacheNode(node)) {
      attached = true;
    }
  }

  bool hasCommon =
      std::any_of(node.consumes().begin(), node.consumes().end(), [&](auto element) { return provides().find(element) != provides().end(); });

  if(node.me != me && hasCommon) {
    // This node provides data for the other node
    bool stillExists = false;
    for(const auto &child : children) {
      if(child.pipeline() == node.me) {
        stillExists = true;
        break;
      }
    }
    if(!stillExists) {
      children.emplace_back(node);
    }
    attached = true;
  }

  return attached;
}

void Node::printTree(int level) const
{
  std::string indent(level * 2, ' ');
  std::cout << indent << "L" << std::to_string(level) << ":" << me->meta.name << std::endl;

  for(const Node &child : children) {
    child.printTree(level + 1);
  }
}

void Node::orderPipeline(std::map<const settings::Pipeline *, int> &ordered, int level) const
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

  for(const Node &parent : children) {
    parent.orderPipeline(ordered, level + 1);
  }
}

std::vector<Node> Node::findParents(const settings::Pipeline *target) const
{
  std::vector<Node> parents;
  for(const Node &node : children) {
    if(node.pipeline() == target) {
      parents.push_back(me);
    } else {
      std::vector<Node> child_parents = node.findParents(target);
      if(!child_parents.empty()) {
        parents.insert(parents.end(), child_parents.begin(), child_parents.end());
        parents.push_back(*this);
      }
    }
  }
  return parents;
}

auto Node::pipeline() const -> const settings::Pipeline *
{
  return me;
}

}    // namespace joda::processor
