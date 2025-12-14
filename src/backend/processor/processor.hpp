
///
/// \file      processor.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///
#pragma once

#include <filesystem>
#include <memory>
#include <mutex>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/types.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/file_grouper/file_grouper.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/helper/image/image.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/processor/dependency_graph.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include <opencv2/core/mat.hpp>
#include <BS_thread_pool.hpp>

namespace joda::processor {

using imagesList_t = joda::filesystem::DirectoryWatcher;

struct PreviewSettings
{
};

struct DisplayImages
{
  joda::image::Image thumbnail;
  joda::image::Image originalImage;
  int tStacks = 1;    // Nr. of t stacks the image has.
};

struct Preview
{
  joda::image::Image editedImage;

  struct PreviewResults
  {
    std::shared_ptr<joda::atom::ObjectList> objectMap = std::make_shared<joda::atom::ObjectList>();
    bool isOverExposed                                = false;
    bool noiseDetected                                = false;
  } results;

  int height;
  int width;
  int tStacks = 1;    // Nr. of t stacks the image has.
  std::string imageFileName;
};

enum class ProcessState
{
  INITIALIZING,
  LOOKING_FOR_IMAGES,
  RUNNING_PREPARING_PIPELINE,
  RUNNING,
  STOPPING,
  FINISHED,
  FINISHED_WITH_ERROR
};

class ProcessProgress
{
public:
  void reset()
  {
    state                  = ProcessState::INITIALIZING;
    totalNrOfImages        = 0;
    processedNrOfImages    = 0;
    totalNrOfTiles         = 0;
    processedNrOfTiles     = 0;
    processedPipelineSteps = 0;
    mWhat.clear();
  }

  void incProcessedImages()
  {
    processedNrOfImages++;
  }
  void incProcessedTiles()
  {
    processedNrOfTiles++;
  }
  void incProcessedPipelineSteps()
  {
    processedPipelineSteps++;
  }

  void setStateFinished()
  {
    state = ProcessState::FINISHED;
  }

  void setStateStopping()
  {
    state = ProcessState::STOPPING;
  }

  void setStateLookingForImages()
  {
    state = ProcessState::LOOKING_FOR_IMAGES;
  }

  void setStateRunning()
  {
    state = ProcessState::RUNNING;
  }

  void setRunningPreparingPipeline()
  {
    state = ProcessState::RUNNING_PREPARING_PIPELINE;
  }

  void setStateError(const std::string &errorMsg)
  {
    state = ProcessState::FINISHED_WITH_ERROR;
    mWhat = errorMsg;
  }

  void setTotalNrOfImages(uint32_t images)
  {
    totalNrOfImages = images;
  }

  void setTotalNrOfTiles(uint32_t tiles)
  {
    totalNrOfTiles = tiles;
  }

  [[nodiscard]] ProcessState getState() const
  {
    return state;
  }

  uint32_t totalImages() const
  {
    return totalNrOfImages;
  }

  uint32_t finishedImages() const
  {
    return processedNrOfImages;
  }

  uint32_t totalTiles() const
  {
    return totalNrOfTiles;
  }

  uint32_t finishedTiles() const
  {
    return processedNrOfTiles;
  }

  uint32_t finishedPipelineSteps() const
  {
    return processedPipelineSteps;
  }

  bool isStopping() const
  {
    return state == ProcessState::STOPPING;
  }

  bool isFinished() const
  {
    return state == ProcessState::FINISHED;
  }

  auto &what() const
  {
    return mWhat;
  }

private:
  std::atomic<ProcessState> state              = ProcessState::INITIALIZING;
  std::atomic<uint32_t> totalNrOfImages        = 0;
  std::atomic<uint32_t> processedNrOfImages    = 0;
  std::atomic<uint32_t> totalNrOfTiles         = 0;
  std::atomic<uint32_t> processedNrOfTiles     = 0;
  std::atomic<uint32_t> processedPipelineSteps = 0;

  std::string mWhat;
};

class Processor
{
public:
  /////////////////////////////////////////////////////
  Processor();
  void stop();

  void execute(std::unique_ptr<BS::thread_pool<>> &threadPool, const joda::settings::AnalyzeSettings &program, const std::string &jobName,
               const std::unique_ptr<imagesList_t> &imagesToAnalyze);

  auto generatePreview(std::unique_ptr<BS::thread_pool<>> &threadPool, const PreviewSettings &previewSettings,
                       const settings::ProjectImageSetup &imageSetup, const settings::AnalyzeSettings &settings, const settings::Pipeline &pipeline,
                       const std::filesystem::path &imagePath, int32_t tStack, int32_t zStack, int32_t tileX, int32_t tileY, const ome::OmeInfo &ome,
                       Preview &previewOut) -> void;

  const ProcessProgress &getProgress() const
  {
    return mProgress;
  }
  const std::unique_ptr<GlobalContext> &getGlobalContext() const
  {
    return mGlobalContext;
  }
  ProcessProgress &mutableProgress()
  {
    return mProgress;
  }

  //  const ProcessInformation &getJobInformation() const
  //  {
  //    return mJobInformation;
  //  }

private:
  /////////////////////////////////////////////////////
  template <class DATABASE_TYPE>
  std::unique_ptr<GlobalContext> initializeGlobalContext(const joda::settings::AnalyzeSettings &program, const std::string &jobName);
  void prepareOutputFolder(const joda::settings::AnalyzeSettings &program, const std::unique_ptr<GlobalContext> &globalContext) const;

  /////////////////////////////////////////////////////
  ProcessProgress mProgress = {};
  std::unique_ptr<GlobalContext> mGlobalContext;
  std::atomic<bool> mCancelAll{false};
};
}    // namespace joda::processor
