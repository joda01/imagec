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
  enums::Measurement measureChannel = enums::Measurement::NONE;
  enums::Stats stats                = enums::Stats::AVG;
  // int32_t crossChannelStacksC              = -1;
  // joda::enums::ClassId intersectingChannel = joda::enums::ClassId::NONE;
  // int32_t zStack                           = 0;
  // int32_t tStack                           = 0;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ResultsTemplate, measureChannel, stats);

  auto toSettings(const AnalyzeSettings &analyzeSettings) -> ResultsSettings;

  void check() const
  {
  }
  // We don't want to do a error check for the history
  void getErrorLogRecursive(SettingParserLog_t &settingsParserLog) const
  {
  }
};
}    // namespace joda::settings
