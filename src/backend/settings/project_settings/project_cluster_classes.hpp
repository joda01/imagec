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
#include "backend/settings/setting.hpp"
#include "backend/settings/settings_types.hpp"
#include <nlohmann/json.hpp>
#include "experiment_settings.hpp"
#include "project_address.hpp"
#include "project_class.hpp"
#include "project_cluster.hpp"
#include "project_cluster_class_colors.hpp"
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
  std::list<Cluster> clusters{};

  //
  // Object classes used in this project
  //
  std::list<Class> classes{};

  //
  // Color assignment for a cluster/class combination
  //
  ClusterClassColorAssignments colors;

  void check() const
  {
  }

  [[nodiscard]] const std::string &schema() const
  {
    return configSchema;
  }

  std::string configSchema = "https://imagec.org/schemas/v1/classification-settings.json";
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ClusterClasses, configSchema, meta, clusters, classes, colors);
};

}    // namespace joda::settings
