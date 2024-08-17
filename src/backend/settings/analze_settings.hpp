///
/// \file      analyze_settings.hpp
/// \author    Joachim Danmayr
/// \date      2023-04-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <cstddef>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include "pipeline/pipeline.hpp"
#include "project_settings/project_settings.hpp"
#include <catch2/catch_config.hpp>
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

namespace joda::settings {

class AnalyzeSettings final
{
public:
  ProjectSettings projectSettings;
  std::vector<Pipeline> pipelines;

  [[nodiscard]] const std::string &schema() const
  {
    return configSchema;
  }

  void check() const
  {
    if(projectSettings.imageSetup.tStackHandling == ProjectImageSetup::TStackHandling::EXACT_ONE) {
      for(const auto &pip : pipelines) {
        CHECK(pip.pipelineSetup.tStackIndex >= 0, "When processing exact one t stack image, define which one!");
      }
    }

    if(projectSettings.imageSetup.zStackHandling == ProjectImageSetup::ZStackHandling::EXACT_ONE) {
      for(const auto &pip : pipelines) {
        CHECK(pip.pipelineSetup.zStackIndex >= 0, "When processing exact one z stack image, define which one!");
      }
    }
  }

private:
  std::string configSchema = "https://imagec.org/schemas/v1/analyze-settings.icproj";

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(AnalyzeSettings, projectSettings, pipelines);
};
}    // namespace joda::settings
