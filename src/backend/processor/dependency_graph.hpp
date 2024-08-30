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

#pragma once

#include <map>
#include <set>
#include <vector>

namespace joda::settings {
class AnalyzeSettings;
class Pipeline;
}    // namespace joda::settings

namespace joda::processor {

using PipelineOrder_t = std::map<int, std::set<const settings::Pipeline *>>;

///
/// \class      DependencyGraph
/// \author     Joachim Danmayr
/// \brief      Calculates the pipeline dependency graph
///             based on the input and output clusters of
///             the pipelines of an analyze setting
///
class DependencyGraph
{
public:
  static auto calcGraph(const joda::settings::AnalyzeSettings &) -> PipelineOrder_t;
  static void printOrder(const PipelineOrder_t &order);
};

}    // namespace joda::processor
