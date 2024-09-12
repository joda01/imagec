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
ObjectInputClustersExp Pipeline::getInputClusters() const
{
  ObjectInputClustersExp clusters;
  for(const auto &pipelineStep : pipelineSteps) {
    auto command            = PipelineFactory<joda::cmd::Command>::generate(pipelineStep);
    const auto &clustersCmd = command->getInputClusters();
    for(const auto &clusterId : clustersCmd) {
      if(clusterId.clusterId == enums::ClusterIdIn::$) {
        clusters.emplace(ClassificatorSettingOut{pipelineSetup.defaultClusterId, clusterId.classId});
      } else {
        clusters.emplace(
            ClassificatorSettingOut{static_cast<joda::enums::ClusterId>(clusterId.clusterId), clusterId.classId});
      }
    }
  }
  return clusters;
}

///
/// \brief      Returns the cluster ID this pipeline is storing the results in
/// \author     Joachim Danmayr
///
ObjectOutputClustersExp Pipeline::getOutputClasses() const
{
  ObjectOutputClustersExp clusters;
  for(const auto &pipelineStep : pipelineSteps) {
    auto command            = PipelineFactory<joda::cmd::Command>::generate(pipelineStep);
    const auto &clustersCmd = command->getOutputClasses();
    for(const auto &cluster : clustersCmd) {
      if(cluster.clusterId == enums::ClusterIdIn::$) {
        clusters.emplace(
            ClassificatorSettingOut{.clusterId = pipelineSetup.defaultClusterId, .classId = cluster.classId});
      } else {
        clusters.emplace(ClassificatorSettingOut{.clusterId = static_cast<joda::enums::ClusterId>(cluster.clusterId),
                                                 .classId   = cluster.classId});
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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Pipeline::check() const
{
  bool hasSaveImage = false;
  bool hasMeasure   = false;

  for(const auto &command : pipelineSteps) {
    if(command.$saveImage.has_value()) {
      hasSaveImage = true;
    }

    if(command.$measure.has_value()) {
      hasMeasure = true;
    }
  }

  CHECK_INFO(hasSaveImage, "Pipeline does not store control image!");
  CHECK_WARNING(hasMeasure, "Pipeline does not measure anything!");
}

}    // namespace joda::settings
