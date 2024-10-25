///
/// \file      project_cluster_classes.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <set>
#include <vector>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/global_enums.hpp"
#include "backend/settings/pipeline/pipeline_meta.hpp"
#include <nlohmann/json.hpp>
#include "experiment_settings.hpp"
#include "project_address.hpp"
#include "project_class.hpp"
#include "project_cluster.hpp"
#include "project_image_setup.hpp"
#include "project_plates.hpp"

namespace joda::settings {

struct ClusterClasses
{
  //
  // Name
  //
  PipelineMeta meta;

  //
  // Object clusters used in this project
  //
  std::list<Cluster> clusters{{.clusterId = joda::enums::ClusterId::A, .name = "A"}, {.clusterId = joda::enums::ClusterId::B, .name = "B"},
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

  void check() const
  {
  }

  [[nodiscard]] const std::string &schema() const
  {
    return configSchema;
  }

  std::string configSchema = "https://imagec.org/schemas/v1/classification-settings.json";
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ClusterClasses, configSchema, meta, clusters, classes);
};

}    // namespace joda::settings
