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
#include "backend/enums/enums_classes.hpp"
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
      auto clusterIdToSet = static_cast<joda::enums::ClusterId>(clusterId.clusterId);
      auto classIdToSet   = static_cast<joda::enums::ClassId>(clusterId.classId);

      if(clusterId.clusterId == enums::ClusterIdIn::$) {
        clusterIdToSet = pipelineSetup.defaultClusterId;
      }

      if(clusterId.classId == enums::ClassIdIn::$) {
        classIdToSet = pipelineSetup.defaultClassId;
      }

      clusters.emplace(ClassificatorSettingOut{clusterIdToSet, classIdToSet});
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
    auto command = PipelineFactory<joda::cmd::Command>::generate(pipelineStep);
    if(command == nullptr) {
      continue;
    }
    const auto &clustersCmd = command->getOutputClasses();
    for(const auto &cluster : clustersCmd) {
      auto clusterIdToSet = static_cast<joda::enums::ClusterId>(cluster.clusterId);
      auto classIdToSet   = static_cast<joda::enums::ClassId>(cluster.classId);

      if(cluster.clusterId == enums::ClusterIdIn::$) {
        clusterIdToSet = pipelineSetup.defaultClusterId;
      }

      if(cluster.classId == enums::ClassIdIn::$) {
        classIdToSet = pipelineSetup.defaultClassId;
      }

      clusters.emplace(ClassificatorSettingOut{clusterIdToSet, classIdToSet});
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
