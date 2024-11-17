#include "analze_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"

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
std::set<ClassificatorSettingOut> AnalyzeSettings::getOutputClustersAndClasses() const
{
  std::set<ClassificatorSettingOut> out;
  for(const auto &pipeline : pipelines) {
    auto cluster = pipeline.getOutputClustersAndClasses();
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
std::set<ClassificatorSettingOut> AnalyzeSettings::getInputClasses() const
{
  std::set<ClassificatorSettingOut> out;
  for(const auto &pipeline : pipelines) {
    auto cluster = pipeline.getInputClustersAndClasses();
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

  std::map<enums::ClusterId, std::string> clusters;
  for(const auto &cluster : projectSettings.classification.clusters) {
    clusters.emplace(cluster.clusterId, cluster.name);
  }

  clusters.emplace(enums::ClusterId::NONE, "None");

  // Check for unused output classes
  {
    auto outputClasses = getOutputClustersAndClasses();
    auto inputClasses  = getInputClasses();
    for(const auto &outputClass : outputClasses) {
      if(!inputClasses.contains(outputClass)) {
        // This class is not used
        CHECK_WARNING(false, "Output cluster/class >" + clusters[outputClass.clusterId] + "/" + classes[outputClass.classId] + "< is unused!");
      }
    }
    errorOrderedByPipeline.emplace_back("Image setup", joda_settings_log);
  }

  // Check the pipelines
  for(const auto &pipeline : pipelines) {
    SettingParserLog_t errors;
    pipeline.getErrorLogRecursive(errors);
    errorOrderedByPipeline.emplace_back(pipeline.meta.name, errors);
  }
  return errorOrderedByPipeline;
}

}    // namespace joda::settings
