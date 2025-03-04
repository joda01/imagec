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
#include "backend/enums/enums_file_endians.hpp"
#include "backend/settings/program/program_meta.hpp"
#include "pipeline/pipeline.hpp"
#include "project_settings/project_settings.hpp"
#include "results_settings/results_settings.hpp"
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

namespace joda::settings {

class AnalyzeSettings final
{
public:
  ProjectSettings projectSettings;
  ResultsSettings resultsSettings;
  ProjectImageSetup imageSetup;
  std::list<Pipeline> pipelines;
  ProgramMeta imagecMeta;
  SettingsMeta meta;

  [[nodiscard]] const std::string &schema() const
  {
    return configSchema;
  }

  std::set<joda::enums::ClassId> getOutputClasses() const;
  std::set<joda::enums::ClassId> getInputClasses() const;

  auto checkForErrors() const -> std::vector<std::pair<std::string, SettingParserLog_t>>;

private:
  std::string configSchema = "https://imagec.org/schemas/v1/analyze-settings.json";
  void check() const;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(AnalyzeSettings, configSchema, projectSettings, resultsSettings, imageSetup, pipelines,
                                                       imagecMeta, meta);
};
}    // namespace joda::settings
