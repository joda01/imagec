#include "analze_settings.hpp"
#include "backend/enums/enums_classes.hpp"

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

}    // namespace joda::settings
