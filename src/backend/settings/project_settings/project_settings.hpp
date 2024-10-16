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
  // Object clusters used in this project
  //
  std::list<Cluster> clusters{
      {.clusterId = joda::enums::ClusterId::A, .name = "A"}, {.clusterId = joda::enums::ClusterId::B, .name = "B"},
      {.clusterId = joda::enums::ClusterId::C, .name = "C"}, {.clusterId = joda::enums::ClusterId::D, .name = "D"},
      {.clusterId = joda::enums::ClusterId::E, .name = "E"}, {.clusterId = joda::enums::ClusterId::F, .name = "F"},
      {.clusterId = joda::enums::ClusterId::G, .name = "G"}};

  //
  // Object classes used in this project
  //
  std::list<Class> classes{{.classId = joda::enums::ClassId::C0, .name = "Spot"},
                           {.classId = joda::enums::ClassId::C1, .name = "Tetraspeck"},
                           {.classId = joda::enums::ClassId::C2, .name = "Cell"},
                           {.classId = joda::enums::ClassId::C3, .name = "Nucleus"},
                           {.classId = joda::enums::ClassId::C4, .name = "Background"}};

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
      CHECK_ERROR(!clusters.empty(), "At least one cluster must be given!");
      std::set<enums::ClusterId> ids;
      for(const auto &element : clusters) {
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
      CHECK_ERROR(!classes.empty(), "At least one class must be given!");
      std::set<enums::ClassId> ids;
      for(const auto &element : classes) {
        if(element.classId == enums::ClassId::UNDEFINED) {
          THROW_ERROR("Class >UNDEFINED< is not allowed was used twice!");
        }
        if(ids.contains(element.classId)) {
          THROW_ERROR("Class ID >" + std::to_string((uint16_t) element.classId) + "< was used twice!");
        }
      }
    }
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ProjectSettings, experimentSettings, plates, address, clusters,
                                                       classes, workingDirectory);
};

}    // namespace joda::settings
