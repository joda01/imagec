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
/// \brief     A short description what happens here.
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
#include "backend/commands/functions/image_loader/image_loader_settings.hpp"
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
  cmd::functions::ImageLoaderSettings imageLoader;
  std::vector<Pipeline> pipelines;

  [[nodiscard]] const std::string &schema() const
  {
    return configSchema;
  }

private:
  std::string configSchema = "https://imagec.org/schemas/v1/analyze-settings.icproj";

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AnalyzeSettings, projectSettings, imageLoader, pipelines);
};
}    // namespace joda::settings
