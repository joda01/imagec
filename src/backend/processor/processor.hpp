
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
#pragma once

#include <filesystem>
#include <mutex>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/file_grouper/file_grouper.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/helper/threading/threading.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::processor {

using imagesList_t = joda::filesystem::DirectoryWatcher;

struct PreviewSettings
{
  settings::ImageSaverSettings::Style style = settings::ImageSaverSettings::Style::OUTLINED;
};

struct PreviewReturn
{
  int32_t count = 0;
  std::string color;
  std::string wantedColor;
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

struct ProcessInformation
{
  std::filesystem::path ouputFolder;
  std::filesystem::path resultsFilePath;
  std::string jobName;
  std::chrono::system_clock::time_point timestampStarted;
  std::string errorLog;
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

  void setRunningPreparingPipeline()
  {
    state = ProcessState::RUNNING_PREPARING_PIPELINE;
  }

  void setStateError(ProcessInformation &info, const std::string &errorMsg)
  {
    state         = ProcessState::FINISHED_WITH_ERROR;
    info.errorLog = errorMsg;
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
  void execute(const joda::settings::AnalyzeSettings &program, const std::string &jobName, const joda::thread::ThreadingSettings &threadingSettings,
               imagesList_t &allImages);
  void stop();
  std::string initializeGlobalContext(const joda::settings::AnalyzeSettings &program, const std::string &jobName, GlobalContext &globalContext);

  void listImages(const joda::settings::AnalyzeSettings &program, imagesList_t &);
  const ProcessProgress &getProgress() const
  {
    return mProgress;
  }

  const ProcessInformation &getJobInformation() const
  {
    return mJobInformation;
  }

  auto generatePreview(const PreviewSettings &previewSettings, const settings::ProjectImageSetup &imageSetup,
                       const settings::AnalyzeSettings &settings, const settings::Pipeline &pipeline, const std::filesystem::path &imagePath,
                       int32_t tStack, int32_t zStack, int32_t tileX, int32_t tileY, bool generateThumb, const ome::OmeInfo &ome,
                       const settings::ObjectInputClusters &clustersClassesToShow)
      -> std::tuple<cv::Mat, cv::Mat, cv::Mat, std::map<settings::ClassificatorSetting, PreviewReturn>>;

private:
  /////////////////////////////////////////////////////
  ProcessProgress mProgress = {};
  ProcessInformation mJobInformation;
};
}    // namespace joda::processor
