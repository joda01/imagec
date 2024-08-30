///
/// \file      pipeline.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-30
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "pipeline.hpp"
#include <memory>
#include "backend/enums/enums_clusters.hpp"
#include "pipeline_factory.hpp"

namespace joda::settings {

///
/// \brief      Returns the input clusters this pipeline is using
/// \author     Joachim Danmayr
///
std::set<enums::ClusterId> Pipeline::getInputClusters() const
{
  std::set<enums::ClusterId> clusters;
  for(const auto &pipelineStep : pipelineSteps) {
    auto command            = PipelineFactory<joda::cmd::Command>::generate(pipelineStep);
    const auto &clustersCmd = command->getInputClusters();
    for(const auto &clusterId : clustersCmd) {
      if(clusterId == enums::ClusterIdIn::$) {
        clusters.emplace(pipelineSetup.defaultClusterId);
      } else {
        clusters.emplace(static_cast<enums::ClusterId>(clusterId));
      }
    }
  }
  return clusters;
}

///
/// \brief      Returns the cluster ID this pipeline is storing the results in
/// \author     Joachim Danmayr
///
enums::ClusterId Pipeline::getOutputCluster() const
{
  return pipelineSetup.defaultClusterId;
}

}    // namespace joda::settings
