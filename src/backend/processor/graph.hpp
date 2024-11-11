///
/// \file      graph.hpp
/// \author    Joachim Danmayr
/// \date      2024-09-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include "backend/enums/enums_clusters.hpp"
#include "backend/settings/pipeline/pipeline.hpp"

namespace joda::processor {

class Node
{
public:
  /////////////////////////////////////////////////////
  Node(const settings::Pipeline *me);
  std::set<settings::ClassificatorSettingOut> provides() const;
  std::set<settings::ClassificatorSettingOut> consumes() const;
  bool attacheNode(Node node);
  void printTree(int level = 0) const;
  void orderPipeline(std::map<const settings::Pipeline *, int> &ordered, int level = 0) const;
  auto pipeline() const -> const settings::Pipeline *;
  std::vector<Node> findParents(const settings::Pipeline *start) const;

private:
  /////////////////////////////////////////////////////
  const settings::Pipeline *me = nullptr;
  std::vector<Node> children;
};

}    // namespace joda::processor
