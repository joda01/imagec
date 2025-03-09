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
#include <optional>
#include <stdexcept>
#include <string>
#include "backend/enums/enums_classes.hpp"
#include "pipeline_factory.hpp"

namespace joda::settings {

static const int32_t MAX_HISTORY_STEPS = 64;

///
/// \brief      Create a snapshot of the actual pipeline steps
/// \author     Joachim Danmayr
///
auto Pipeline::createSnapShot(enums::HistoryCategory category, const std::string &note) -> std::optional<PipelineHistoryEntry>
{
  if(pipelineSteps.empty() && history.empty()) {
    return std::nullopt;
  }
  if(!history.empty()) {
    nlohmann::json act  = pipelineSteps;
    nlohmann::json last = history.at(0).pipelineSteps;
    if(enums::HistoryCategory::SAVED != category && act.dump() == last.dump()) {
      return std::nullopt;
    }
  }
  auto entry = PipelineHistoryEntry{
      category, this->pipelineSteps, note, "",
      static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count())};
  history.emplace(history.begin(), entry);

  // Limit to max. history steps
  if(history.size() > MAX_HISTORY_STEPS) {
    history.pop_back();
  }
  return entry;
}

///
/// \brief      Restore a snap shot
/// \author     Joachim Danmayr
///
auto Pipeline::restoreSnapShot(int32_t idx) const -> Pipeline
{
  if(history.size() < idx) {
    throw std::runtime_error("This history entry does not exist!");
  }

  Pipeline pip = *this;
  pip.pipelineSteps.clear();
  pip.pipelineSteps = history.at(idx).pipelineSteps;

  return pip;
}

///
/// \brief      Tagname
/// \author     Joachim Danmayr
///
void Pipeline::tag(const std::string &tagName, int32_t index)
{
  if(history.size() < index) {
    throw std::runtime_error("This history entry does not exist!");
  }
  history.at(index).tagMessage = tagName;
}

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
