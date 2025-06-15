#include "analze_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/processor/dependency_graph.hpp"

namespace joda::settings {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void AnalyzeSettings::check() const
{
  if(imageSetup.tStackHandling == ProjectImageSetup::TStackHandling::EXACT_ONE) {
    for(const auto &pip : pipelines) {
      CHECK_ERROR(pip.pipelineSetup.tStackIndex >= 0, "When processing exact one t stack image, define which one!");
    }
  }

  if(imageSetup.zStackHandling == ProjectImageSetup::ZStackHandling::EXACT_ONE) {
    for(const auto &pip : pipelines) {
      CHECK_ERROR(pip.pipelineSetup.zStackIndex >= 0, "When processing exact one z stack image, define which one!");
    }
  }
  CHECK_INFO(true, "Okay");
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::set<joda::enums::ClassId> AnalyzeSettings::getOutputClasses() const
{
  std::set<joda::enums::ClassId> out;
  for(const auto &pipeline : pipelines) {
    auto classs = pipeline.getOutputClasses();
    for(const auto &outClassesOfPipeline : classs) {
      out.emplace(outClassesOfPipeline);
    }
  }

  return out;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::set<joda::enums::ClassId> AnalyzeSettings::getInputClasses() const
{
  std::set<joda::enums::ClassId> out;
  for(const auto &pipeline : pipelines) {
    auto classs = pipeline.getInputClasses();
    for(const auto &outClassesOfPipeline : classs) {
      out.emplace(outClassesOfPipeline);
    }
  }
  return out;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto AnalyzeSettings::checkForErrors() const -> std::vector<std::pair<std::string, SettingParserLog_t>>
{
  joda_settings_log.clear();
  std::vector<std::pair<std::string, SettingParserLog_t>> errorOrderedByPipeline;

  {
    SettingParserLog_t errors;
    projectSettings.getErrorLogRecursive(errors);
    errorOrderedByPipeline.emplace_back("Project settings", errors);
  }

  {
    SettingParserLog_t errors;
    imageSetup.getErrorLogRecursive(errors);
    errorOrderedByPipeline.emplace_back("Image setup", errors);
  }

  std::map<enums::ClassId, std::string> classes;
  for(const auto &classs : projectSettings.classification.classes) {
    classes.emplace(classs.classId, classs.name);
  }
  classes.emplace(enums::ClassId::NONE, "None");

  // Check for unused output classes
  {
    auto outputClasses = getOutputClasses();
    auto inputClasses  = getInputClasses();
    for(const auto &outputClass : outputClasses) {
      if(!inputClasses.contains(outputClass)) {
        // This class is not used
        CHECK_WARNING(false, "Output classs/class >" + classes[outputClass] + "< is unused!");
      }
    }
    errorOrderedByPipeline.emplace_back("Image setup", joda_settings_log);
  }

  // Check the pipeline graph for errors
  {
    SettingParserLog_t depGraphLog;
    joda::processor::DependencyGraph::calcGraph(*this, nullptr, &depGraphLog);
    errorOrderedByPipeline.emplace_back("Dependency graph", depGraphLog);
  }

  // Check the pipelines
  {
    for(const auto &pipeline : pipelines) {
      SettingParserLog_t errors;
      pipeline.getErrorLogRecursive(errors);
      errorOrderedByPipeline.emplace_back(pipeline.meta.name, errors);
    }
  }

  return errorOrderedByPipeline;
}

///
/// \brief      Returns classes which may intersect with an other class
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto AnalyzeSettings::getPossibleIntersectingClasses() const -> std::map<enums::ClassId, std::set<enums::ClassId>>
{
  std::map<enums::ClassId, std::set<enums::ClassId>> possibleIntersection;
  for(const auto &pipeline : pipelines) {
    for(const auto &step : pipeline.pipelineSteps) {
      if(step.$reclassify.has_value()) {
        // step.$reclassify.value().inputClasses;
        auto baseClassIds = step.$reclassify.value().intersection.inputClassesIntersectWith;
        for(const auto baseClassId : baseClassIds) {
          enums::ClassId classId;
          if(baseClassId == enums::ClassIdIn::$) {
            classId = pipeline.pipelineSetup.defaultClassId;
          } else {
            classId = static_cast<enums::ClassId>(baseClassId);
          }
          auto newClassIdTmp = step.$reclassify.value().newClassId;
          enums::ClassId newClassId;
          if(newClassIdTmp == enums::ClassIdIn::$) {
            newClassId = pipeline.pipelineSetup.defaultClassId;
          } else {
            newClassId = static_cast<enums::ClassId>(newClassIdTmp);
          }
          possibleIntersection[classId].emplace(newClassId);
        }
      }
    }
  }
  return possibleIntersection;
}

///
/// \brief      Returns classes which may intersect with an other class
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto AnalyzeSettings::getPossibleDistanceClasses() const -> std::map<enums::ClassId, std::set<enums::ClassId>>
{
  std::map<enums::ClassId, std::set<enums::ClassId>> possibleDistances;
  for(const auto &pipeline : pipelines) {
    for(const auto &step : pipeline.pipelineSteps) {
      if(step.$measureDistance.has_value()) {
        auto baseClassId = step.$measureDistance.value().inputClassFrom;
        enums::ClassId classId;
        if(baseClassId == enums::ClassIdIn::$) {
          classId = pipeline.pipelineSetup.defaultClassId;
        } else {
          classId = static_cast<enums::ClassId>(baseClassId);
        }
        auto newClassIdTmp = step.$measureDistance.value().inputClassTo;
        enums::ClassId newClassId;
        if(newClassIdTmp == enums::ClassIdIn::$) {
          newClassId = pipeline.pipelineSetup.defaultClassId;
        } else {
          newClassId = static_cast<enums::ClassId>(newClassIdTmp);
        }
        possibleDistances[classId].emplace(newClassId);
      }
    }
  }
  return possibleDistances;
}

///
/// \brief      Returns classes which may intersect with an other class
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto AnalyzeSettings::getImageChannelsUsedForMeasurement() const -> std::map<enums::ClassId, std::set<int32_t>>
{
  std::map<enums::ClassId, std::set<int32_t>> usedImageChannels;
  for(const auto &pipeline : pipelines) {
    for(const auto &step : pipeline.pipelineSteps) {
      if(step.$measureIntensity.has_value()) {
        // step.$reclassify.value().inputClasses;
        for(const auto &imagePlane : step.$measureIntensity.value().planesIn) {
          int32_t imageChannelTmp = imagePlane.imagePlane.cStack;
          if(imageChannelTmp < 0) {
            imageChannelTmp = pipeline.pipelineSetup.cStackIndex;
          }
          for(const auto baseClassId : step.$measureIntensity.value().inputClasses) {
            enums::ClassId classId;
            if(baseClassId == enums::ClassIdIn::$) {
              classId = pipeline.pipelineSetup.defaultClassId;
            } else {
              classId = static_cast<enums::ClassId>(baseClassId);
            }
            usedImageChannels[classId].emplace(imageChannelTmp);
          }
        }
      }
    }
  }
  return usedImageChannels;
}

///
/// \brief      Generate results table settings based on template
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto AnalyzeSettings::toResultsSettings() const -> ResultsSettings
{
  std::map<enums::ClassId, std::string> classNames;
  for(const auto &classs : projectSettings.classification.classes) {
    classNames.emplace(classs.classId, classs.name);
  }

  auto outputClasses       = getOutputClasses();
  auto intersectingClasses = getPossibleIntersectingClasses();
  auto measuredChannels    = getImageChannelsUsedForMeasurement();
  auto distanceFromClasses = getPossibleDistanceClasses();

  std::map<enums::ClassId, joda::settings::Class> classes;
  for(const auto &cl : projectSettings.classification.classes) {
    classes.emplace(cl.classId, cl);
  }

  ResultsSettings settings;
  int32_t colIdx = 0;
  for(const auto &entry : projectSettings.classification.classes) {
    for(const auto &classId : outputClasses) {
      if(classId == enums::ClassId::UNDEFINED || classId == enums::ClassId::NONE) {
        continue;
      }
      for(const auto &measure : entry.defaultMeasurements) {
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

          settings.addColumn(ResultsSettings::ColumnIdx{.colIdx = colIdx},
                             ResultsSettings::ColumnKey{.classId             = classId,
                                                        .measureChannel      = measure.measureChannel,
                                                        .stats               = stat,
                                                        .crossChannelStacksC = crossChannel,
                                                        .intersectingChannel = intersecting,
                                                        .zStack              = 0},
                             ResultsSettings::ColumnName{
                                 .crossChannelName = channelName, .className = classes.at(classId).name, .intersectingName = intersectingName});
          colIdx++;
        };

        for(auto stats : measure.stats) {
          //
          // For count only sum makes sense
          //
          if(measure.measureChannel == enums::Measurement::COUNT) {
            addColumn(enums::Stats::SUM, -1, enums::ClassId::UNDEFINED);
            continue;
          }

          //
          // For object IDs stats does not make sense
          //
          if(measure.measureChannel == enums::Measurement::OBJECT_ID || measure.measureChannel == enums::Measurement::ORIGIN_OBJECT_ID ||
             measure.measureChannel == enums::Measurement::PARENT_OBJECT_ID || measure.measureChannel == enums::Measurement::TRACKING_ID) {
            addColumn(enums::Stats::OFF, -1, enums::ClassId::UNDEFINED);
            continue;
          }

          //
          // For intersecting only AVG and SUM makes sense
          //
          if(measure.measureChannel == enums::Measurement::INTERSECTING) {
            // Add only those intersecting classes which appear in the pipeline
            if(intersectingClasses.contains(classId)) {
              for(const auto &intersectingClassId : intersectingClasses.at(classId)) {
                addColumn(stats, -1, intersectingClassId);
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
            if(measuredChannels.contains(classId)) {
              for(const auto &channelId : measuredChannels.at(classId)) {
                addColumn(stats, channelId, enums::ClassId::UNDEFINED, "CH " + std::to_string(channelId));
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
            if(distanceFromClasses.contains(classId)) {
              for(const auto &intersectingClassId : distanceFromClasses.at(classId)) {
                if(measure.measureChannel == enums::Measurement::DISTANCE_FROM_OBJECT_ID ||
                   measure.measureChannel == enums::Measurement::DISTANCE_TO_OBJECT_ID) {
                  addColumn(enums::Stats::OFF, -1, intersectingClassId);
                } else {
                  addColumn(stats, -1, intersectingClassId);
                }
              }
            }
            continue;
          }

          addColumn(stats, -1, enums::ClassId::UNDEFINED);
        }
      }
    }
  }
  settings.sortColumns();
  return settings;
}

}    // namespace joda::settings
