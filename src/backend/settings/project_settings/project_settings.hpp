///
/// \file      project_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

#pragma once

#include <set>
#include <vector>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/global_enums.hpp"
#include <nlohmann/json.hpp>
#include "experiment_settings.hpp"
#include "project_address.hpp"
#include "project_class.hpp"
#include "project_cluster.hpp"
#include "project_cluster_classes.hpp"
#include "project_image_setup.hpp"
#include "project_plates.hpp"

namespace joda::settings {

class ProjectSettings
{
public:
  //
  // Job settings
  //
  ExperimentSettings experimentSettings;

  //
  // Plates analyzed by this run
  //
  std::list<Plate> plates = {{}};

  //
  // The address of the experiment
  //
  Address address;

  //
  // Cluster and classes
  //
  ClusterClasses classification;

  //
  // Directory where outputs will be stored
  //
  std::string workingDirectory;

  void check() const
  {
    CHECK_ERROR(!workingDirectory.empty(), "Working directory must not be empty!");
    // Check plates
    {
      CHECK_ERROR(!plates.empty(), "At least one plate must be given!");
      std::set<uint8_t> ids;
      for(const auto &plate : plates) {
        if(ids.contains(plate.plateId)) {
          THROW_ERROR("Plate ID >" + std::to_string(plate.plateId) + "< was used twice!");
        }
      }
    }
    // Check clusters
    {
      CHECK_ERROR(!classification.clusters.empty(), "At least one cluster must be given!");
      std::set<enums::ClusterId> ids;
      for(const auto &element : classification.clusters) {
        if(element.clusterId == enums::ClusterId::UNDEFINED) {
          THROW_ERROR("Cluster >UNDEFINED< is not allowed was used twice!");
        }
        if(ids.contains(element.clusterId)) {
          THROW_ERROR("Cluster ID >" + std::to_string((uint16_t) element.clusterId) + "< was used twice!");
        }
      }
    }
    // Check classes
    {
      CHECK_ERROR(!classification.classes.empty(), "At least one class must be given!");
      std::set<enums::ClassId> ids;
      for(const auto &element : classification.classes) {
        if(element.classId == enums::ClassId::UNDEFINED) {
          THROW_ERROR("Class >UNDEFINED< is not allowed was used twice!");
        }
        if(ids.contains(element.classId)) {
          THROW_ERROR("Class ID >" + std::to_string((uint16_t) element.classId) + "< was used twice!");
        }
      }
    }
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ProjectSettings, experimentSettings, plates, address, classification, workingDirectory);
};

}    // namespace joda::settings
