#include "analze_settings.hpp"

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
std::set<ClassificatorSettingOut> AnalyzeSettings::getOutputClasses()
{
  std::set<ClassificatorSettingOut> out;
  for(const auto &pipeline : pipelines) {
    auto cluster = pipeline.getOutputClasses();
    for(const auto &outClassesOfPipeline : cluster) {
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
std::set<ClassificatorSettingOut> AnalyzeSettings::getInputClasses()
{
  std::set<ClassificatorSettingOut> out;
  for(const auto &pipeline : pipelines) {
    auto cluster = pipeline.getInputClusters();
    for(const auto &outClassesOfPipeline : cluster) {
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
auto AnalyzeSettings::checkForErrors() -> std::vector<std::pair<std::string, SettingParserLog_t>>
{
  std::vector<std::pair<std::string, SettingParserLog_t>> errorOrderedByPipeline;

  {
    SettingParserLog_t errors;
    projectSettings.getErrorLogRecursive(errors);
    errorOrderedByPipeline.emplace_back("Project settings", errors);
  }

  {
    SettingParserLog_t errors;
    projectSettings.getErrorLogRecursive(errors);
    errorOrderedByPipeline.emplace_back("Image setup", errors);
  }

  // Check for unused output classes
  {
    auto outputClasses = getOutputClasses();
    auto inputClasses  = getInputClasses();
    for(const auto &outputClass : outputClasses) {
      if(!inputClasses.contains(outputClass)) {
        // This class is not used
        CHECK_INFO(false, "Output class >" + std::to_string((uint16_t) outputClass.clusterId) + "/" +
                              std::to_string((uint16_t) outputClass.classId) + "< has no input!");
      }
    }
    errorOrderedByPipeline.emplace_back("Image setup", joda_settings_log);
  }

  // Check the pipelines
  for(const auto &pipeline : pipelines) {
    SettingParserLog_t errors;
    pipeline.getErrorLogRecursive(errors);
    errorOrderedByPipeline.emplace_back("Pipeline: " + pipeline.meta.name, errors);
  }
  return errorOrderedByPipeline;
}

}    // namespace joda::settings
