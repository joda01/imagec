
///
/// \file      processor.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include <filesystem>
#include <mutex>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/file_grouper/file_grouper.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::processor {

using imagesList_t = joda::filesystem::DirectoryWatcher;

enum class ProcessState
{
  INITIALIZING,
  LOOKING_FOR_IMAGES,
  RUNNING,
  STOPPING,
  FINISHED
};

struct ProcessInformation
{
  std::filesystem::path ouputFolder;
  std::filesystem::path resultsFilePath;
  std::string jobName;
  std::chrono::system_clock::time_point timestamp;
};

class ProcessProgress
{
public:
  void reset()
  {
    state               = ProcessState::INITIALIZING;
    totalNrOfImages     = 0;
    processedNrOfImages = 0;
    totalNrOfTiles      = 0;
    processedNrOfTiles  = 0;
  }

  void incProcessedImages()
  {
    processedNrOfImages++;
  }
  void incProcessedTiles()
  {
    processedNrOfTiles++;
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

  void setTotalNrOfImages(uint32_t images)
  {
    totalNrOfImages = images;
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

  bool isStopping() const
  {
    return state == ProcessState::STOPPING;
  }

  bool isFinished() const
  {
    return state == ProcessState::FINISHED;
  }

private:
  std::atomic<ProcessState> state           = ProcessState::INITIALIZING;
  std::atomic<uint32_t> totalNrOfImages     = 0;
  std::atomic<uint32_t> processedNrOfImages = 0;
  std::atomic<uint32_t> totalNrOfTiles      = 0;
  std::atomic<uint32_t> processedNrOfTiles  = 0;
};

class Processor
{
public:
  /////////////////////////////////////////////////////
  Processor();
  void execute(const joda::settings::AnalyzeSettings &program, imagesList_t &allImages);
  void stop();
  std::string initializeGlobalContext(const joda::settings::AnalyzeSettings &program, GlobalContext &globalContext);
  void initializePipelineContext(const joda::settings::AnalyzeSettings &program, const GlobalContext &globalContext,
                                 const PlateContext &plateContext, joda::grp::FileGrouper &grouper,
                                 const joda::filesystem::path &imagePath, PipelineInitializer &imageLoader,
                                 ImageContext &imageContext);

  void listImages(const joda::settings::AnalyzeSettings &program, imagesList_t &);
  const ProcessProgress &getProgress() const
  {
    return mProgress;
  }

  const ProcessInformation &getJobInformation() const
  {
    return mJobInformation;
  }

private:
  /////////////////////////////////////////////////////
  ProcessProgress mProgress;
  ProcessInformation mJobInformation;
};
}    // namespace joda::processor
