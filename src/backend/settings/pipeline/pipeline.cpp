///
/// \file      pipeline.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-30
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "pipeline.hpp"
#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include "backend/enums/enum_memory_idx.hpp"
#include "backend/enums/enums_classes.hpp"
#include "pipeline_factory.hpp"

namespace joda::settings {

static const int32_t MAX_HISTORY_STEPS = 64;

void Pipeline::triggerHistoryChanged() const
{
  for(const auto &f : mHistoryChangeCallback) {
    f();
  }
}

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
    int32_t n = 1;
    // Find the oldest element which is not a tag
    {
      const auto &element = history.at(history.size() - n);
      while(!element.tagMessage.empty() && ((history.size() - n) > 0)) {
        n++;
      }
    }
    const auto &elementToDelete = history.at(history.size() - n);
    if(elementToDelete.tagMessage.empty()) {
      history.erase(history.begin() + (history.size() - n));
    }
  }
  actHistoryIndex = 0;
  triggerHistoryChanged();
  return entry;
}
///
/// \brief      Clear all elements except tags from the history
/// \author     Joachim Danmayr
///
void Pipeline::clearHistory()
{
  actHistoryIndex = 0;
  history.erase(std::remove_if(history.begin(), history.end(), [](const PipelineHistoryEntry &item) { return item.tagMessage.empty(); }),
                history.end());
  if(history.empty()) {
    history.emplace_back(PipelineHistoryEntry{.commitMessage = "Created"});
  }
  triggerHistoryChanged();
}

///
/// \brief      Clear all elements except tags from the history
/// \author     Joachim Danmayr
///
void Pipeline::eraseHistory()
{
  actHistoryIndex = 0;
  history.clear();
  triggerHistoryChanged();
}

///
/// \brief      Restore a snap shot
/// \author     Joachim Danmayr
///
auto Pipeline::restoreSnapShot(size_t idx) const -> Pipeline
{
  if(history.size() < idx) {
    throw std::runtime_error("This history entry does not exist!");
  }

  Pipeline pip = *this;
  pip.pipelineSteps.clear();
  pip.pipelineSteps   = history.at(idx).pipelineSteps;
  pip.actHistoryIndex = static_cast<int32_t>(idx);
  triggerHistoryChanged();
  return pip;
}

///
/// \brief      Restore a snap shot
/// \author     Joachim Danmayr
///
auto Pipeline::undo() const -> Pipeline
{
  return restoreSnapShot(static_cast<size_t>(actHistoryIndex) + 1);
}

///
/// \brief      Tagname
/// \author     Joachim Danmayr
///
void Pipeline::tag(const std::string &tagName, size_t indexIn)
{
  if(history.size() < indexIn) {
    throw std::runtime_error("This history entry does not exist!");
  }
  history.at(indexIn).tagMessage = tagName;
  triggerHistoryChanged();
}

///
/// \brief      Returns the input classes this pipeline is using
/// \author     Joachim Danmayr
///
ObjectInputClassesExp Pipeline::getInputClasses() const
{
  ObjectInputClassesExp classes;
  for(const auto &pipelineStep : pipelineSteps) {
    if(pipelineStep.disabled) {
      continue;
    }
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
    if(pipelineStep.disabled) {
      continue;
    }
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
    if(pipelineStep.disabled) {
      continue;
    }
    auto command = PipelineFactory<joda::cmd::Command>::generate(pipelineStep);
    if(command == nullptr) {
      continue;
    }
    const auto &mems = command->getInputImageCache();
    for(const auto &tmp : mems) {
      if(tmp == enums::MemoryIdx::NONE) {
        continue;
      }
      caches.emplace(tmp);
    }
  }
  return caches;
}

[[nodiscard]] std::set<enums::MemoryIdx> Pipeline::getOutputImageCache() const
{
  std::set<enums::MemoryIdx> caches;
  for(const auto &pipelineStep : pipelineSteps) {
    if(pipelineStep.disabled) {
      continue;
    }
    auto command = PipelineFactory<joda::cmd::Command>::generate(pipelineStep);
    if(command == nullptr) {
      continue;
    }
    const auto &mems = command->getOutputImageCache();
    for(const auto &tmp : mems) {
      if(tmp == enums::MemoryIdx::NONE) {
        continue;
      }
      caches.emplace(tmp);
    }
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

    if(command.$measureIntensity.has_value()) {
      hasMeasure = true;
    }
  }

  CHECK_INFO(hasSaveImage, "Pipeline does not store control image!");
  CHECK_WARNING(hasMeasure, "Pipeline does not measure anything!");
}

}    // namespace joda::settings
