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
#include "../anaylze_settings_enums.hpp"
#include <nlohmann/json.hpp>
#include "project_address.hpp"
#include "project_experiment_setup.hpp"
#include "project_object_class.hpp"

namespace joda::settings {

class ProjectSettings
{
public:
  //
  // How the experiment is setup
  //
  ExperimentSetup experimentSetup;

  //
  // Notes to the experiment
  //
  std::string notes;

  //
  // The address of the experiment
  //
  Address address;

  //
  // Object classes used in this project
  //
  std::map<ObjectClassId, ObjectClass> objectClasses;

  //
  // Working directory
  //
  std::string workingDirectory;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ProjectSettings, experimentSetup, notes, address, objectClasses,
                                              workingDirectory);
};

}    // namespace joda::settings
