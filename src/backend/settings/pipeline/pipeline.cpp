///
/// \file      pipeline.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-30
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "pipeline.hpp"
#include <memory>
#include <string>
#include "backend/enums/enums_classes.hpp"
#include "pipeline_factory.hpp"

namespace joda::settings {

///
/// \brief      Returns the input classes this pipeline is using
/// \author     Joachim Danmayr
///
ObjectInputClassesExp Pipeline::getInputClasses() const
{
  ObjectInputClassesExp classes;
  for(const auto &pipelineStep : pipelineSteps) {
    auto command = PipelineFactory<joda::cmd::Command>::generate(pipelineStep);
    if(command == nullptr) {
      /// \todo Log warning
      continue;
    }
    const auto &classesCmd = command->getInputClasses();
    for(const auto &classId : classesCmd) {
      auto classIdToSet = static_cast<joda::enums::ClassId>(classId);

      if(classId >= enums::ClassIdIn::TEMP_01 && classId <= enums::ClassIdIn::TEMP_LAST) {
        continue;
      }

      if(classId == enums::ClassIdIn::$) {
        classIdToSet = pipelineSetup.defaultClassId;
      }

      classes.emplace(classIdToSet);
    }
  }
  return classes;
}

///
/// \brief      Returns the classs ID this pipeline is storing the results in
/// \author     Joachim Danmayr
///
ObjectOutputClassesExp Pipeline::getOutputClasses() const
{
  ObjectOutputClassesExp classes;
  for(const auto &pipelineStep : pipelineSteps) {
    auto command = PipelineFactory<joda::cmd::Command>::generate(pipelineStep);
    if(command == nullptr) {
      continue;
    }
    const auto &classesCmd = command->getOutputClasses();
    for(const auto &classs : classesCmd) {
      auto classIdToSet = static_cast<joda::enums::ClassId>(classs);
      if(classs >= enums::ClassIdIn::TEMP_01 && classs <= enums::ClassIdIn::TEMP_LAST) {
        continue;
      }

      if(classs == enums::ClassIdIn::$) {
        classIdToSet = pipelineSetup.defaultClassId;
      }

      classes.emplace(classIdToSet);
    }
  }
  return classes;
}

[[nodiscard]] std::set<enums::MemoryIdx> Pipeline::getInputImageCache() const
{
  std::set<enums::MemoryIdx> caches;
  for(const auto &pipelineStep : pipelineSteps) {
    auto command = PipelineFactory<joda::cmd::Command>::generate(pipelineStep);
    if(command == nullptr) {
      continue;
    }
    const auto &mems = command->getInputImageCache();
    caches.insert(mems.begin(), mems.end());
  }
  return caches;
}

[[nodiscard]] std::set<enums::MemoryIdx> Pipeline::getOutputImageCache() const
{
  std::set<enums::MemoryIdx> caches;
  for(const auto &pipelineStep : pipelineSteps) {
    auto command = PipelineFactory<joda::cmd::Command>::generate(pipelineStep);
    if(command == nullptr) {
      continue;
    }
    const auto &mems = command->getOutputImageCache();
    caches.insert(mems.begin(), mems.end());
  }
  return caches;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Pipeline::check() const
{
  bool hasSaveImage = false;
  bool hasMeasure   = false;

  for(const auto &command : pipelineSteps) {
    if(command.$saveImage.has_value()) {
      hasSaveImage = true;
    }

    if(command.$measure.has_value()) {
      hasMeasure = true;
    }
  }

  CHECK_INFO(hasSaveImage, "Pipeline does not store control image!");
  CHECK_WARNING(hasMeasure, "Pipeline does not measure anything!");
}

}    // namespace joda::settings
