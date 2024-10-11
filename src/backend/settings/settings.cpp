///
/// \file      settings.cpp
/// \author    Joachim Danmayr
/// \date      2024-04-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "settings.hpp"
#include <exception>
#include <filesystem>
#include <stdexcept>
#include <string>
#include "backend/enums/enums_clusters.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/analze_settings.hpp"

namespace joda::settings {

void Settings::storeSettings(const std::filesystem::path &pathIn, const joda::settings::AnalyzeSettings &settings)
{
  std::string path = pathIn.string();
  if(!path.empty()) {
    nlohmann::json json = settings;
    removeNullValues(json);

    if(!path.ends_with(joda::fs::EXT_PROJECT)) {
      path += joda::fs::EXT_PROJECT;
    }
    std::ofstream out(path);
    if(!out.is_open()) {
      throw std::runtime_error("Cannot open file >" + path + "< for writing! Do you have write permissions?");
    }
    out << json.dump(2);
    if(out.bad()) {
      throw std::runtime_error("Cannot write data! Do you have write permissions and enough space left on your disk?");
    }
    out.close();
  }
}

std::string Settings::toString(const joda::settings::AnalyzeSettings &settings)
{
  nlohmann::json json = settings;
  removeNullValues(json);
  return json.dump(2);
}

/// \todo How to check incomplete settings
bool Settings::isEqual(const joda::settings::AnalyzeSettings &settingsOld, const joda::settings::AnalyzeSettings &settingsNew)
{
  try {
    nlohmann::json jsonOld = settingsOld;
    nlohmann::json jsonNew = settingsNew;
    return jsonOld.dump() == jsonNew.dump();

  } catch(const std::exception &ex) {
    joda::log::logError("Cannot compare: " + std::string(ex.what()));
  }
  return false;
}

}    // namespace joda::settings
