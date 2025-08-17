///
/// \file      exporter_r.cpp
/// \author    Joachim Danmayr
/// \date      2025-07-04
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "exporter_r.hpp"
#include <string>
#include "../xlsx/exporter_xlsx.hpp"
#include "backend/helper/logger/console_logger.hpp"

namespace joda::exporter::r {

void Exporter::startExport(const std::vector<const Exportable *> &data, const settings::AnalyzeSettings &analyzeSettings, const std::string &jobName,
                           std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished,
                           const std::string &outputFileName)
{
  std::string xlsxOutFile = outputFileName;
  helper::stringReplace(xlsxOutFile, ".r", ".xlsx");
  joda::exporter::xlsx::Exporter::startExport(data, analyzeSettings, jobName, timeStarted, timeFinished, xlsxOutFile);

  std::string rScript =
      "# \n"
      "# \\author  " +
      analyzeSettings.projectSettings.address.firstName + " " + analyzeSettings.projectSettings.address.lastName +
      "\n"
      "# \\date  " +
      helper::timepointToIsoString(timeStarted) + "\n" + "# \\version " + analyzeSettings.imagecMeta.imagecVersion +
      "\n"
      "\n"
      "# Main\n"
      "# Press Alt+Ctrl+R to execute the script\n"
      "#\n";

  rScript += R"(
library(readxl)
library(ggplot2)
)";
  rScript += "file_name <- \"" + xlsxOutFile + "\"\n\n";

  // Iterate over the sheets
  int32_t index = 1;
  for(const auto *dataIn : data) {
    std::string name = dataIn->getTitle();
    // Max. sheet name length = 31 because of excel limitation
    if(name.size() > 28) {
      name = name.substr(0, 28);
    }
    name.erase(std::remove_if(name.begin(), name.end(),
                              [](char c) { return !std::isalnum(static_cast<unsigned char>(c)) && static_cast<unsigned char>(c) != ' '; }),
               name.end());
    helper::stringReplace(name, " ", "_");
    name += "_" + std::to_string(index);
    rScript += "data_" + std::to_string(index) + " <- read_excel(file_name, sheet = \"" + name + "\")\n";
    index++;
  }

  // ==================================================
  // Write R file
  // ===================================================
  std::ofstream outFile(outputFileName);
  if(outFile) {
    outFile << rScript;
  } else {
    joda::log::logError("Could not write file >" + rScript + "<");
  }
}

}    // namespace joda::exporter::r
