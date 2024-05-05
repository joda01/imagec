///
/// \file      pipeline.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once
// #include <memory>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <vector>
#include "../helper/directory_iterator.hpp"
#include "../helper/helper.hpp"
#include "../image_processing/detection/detection_response.hpp"
#include "../logger/console_logger.hpp"
#include "../results/results.hpp"
#include "backend/helper/file_info_images.hpp"
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "backend/helper/thread_pool.hpp"
#include "backend/image_reader/image_reader.hpp"
#include "backend/pipelines/processor/image_processor.hpp"
#include "backend/results/results.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/channel/channel_settings.hpp"

namespace joda::pipeline {

///
/// \class      Pipeline
/// \author     Joachim Danmayr
/// \brief
///
class Pipeline
{
  friend class PipelineFactory;

public:
  /////////////////////////////////////////////////////

  struct ProgressIndicator
  {
    types::Progress total;
    types::Progress image;
  };

  struct ThreadingSettings
  {
    ThreadingSettings()
    {
    }
    enum Type
    {
      IMAGES,
      TILES,
      CHANNELS
    };
    uint64_t ramPerImage    = 0;
    uint64_t ramFree        = 0;
    uint64_t ramTotal       = 0;
    uint32_t coresAvailable = 0;
    uint32_t coresUsed      = 0;
    uint64_t totalRuns      = 0;
    std::map<Type, int32_t> cores{{IMAGES, 1}, {TILES, 1}, {CHANNELS, 1}};
  };

  enum class State : int
  {
    STOPPED  = 0,
    RUNNING  = 1,
    PAUSED   = 2,
    STOPPING = 3,
    FINISHED = 4,
    ERROR_   = 5
  };

  Pipeline(const joda::settings::AnalyzeSettings &, joda::helper::DirectoryWatcher<FileInfoImages> *imageFileContainer,
           const std::string &inputFolder, const std::string &jobName,
           const ThreadingSettings &threadingSettings = ThreadingSettings());
  ~Pipeline()
  {
    stopJob();
    if(mMainThread != nullptr && mMainThread->joinable()) {
      mMainThread->join();
    }
  }
  const std::string &getLastErrorMessage()
  {
    return mLastErrorMessage;
  }

  [[nodiscard]] const std::string &getOutputFolder() const
  {
    return mOutputFolder;
  }

protected:
  /////////////////////////////////////////////////////
  [[noreturn]] void setStateError(const std::string &what) noexcept(false)
  {
    joda::log::logError(what);
    mState            = State::ERROR_;
    mLastErrorMessage = what;
    throw std::runtime_error(what);
  }

  void stopWithError(const std::string &what)
  {
    joda::log::logError(what);
    mState            = State::ERROR_;
    mLastErrorMessage = what;
    mStop             = true;
  }

  ///
  /// \brief Returns the analyze settings of this pipeline
  [[nodiscard]] auto getAnalyzeSetings() const -> const joda::settings::AnalyzeSettings &
  {
    return mAnalyzeSettings;
  }

  auto getStopReference() -> bool &
  {
    return mStop;
  }

private:
  static const int THREAD_POOL_BUFFER = 25;

  /////////////////////////////////////////////////////
  void runJob();

  ///
  /// \brief Stop a running job
  void stopJob();

  ///
  /// \brief Total progress and state of the analysis
  [[nodiscard]] auto getState() const -> std::tuple<ProgressIndicator, State, std::string>
  {
    return {mProgress, mState, mLastErrorMessage};
  }

private:
  /////////////////////////////////////////////////////
  static inline const std::string OUTPUT_FOLDER_PATH{"imagec"};

  /////////////////////////////////////////////////////
  auto prepareOutputFolder(const std::string &inputFolder, const std::string &jobName) -> std::string;
  ///
  /// \brief Returns if the thread should be stopped
  [[nodiscard]] auto shouldThreadBeStopped() const -> bool
  {
    return mStop;
  }

  void analyzeImage(joda::results::WorkSheet &alloverReport, const FileInfoImages &imagePath);

  void analyzeTile(joda::results::WorkSheet &detailReports, FileInfoImages imagePath, std::string detailOutputFolder,
                   int tileIdx, const joda::algo::ChannelProperties &channelProperties);
  void analyszeChannel(std::map<joda::settings::ChannelIndex, joda::func::DetectionResponse> &detectionResults,
                       const joda::settings::ChannelSettings &channelSettings, FileInfoImages imagePath, int tileIdx,
                       const joda::algo::ChannelProperties &channelProperties);

  /////////////////////////////////////////////////////
  std::string mInputFolder;
  std::string mOutputFolder;
  bool mStop = false;
  joda::settings::AnalyzeSettings mAnalyzeSettings;
  std::vector<const joda::settings::ChannelSettings *> mListOfChannelSettings;
  std::vector<const joda::settings::VChannelSettings *> mListOfVChannelSettings;

  joda::helper::DirectoryWatcher<FileInfoImages> *mImageFileContainer;

  ProgressIndicator mProgress;
  State mState;
  std::string mLastErrorMessage;
  std::shared_ptr<std::thread> mMainThread = nullptr;
  ThreadingSettings mThreadingSettings;
  std::mutex mAddToDetailReportMutex;
  std::map<std::string, joda::onnx::OnnxParser::Data> mOnnxModels;
  std::string mJobName;
  std::chrono::system_clock::time_point mTimePipelineStarted;
  joda::results::ExperimentMeta mExperimentMeta;

  /////////////////////////////////////////////////////
  int32_t mWellSizeX = 0;
  int32_t mWellSizeY = 0;
  std::map<int32_t, results::ImgPositionInWell> mTransformedWellMatrix;
};

}    // namespace joda::pipeline
