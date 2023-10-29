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
#include <mutex>
#include <set>
#include <string>
#include <vector>
#include "../helper/directory_iterator.hpp"
#include "../helper/helper.hpp"
#include "../image_processing/functions/func_types.hpp"
#include "../reporting/reporting.h"
#include "../settings/analze_settings_parser.hpp"
#include "backend/image_reader/image_reader.hpp"

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

  Pipeline(const settings::json::AnalyzeSettings &, joda::helper::ImageFileContainer *imageFileContainer,
           const std::string &inputFolder, const ThreadingSettings &threadingSettings = ThreadingSettings());
  ~Pipeline()
  {
    stopJob();
    if(mMainThread->joinable()) {
      mMainThread->join();
    }
  }
  const std::string &getLastErrorMessage()
  {
    return mLastErrorMessage;
  }

protected:
  /////////////////////////////////////////////////////
  [[noreturn]] void setStateError(const std::string &what) noexcept(false)
  {
    mState            = State::ERROR_;
    mLastErrorMessage = what;
    throw std::runtime_error(what);
  }

  ///
  /// \brief Returns the analyze settings of this pipeline
  [[nodiscard]] auto getAnalyzeSetings() const -> const joda::settings::json::AnalyzeSettings &
  {
    return mAnalyzeSettings;
  }

  auto getStopReference() -> bool &
  {
    return mStop;
  }

private:
  enum class ColumnIndexDetailedReport : int
  {
    CONFIDENCE    = 0,
    INTENSITY     = 1,
    INTENSITY_MIN = 2,
    INTENSITY_MAX = 3,
    AREA_SIZE     = 4,
    PERIMETER     = 5,
    CIRCULARITY   = 6,
    VALIDITY      = 7
  };
  static const int NR_OF_COLUMNS_PER_CHANNEL_IN_DETAIL_REPORT = 8;
  static const int THREAD_POOL_BUFFER                         = 5;

  /////////////////////////////////////////////////////
  void runJob();

  ///
  /// \brief Stop a running job
  void stopJob()
  {
    mState = State::STOPPING;
    mStop  = true;
  }

  ///
  /// \brief Total progress and state of the analysis
  [[nodiscard]] auto getState() const -> std::tuple<ProgressIndicator, State, std::string>
  {
    return {mProgress, mState, mLastErrorMessage};
  }

private:
  /////////////////////////////////////////////////////
  static inline const std::string RESULTS_PATH_NAME{"results"};

  /////////////////////////////////////////////////////
  auto prepareOutputFolder(const std::string &inputFolder) -> std::string;
  ///
  /// \brief Returns if the thread should be stopped
  [[nodiscard]] auto shouldThreadBeStopped() const -> bool
  {
    return mStop;
  }

  void setDetailReportHeader(joda::reporting::Table &detailReportTable, const std::string &channelName,
                             int tempChannelIdx);
  void appendToDetailReport(joda::func::DetectionResponse &result, joda::reporting::Table &detailReportTable,
                            const std::string &detailReportOutputPath, int tempChannelIdx, uint32_t tileIdx);
  void appendToAllOverReport(joda::reporting::Table &allOverReport, const joda::reporting::Table &detailedReport,
                             const std::string &imageName, int nrOfChannels);

  void analyzeImage(joda::reporting::Table &alloverReport, const std::string &imagePath);

  void analyzeTile(joda::reporting::Table &detailReports, std::string imagePath, std::string detailOutputFolder,
                   int tileIdx);
  void analyszeChannel(joda::reporting::Table &detailReports,
                       std::map<int32_t, joda::func::DetectionResponse> &detectionResults,
                       joda::settings::json::ChannelSettings &channelSettings, std::string imagePath,
                       std::string detailOutputFolder, int chIdx, int tileIdx);

  /////////////////////////////////////////////////////
  std::string mInputFolder;
  std::string mOutputFolder;
  bool mStop = false;
  joda::settings::json::AnalyzeSettings mAnalyzeSettings;
  joda::helper::ImageFileContainer *mImageFileContainer;

  ProgressIndicator mProgress;
  State mState = State::STOPPED;
  std::string mLastErrorMessage;
  std::shared_ptr<std::thread> mMainThread;
  ThreadingSettings mThreadingSettings;
};

}    // namespace joda::pipeline
