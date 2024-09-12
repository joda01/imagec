///
/// \file      experiment_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-16
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
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ExperimentSettings
{
  //
  // Global unique ID of the job
  //
  std::string experimentId;

  //
  // Unique name of the job
  //
  std::string experimentName;

  //
  // Notes on the job
  //
  std::string notes;

  void check() const
  {
    // CHECK_ERROR(!experimentId.empty(), "Experiment ID is empty.");
    // CHECK_ERROR(!experimentName.empty(), "Experiment name is empty.");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ExperimentSettings, experimentId, experimentName, notes);
};

}    // namespace joda::settings
