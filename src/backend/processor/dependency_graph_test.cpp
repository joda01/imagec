///
/// \file      dependency_graph_test.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-30
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "backend/helper/duration_count/duration_count.h"
#include "backend/processor/dependency_graph.hpp"
#include "backend/settings/analze_settings.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <nlohmann/json.hpp>

namespace joda::test {

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
SCENARIO("pipeline:test:dependencygraph", "[dependencygraph]")
{
  std::ifstream file("src/backend/processor/test/test_run.json");
  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(file);
  auto i                                   = DurationCount::start("Graphj");
  joda::processor::DependencyGraph::calcGraph(settings);
  DurationCount::stop(i);
}
}    // namespace joda::test

/*
L0:EV detection c7
  L1:Cell brightfiel
    L2:Voronoi
  L1:Colocalization
L0:Nucle detection
  L1:Cell brightfiel
    L2:Voronoi
  L1:Voronoi
L0:EV detection c5
  L1:Cell brightfiel
    L2:Voronoi
  L1:Colocalization
0) EV detection c7,Nucle detection,EV detection c5,
1) Cell brightfiel,Colocalization,
2) Voronoi,

*/
