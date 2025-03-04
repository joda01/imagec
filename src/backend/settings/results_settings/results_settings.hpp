///
/// \file      results_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-03-04
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <string>
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/settings/setting.hpp"

namespace joda::settings {

struct ResultsSettings
{
  db::QueryFilter resultsTableTemplate;

  void check() const
  {
  }
  // We don't want to do a error check for the history
  void getErrorLogRecursive(SettingParserLog_t &settingsParserLog) const
  {
  }

private:
  std::string configSchema = "https://imagec.org/schemas/v1/results-settings.json";

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ResultsSettings, configSchema, resultsTableTemplate);
};
}    // namespace joda::settings
