///
/// \file      settings.cpp
/// \author    Joachim Danmayr
/// \date      2024-04-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "settings.hpp"
#include <exception>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/enums/enums_units.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/analze_settings.hpp"

namespace joda::settings {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto Settings::openSettings(const std::filesystem::path &pathIn) -> joda::settings::AnalyzeSettings
{
  std::ifstream ifs(pathIn);
  if(!ifs) {
    return {};
  }
  std::string wholeFile;
  size_t size = std::filesystem::file_size(pathIn);
  wholeFile.resize(size);
  ifs.read(wholeFile.data(), static_cast<std::streamsize>(size));

  ifs.close();
  migrateSettings(wholeFile);

  auto parsed = nlohmann::json::parse(wholeFile);

  //
  //
  /// \todo Remove legacy
  bool doesPipelineSetupExists = true;
  {
    if(!parsed.contains("pipelineSetup")) {
      doesPipelineSetupExists = false;
    }
  }

  joda::settings::AnalyzeSettings analyzeSettings = parsed;

  //
  //
  /// \todo Remove legacy
  {
    if(!doesPipelineSetupExists) {
      analyzeSettings.pipelineSetup.realSizesUnit = enums::Units::Pixels;
    }
  }

  //
  // Further legacy migration
  /// \todo REMOVE, legacy
  {
    if(!analyzeSettings.projectSettings.plates.empty()) {
      analyzeSettings.projectSettings.plate = *analyzeSettings.projectSettings.plates.begin();
      analyzeSettings.projectSettings.plates.clear();
    }
  }    // namespace joda::settings

  return analyzeSettings;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Settings::migrateSettings(std::string &settings)
{
  helper::stringReplace(settings, "$edgeDetection", "$sobel");
  helper::stringReplace(settings, "\"$measure\"", "\"$measureIntensity\"");
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Settings::storeSettings(const std::filesystem::path &pathIn, const joda::settings::AnalyzeSettings &settings)
{
  std::string path = pathIn.string();
  if(!path.empty()) {
    settings.meta.setModifiedAtDateToNow();
    nlohmann::json json = settings;
    removeNullValues(json);

    /// \todo REMOVE, legacy
    {
      json["projectSettings"].erase("plates");
    }

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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Settings::storeSettingsTemplate(const std::filesystem::path &pathIn, joda::settings::AnalyzeSettings settings, const SettingsMeta &settingsMeta)
{
  std::string path = pathIn.string();
  if(!path.empty()) {
    // Set modified at
    settings.meta = settingsMeta;
    settings.meta.setModifiedAtDateToNow();

    //
    // Remove settings
    //
    settings.projectSettings.workingDirectory                = "";
    settings.projectSettings.experimentSettings.experimentId = "";
    settings.projectSettings.plate                           = {{}};
    settings.projectSettings.address                         = {};
    for(auto &pipeline : settings.pipelines) {
      pipeline.meta.icon = "";
      pipeline.eraseHistory();
    }

    //
    // Convert to json
    //
    nlohmann::json json = settings;
    removeNullValues(json);

    /// \todo REMOVE, legacy
    {
      json["projectSettings"].erase("plates");
    }

    if(!path.ends_with(joda::fs::EXT_PROJECT_TEMPLATE)) {
      path += joda::fs::EXT_PROJECT_TEMPLATE;
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

/// \todo How to check incomplete settings
///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
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

///
/// \brief      Generate results table settings based on template
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto Settings::toResultsSettings(const ResultSettingsInput &settingsIn) -> ResultsSettings
{
  std::map<enums::ClassId, std::string> classNames;
  for(const auto &classs : settingsIn.classes) {
    classNames.emplace(classs.classId, classs.name);
  }

  std::map<enums::ClassId, joda::settings::Class> classes;
  for(const auto &cl : settingsIn.classes) {
    classes.emplace(cl.classId, cl);
  }

  ResultsSettings settings;
  uint32_t colIdx = 0;
  for(const auto &entry : settingsIn.classes) {
    auto addColumn = [&](enums::ClassId classId, enums::Measurement measureChannel, enums::Stats stat, int32_t crossChannel,
                         enums::ClassId intersecting, const std::string &channelName = "") {
      if(!classes.contains(classId)) {
        joda::log::logError("Class name for ID >" + std::to_string(static_cast<int32_t>(classId)) +
                            "<not found during table generation from template!");
        return;
      }
      std::string intersectingName;
      if(intersecting != enums::ClassId::UNDEFINED) {
        if(!classes.contains(intersecting)) {
          joda::log::logError("Intersecting class name for ID >" + std::to_string(static_cast<int32_t>(intersecting)) +
                              "<not found during table generation from template!");
          return;
        }
        intersectingName = classes.at(intersecting).name;
      }

      settings.addColumn(
          ResultsSettings::ColumnIdx{.colIdx = colIdx},
          ResultsSettings::ColumnKey{.classId             = classId,
                                     .measureChannel      = measureChannel,
                                     .stats               = stat,
                                     .crossChannelStacksC = crossChannel,
                                     .intersectingChannel = intersecting,
                                     .zStack              = 0},
          ResultsSettings::ColumnName{.crossChannelName = channelName, .className = classes.at(classId).name, .intersectingName = intersectingName});
      colIdx++;
    };
    for(const auto &measure : entry.defaultMeasurements) {
      //
      // For count only sum makes sense
      //
      if(measure.measureChannel == enums::Measurement::COUNT) {
        addColumn(entry.classId, measure.measureChannel, enums::Stats::SUM, -1, enums::ClassId::UNDEFINED);
        continue;
      }

      //
      // For object IDs stats does not make sense
      //
      if(measure.measureChannel == enums::Measurement::OBJECT_ID || measure.measureChannel == enums::Measurement::ORIGIN_OBJECT_ID ||
         measure.measureChannel == enums::Measurement::PARENT_OBJECT_ID || measure.measureChannel == enums::Measurement::TRACKING_ID) {
        addColumn(entry.classId, measure.measureChannel, enums::Stats::OFF, -1, enums::ClassId::UNDEFINED);
        continue;
      }

      //
      // For coordinates stats does not make sense
      //
      if(measure.measureChannel == enums::Measurement::CENTEROID_X || measure.measureChannel == enums::Measurement::CENTEROID_Y ||
         measure.measureChannel == enums::Measurement::BOUNDING_BOX_WIDTH || measure.measureChannel == enums::Measurement::BOUNDING_BOX_HEIGHT) {
        addColumn(entry.classId, measure.measureChannel, enums::Stats::OFF, -1, enums::ClassId::UNDEFINED);
        continue;
      }

      for(auto stats : measure.stats) {
        if(measure.measureChannel == enums::Measurement::CONFIDENCE || measure.measureChannel == enums::Measurement::AREA_SIZE ||
           measure.measureChannel == enums::Measurement::PERIMETER || measure.measureChannel == enums::Measurement::CIRCULARITY) {
          addColumn(entry.classId, measure.measureChannel, stats, -1, enums::ClassId::UNDEFINED);
          continue;
        }
      }
    }

    for(const auto &classId : settingsIn.outputClasses) {
      if(classId == enums::ClassId::UNDEFINED || classId == enums::ClassId::NONE) {
        continue;
      }
      for(const auto &measure : entry.defaultMeasurements) {
        for(auto stats : measure.stats) {
          //
          // For intersecting only AVG and SUM makes sense
          //
          if(measure.measureChannel == enums::Measurement::INTERSECTING) {
            // Add only those intersecting classes which appear in the pipeline
            if(entry.classId == classId && settingsIn.intersectingClasses.contains(classId)) {
              for(const auto &intersectingClassId : settingsIn.intersectingClasses.at(classId)) {
                addColumn(classId, measure.measureChannel, stats, -1, intersectingClassId);
              }
            }
            continue;
          }

          //
          // Intensity
          //
          if(measure.measureChannel == enums::Measurement::INTENSITY_SUM || measure.measureChannel == enums::Measurement::INTENSITY_AVG ||
             measure.measureChannel == enums::Measurement::INTENSITY_MIN || measure.measureChannel == enums::Measurement::INTENSITY_MAX) {
            // Iterate over cross channels
            if(entry.classId == classId && settingsIn.measuredChannels.contains(classId)) {
              for(const auto &channelId : settingsIn.measuredChannels.at(classId)) {
                addColumn(classId, measure.measureChannel, stats, channelId, enums::ClassId::UNDEFINED, "CH " + std::to_string(channelId));
              }
            }
            continue;
          }

          //
          // Distance
          //
          if(measure.measureChannel == enums::Measurement::DISTANCE_CENTER_TO_CENTER ||
             measure.measureChannel == enums::Measurement::DISTANCE_CENTER_TO_SURFACE_MAX ||
             measure.measureChannel == enums::Measurement::DISTANCE_CENTER_TO_SURFACE_MIN ||
             measure.measureChannel == enums::Measurement::DISTANCE_SURFACE_TO_SURFACE_MAX ||
             measure.measureChannel == enums::Measurement::DISTANCE_SURFACE_TO_SURFACE_MIN ||
             measure.measureChannel == enums::Measurement::DISTANCE_FROM_OBJECT_ID ||
             measure.measureChannel == enums::Measurement::DISTANCE_TO_OBJECT_ID) {
            // List of distance from classes which contains the distance to classes
            if(entry.classId == classId && settingsIn.distanceFromClasses.contains(classId)) {
              for(const auto &intersectingClassId : settingsIn.distanceFromClasses.at(classId)) {
                if(measure.measureChannel == enums::Measurement::DISTANCE_FROM_OBJECT_ID ||
                   measure.measureChannel == enums::Measurement::DISTANCE_TO_OBJECT_ID) {
                  addColumn(classId, measure.measureChannel, enums::Stats::OFF, -1, intersectingClassId);
                } else {
                  addColumn(classId, measure.measureChannel, stats, -1, intersectingClassId);
                }
              }
            }
            continue;
          }
        }
      }
    }
  }
  settings.sortColumns();
  return settings;
}

}    // namespace joda::settings
