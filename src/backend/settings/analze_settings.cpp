#include "analze_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/processor/dependency_graph.hpp"
#include "backend/settings/settings.hpp"
#include "backend/user_settings/user_home_dir.hpp"

namespace joda::settings {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto AnalyzeSettings::getProjectPath() const -> std::filesystem::path
{
  if(projectSettings.workingDirectory.empty()) {
    return joda::user_settings::getUserHomeDir() / joda::fs::WORKING_DIRECTORY_TEMP_PROJECT_PATH;
  }

  return std::filesystem::path(projectSettings.workingDirectory) / joda::fs::WORKING_DIRECTORY_PROJECT_PATH;
}

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
  return Settings::toResultsSettings(
      Settings::ResultSettingsInput{.classes       = {projectSettings.classification.classes.begin(), projectSettings.classification.classes.end()},
                                    .outputClasses = getOutputClasses(),
                                    .intersectingClasses = getPossibleIntersectingClasses(),
                                    .measuredChannels    = getImageChannelsUsedForMeasurement(),
                                    .distanceFromClasses = getPossibleDistanceClasses()});
}

}    // namespace joda::settings
