///
/// \file      project_classification.hpp
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
#include "backend/global_enums.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/settings_meta.hpp"
#include "backend/settings/settings_types.hpp"
#include <nlohmann/json.hpp>
#include "experiment_settings.hpp"
#include "project_address.hpp"
#include "project_class.hpp"
#include "project_image_setup.hpp"
#include "project_plates.hpp"

namespace joda::settings {

struct Classification
{
  //
  // Name
  //
  SettingsMeta meta;

  //
  // Object classes used in this project
  //
  std::list<Class> classes{};

  void check() const
  {
  }

  [[nodiscard]] const std::string &schema() const
  {
    return configSchema;
  }

  std::string configSchema = "https://imagec.org/schemas/v1/classification-settings.json";
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Classification, configSchema, meta, classes);
};

}    // namespace joda::settings
