///
/// \file      results_template.hpp
/// \author    Joachim Danmayr
/// \date      2025-04-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <string>
#include <utility>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/database/exporter/heatmap/export_heatmap_settings.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/project_settings/project_plate_setup.hpp"
#include "backend/settings/setting.hpp"
#include "results_settings.hpp"

namespace joda::settings {

class AnalyzeSettings;

///
/// \class      ResultsTemplate
/// \author     Joachim Danmayr
/// \brief      Template for results
///
struct ResultsTemplate
{
  struct TemplateEntry
  {
    std::vector<enums::Measurement> measureChannels;
    std::vector<enums::Stats> stats;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(TemplateEntry, measureChannels, stats);

    void check() const
    {
    }
    // We don't want to do a error check for the history
    void getErrorLogRecursive(SettingParserLog_t &settingsParserLog) const
    {
    }
  };

  std::vector<TemplateEntry> columns;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ResultsTemplate, columns);

  auto toSettings(const AnalyzeSettings &analyzeSettings) -> ResultsSettings;
  auto toSettings(const AnalyzeSettingsMeta &analyzeSettingsMeta, const std::map<enums::ClassId, joda::settings::Class> &classes) -> ResultsSettings;

  void check() const
  {
  }
  // We don't want to do a error check for the history
  void getErrorLogRecursive(SettingParserLog_t &settingsParserLog) const
  {
  }
};
}    // namespace joda::settings
