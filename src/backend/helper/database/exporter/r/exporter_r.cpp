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

namespace joda::db {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void RExporter::startExport(const ExportSettings &settings, const settings::AnalyzeSettings &analyzeSettings, const std::string &jobName,
                            std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished,
                            std::string outputFileName)
{
  std::string rScript =
      "# \n"
      "# \n"
      "# \n"
      ""
      "install.packages(\"duckdb\")\n"
      "library(\"duckdb\")\n"
      "con <- dbConnect(duckdb(), dbdir = \"result.icdb\"++, read_only = TRUE)\n"
      "res <- dbGetQuery(con, \"SELECT * FROM items\")\n"
      "print(res)\n";

  switch(settings.exportDetail) {
    case ExportSettings::ExportDetail::PLATE:
      table = joda::db::StatsPerPlate::toTable(filter);
      break;
    case ExportSettings::ExportDetail::WELL:
      table = joda::db::StatsPerGroup::toTable(filter);
      break;
    case ExportSettings::ExportDetail::IMAGE:
      if(settings.exportType == ExportSettings::ExportType::TABLE_DETAIL) {
        table = joda::db::StatsPerImage::toTable(filter);
      } else {
        table = joda::db::StatsPerImage::toHeatmapList(filter);
      }
      break;
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
