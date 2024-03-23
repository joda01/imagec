///
/// \file      pipeline_factory.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-12
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

// #include <malloc.h>
#include <exception>
#include <future>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include "../helper/helper.hpp"
#include "../logger/console_logger.hpp"
#include "../reporting/reporting.h"
#include "../settings/analze_settings_parser.hpp"
#include "pipeline.hpp"
namespace joda::pipeline {

using namespace std::chrono_literals;

class PipelineFactory
{
public:
  ///
  /// \brief      Init method which starts the main thread.
  ///             Call this function in the main.
  /// \author     Joachim Danmayr
  ///
  static void init()
  {
    mMainThread = std::make_shared<std::thread>(observer);
  }

  ///
  /// \brief     Reset to default variables
  /// \author     Joachim Danmayr
  ///
  static void reset()
  {
    mLastJobProgressIndicator = {};
    mLastErrorMessage         = "";
  }

  ///
  /// \brief      Blocking function which stops the main thread
  /// \author     Joachim Danmayr
  ///
  static void shutdown()
  {
    mStopped = true;
    if(mMainThread) {
      mMainThread->join();
    }
  }

  ///
  /// \brief      Generate a new pipeline job
  /// \author     Joachim Danmayr
  ///
  static auto
  startNewJob(const settings::json::AnalyzeSettings &settings, const std::string &inputFolder,
              joda::helper::ImageFileContainer *imageFileContainer,
              const pipeline::Pipeline::ThreadingSettings &threadingSettings = pipeline::Pipeline::ThreadingSettings())
      -> std::string
  {
    std::string jobId = std::to_string(mJobCount++);
    mJob = std::make_unique<pipeline::Pipeline>(settings, imageFileContainer, inputFolder, threadingSettings);
    mLastOutputFolder = mJob->getOutputFolder();
    return jobId;
  };

  static void stopJob(const std::string jobId)
  {
    if(mJob) {
      return mJob->stopJob();
    }

    throw std::invalid_argument("Job with ID >" + jobId + "< not found!");
  }

  static auto getState(const std::string jobId)
      -> std::tuple<joda::pipeline::Pipeline::ProgressIndicator, joda::pipeline::Pipeline::State, std::string>
  {
    if(mJob) {
      return mJob->getState();
    }
    if(mLastErrorMessage.empty()) {
      return {mLastJobProgressIndicator, joda::pipeline::Pipeline::State::FINISHED, mLastErrorMessage};
    } else {
      return {mLastJobProgressIndicator, joda::pipeline::Pipeline::State::ERROR_, mLastErrorMessage};
    }
  }

  static auto getOutputFolder(const std::string jobId) -> std::string
  {
    return mLastOutputFolder;
  }

private:
  ///
  /// \brief      Observes running threads and clean up finished ones
  /// \author     Joachim Danmayr
  ///
  static void observer()
  {
    while(!mStopped) {
      std::set<std::string> toDelete;
      if(mJob) {
        auto [progress, state, errorMsg] = mJob->getState();
        if(state == Pipeline::State::FINISHED || state == Pipeline::State::ERROR_) {
          mLastJobProgressIndicator = progress;
          mLastErrorMessage         = errorMsg;

          joda::log::logInfo("Analyze finished!");
          mJob.reset();
          // malloc_trim(0);
        }
      }

      std::this_thread::sleep_for(2.5s);
    }
  }

  static inline joda::pipeline::Pipeline::ProgressIndicator mLastJobProgressIndicator;
  static inline std::string mLastErrorMessage;
  static inline std::string mLastOutputFolder;
  static inline std::unique_ptr<Pipeline> mJob = nullptr;
  static inline std::shared_ptr<std::thread> mMainThread;
  static inline bool mStopped = false;
  static inline int mJobCount = 0;
};
}    // namespace joda::pipeline
