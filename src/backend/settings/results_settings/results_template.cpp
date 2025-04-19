///
/// \file      results_template.cpp
/// \author    Joachim Danmayr
/// \date      2025-04-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "results_template.hpp"
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/analze_settings.hpp"

namespace joda::settings {

///
/// \brief      Generate results table settings based on template
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto ResultsTemplate::toSettings(const AnalyzeSettings &analyzeSettings) -> ResultsSettings
{
  std::map<enums::ClassId, std::string> classNames;
  for(const auto &classs : analyzeSettings.projectSettings.classification.classes) {
    classNames.emplace(classs.classId, classs.name);
  }

  auto outputClasses               = analyzeSettings.getOutputClasses();
  auto possibleIntersecting        = analyzeSettings.getPossibleIntersectingClasses();
  auto imageChannelsUsedForMeasure = analyzeSettings.getImageChannelsUsedForMeasurement();

  std::map<enums::ClassId, joda::settings::Class> classes;
  for(const auto &cl : analyzeSettings.projectSettings.classification.classes) {
    classes.emplace(cl.classId, cl);
  }

  return toSettings(AnalyzeSettingsMeta{outputClasses, possibleIntersecting, imageChannelsUsedForMeasure}, classes);
}
///
/// \brief      Generate results table settings based on template
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto ResultsTemplate::toSettings(const AnalyzeSettingsMeta &analyzeSettings, const std::map<enums::ClassId, joda::settings::Class> &classes)
    -> ResultsSettings
{
  ResultsSettings settings;

  int32_t colIdx = 0;
  for(const auto &entry : columns) {
    for(const auto &classId : analyzeSettings.outputClasses) {
      if(classId == enums::ClassId::UNDEFINED || classId == enums::ClassId::NONE) {
        continue;
      }
      for(const auto &measure : entry.measureChannels) {
        auto addColumn = [&](enums::Stats stat, int32_t crossChannel, enums::ClassId intersecting, const std::string &channelName = "") {
          if(!classes.contains(classId)) {
            joda::log::logError("Class name for ID >" + std::to_string((int32_t) classId) + "<not found during table generation from template!");
            return;
          }
          std::string intersectingName;
          if(intersecting != enums::ClassId::UNDEFINED) {
            if(!classes.contains(intersecting)) {
              joda::log::logError("Intersecting class name for ID >" + std::to_string((int32_t) intersecting) +
                                  "<not found during table generation from template!");
              return;
            }
            intersectingName = classes.at(intersecting).name;
          }

          settings.addColumn(ResultsSettings::ColumnIdx{.tabIdx = 0, .colIdx = colIdx},
                             ResultsSettings::ColumnKey{.classId             = classId,
                                                        .measureChannel      = measure,
                                                        .stats               = stat,
                                                        .crossChannelStacksC = crossChannel,
                                                        .intersectingChannel = intersecting,
                                                        .zStack              = 0,
                                                        .tStack              = 0},
                             ResultsSettings::ColumnName{
                                 .crossChannelName = channelName, .className = classes.at(classId).name, .intersectingName = intersectingName});
          colIdx++;
        };

        //
        // For count only sum makes sense
        //
        if(measure == enums::Measurement::COUNT) {
          addColumn(enums::Stats::SUM, -1, enums::ClassId::UNDEFINED);
          continue;
        }

        //
        // For object IDs stats does not make sense
        //
        if(measure == enums::Measurement::OBJECT_ID || measure == enums::Measurement::ORIGIN_OBJECT_ID ||
           measure == enums::Measurement::PARENT_OBJECT_ID || measure == enums::Measurement::TRACKING_ID) {
          addColumn(enums::Stats::OFF, -1, enums::ClassId::UNDEFINED);
          continue;
        }

        //
        // For intersecting only AVG and SUM makes sense
        //
        if(measure == enums::Measurement::INTERSECTING) {
          // Add only those intersecting classes which appear in the pipeline
          if(analyzeSettings.intersectingClasses.contains(classId)) {
            for(const auto &intersectingClassId : analyzeSettings.intersectingClasses.at(classId)) {
              if(intersectingClassId == enums::ClassId::UNDEFINED || intersectingClassId == enums::ClassId::NONE) {
                continue;
              }
              if(entry.stats.contains(enums::Stats::AVG)) {
                addColumn(enums::Stats::AVG, -1, intersectingClassId);
              }
              if(entry.stats.contains(enums::Stats::SUM)) {
                addColumn(enums::Stats::SUM, -1, intersectingClassId);
              }
            }
          }
          continue;
        }

        for(auto stat : entry.stats) {
          if(measure == enums::Measurement::INTENSITY_SUM || measure == enums::Measurement::INTENSITY_AVG ||
             measure == enums::Measurement::INTENSITY_MIN || measure == enums::Measurement::INTENSITY_MAX) {
            // Iterate over cross channels
            if(analyzeSettings.measuredChannels.contains(classId)) {
              for(const auto &channelId : analyzeSettings.measuredChannels.at(classId)) {
                addColumn(stat, channelId, enums::ClassId::UNDEFINED, "CH " + std::to_string(channelId));
              }
            }
          } else {
            addColumn(stat, -1, enums::ClassId::UNDEFINED);
          }
        }
      }
    }
  }
  settings.sortColumns();
  return settings;
}

}    // namespace joda::settings
