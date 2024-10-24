///
/// \file      exporter_r.cpp
/// \author    Joachim Danmayr
/// \date      2024-10-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "exporter_r.hpp"
#include <iostream>
#include <ostream>
#include <string>
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/helper/database/plugins/stats_for_image.hpp"
#include "backend/helper/database/plugins/stats_for_well.hpp"
#include "backend/helper/helper.hpp"

namespace joda::db {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void RExporter::startExport(const db::QueryFilter &filter, db::StatsPerGroup::Grouping grouping, const settings::AnalyzeSettings &analyzeSettings,
                            const std::string &jobName, std::chrono::system_clock::time_point timeStarted,
                            std::chrono::system_clock::time_point timeFinished, std::string outputFileName)
{
  std::vector<std::string> sqlStatements;
  int32_t loopCount = 0;
  for(const auto &cc : filter.getClustersAndClassesToExport()) {
    auto createSQL = [&sqlStatements](const std::pair<std::string, DbArgs_t> &sqlData, int32_t loopCount) {
      std::string arguments;
      for(const auto &arg : sqlData.second) {
        if(std::holds_alternative<std::string>(arg)) {
          arguments += "'" + (std::get<std::string>(arg)) + "', ";
        } else if(std::holds_alternative<uint16_t>(arg)) {
          arguments += std::to_string((std::get<uint16_t>(arg))) + ", ";
        } else if(std::holds_alternative<uint32_t>(arg)) {
          arguments += std::to_string((std::get<uint32_t>(arg))) + ", ";
          ;
        } else if(std::holds_alternative<uint64_t>(arg)) {
          arguments += std::to_string((std::get<uint64_t>(arg))) + ", ";
          ;
        } else if(std::holds_alternative<double>(arg)) {
          arguments += std::to_string((std::get<double>(arg))) + ", ";
        }
      }
      if(arguments.size() >= 2) {
        arguments = arguments.erase(arguments.size() - 2, 2);
      }
      auto command = "result_" + std::to_string(loopCount) + " <- dbGetQuery(con, \"" + sqlData.first + "\", list(" + arguments + "))\n";
      sqlStatements.emplace_back(command);
    };

    switch(grouping) {
      case db::StatsPerGroup::Grouping::BY_PLATE:
      case db::StatsPerGroup::Grouping::BY_WELL:
        createSQL(joda::db::StatsPerGroup::toSQL(cc.first, filter.getFilter(), cc.second, grouping), loopCount);
        break;
      case db::StatsPerGroup::Grouping::BY_IMAGE:
        createSQL(joda::db::StatsPerImage::toSqlTable(cc.first, filter.getFilter(), cc.second), loopCount);
        break;
    }
    loopCount++;
  }

  std::string rScript =
      "# \n"
      "# \\author  " +
      analyzeSettings.projectSettings.address.firstName + " " + analyzeSettings.projectSettings.address.lastName +
      "\n"
      "# \\date  " +
      helper::timepointToIsoString(timeStarted) + "\n" + "# \\version " + analyzeSettings.meta.imagecVersion +
      "\n"
      "\n"
      "# Be sure the duckdb package is installed. By executing the following line, the package will be installed.\n"
      "# You have to run the install package only once on your computer\n"
      "# install.packages(\"duckdb\")\n"
      "\n"
      "library(\"duckdb\")\n"
      "\n"
      "#\n"
      "# Main\n"
      "# Press Alt+Ctrl+R to execute the script\n"
      "#\n"
      "con <- dbConnect(duckdb(), dbdir = \"results.icdb\", read_only = TRUE)\n";

  for(const auto &selects : sqlStatements) {
    rScript += selects;
  }

  if(!outputFileName.ends_with(".r")) {
    outputFileName += ".r";
  }
  // Create an output file stream (ofstream) object
  std::ofstream outFile(outputFileName);

  // Check if the file is open
  if(outFile.is_open()) {
    // Write the string to the file
    outFile << rScript;

    // Close the file
    outFile.close();

  } else {
    throw std::runtime_error("Could not write R script.");
  }
}

}    // namespace joda::db
