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
#include "backend/helper/database/plugins/helper.hpp"
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
void RExporter::startExport(const ExportSettings &settings, const settings::AnalyzeSettings &analyzeSettings, const std::string &jobName,
                            std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished,
                            std::string outputFileName)
{
  /*
  std::vector<std::string> sqlStatements;

  if(settings.clustersToExport.empty()) {
    return;
  }
  enums::ClusterId actClusterId   = settings.clustersToExport.begin()->first.clusterId;
  std::string actImageClusterName = settings.clustersToExport.begin()->second.clusterName;

  int loopCount = 0;
  for(const auto &[clusterAndClassId, imageChannel] : settings.clustersToExport) {
    if(actClusterId != clusterAndClassId.clusterId) {
      actClusterId        = clusterAndClassId.clusterId;
      actImageClusterName = imageChannel.clusterName;
    }

    for(const auto &[measureChannelId, statsIn] : imageChannel.measureChannels) {
      for(const auto stats : statsIn) {
        auto generate = [&, clusterId = clusterAndClassId.clusterId, classId = clusterAndClassId.classId, className = imageChannel.className,
                         measureChannelId = measureChannelId](uint32_t cStack, const std::string &crossChannelChannelCName,
                                                              std::pair<enums::ClusterId, std::string> crossChannelCluster,
                                                              std::pair<enums::ClassId, std::string> crossChannelClass) {
          auto filter = joda::db::QueryFilter{.analyzer                = &settings.analyzer,
                                              .plateRows               = settings.plateRows,
                                              .plateCols               = settings.plateCols,
                                              .plateId                 = settings.plateId,
                                              .actGroupId              = settings.groupId,
                                              .actImageId              = settings.imageId,
                                              .clusterId               = clusterId,
                                              .classId                 = classId,
                                              .className               = className,
                                              .measurementChannel      = measureChannelId,
                                              .stats                   = stats,
                                              .wellImageOrder          = settings.wellImageOrder,
                                              .densityMapAreaSize      = settings.heatmapAreaSize,
                                              .crossChanelStack_c      = cStack,
                                              .crossChannelStack_cName = crossChannelChannelCName};

          std::pair<std::string, DbArgs_t> sqlData;
          switch(settings.exportDetail) {
            case ExportSettings::ExportDetail::PLATE:
              sqlData = joda::db::StatsPerPlate::toSQL(filter);
              break;
            case ExportSettings::ExportDetail::WELL:
              sqlData = joda::db::StatsPerGroup::toSQL(filter);
              break;
            case ExportSettings::ExportDetail::IMAGE:
              if(settings.exportType == ExportSettings::ExportType::TABLE_DETAIL) {
                sqlData = joda::db::StatsPerImage::toSqlTable(filter);
              } else {
                sqlData = joda::db::StatsPerImage::toSqlHeatmap(filter);
              }
              break;
          }

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
          auto command = "res_" + std::to_string(loopCount) + " <- dbGetQuery(con, \"" + sqlData.first + "\", list(" + arguments + "))\n";
          sqlStatements.emplace_back(command);
          loopCount++;
        };
        if(getType(measureChannelId) == joda::db::MeasureType::INTENSITY) {
          for(const auto &[cStack, name] : imageChannel.crossChannelStacksC) {
            generate(cStack, name, {}, {});
          }
        } else {
          generate(0, "", {}, {});
        }
      }
    }
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
  }*/
}

}    // namespace joda::db
