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
  static auto startNewJob(const settings::json::AnalyzeSettings &settings, const std::string &inputFolder,
                          joda::helper::ImageFileContainer *imageFileContainer) -> std::string
  {
    std::string jobId = std::to_string(mJobCount++);

    auto pipeline       = std::make_shared<pipeline::Pipeline>(settings, imageFileContainer);
    auto mainThreadFunc = [=](std::string inputFolder, std::string jobId) {
      try {
        pipeline->runJob(inputFolder);
      } catch(std::exception &ex) {
        joda::log::logError(ex.what());
      }
      // mJobs.erase(jobId);
    };
    std::future<void> future = std::async(std::launch::async, mainThreadFunc, inputFolder, jobId);
    mJobs.emplace(jobId, Job{pipeline, std::move(future)});

    return jobId;
  };

  static void stopJob(const std::string jobId)
  {
    if(mJobs.contains(jobId)) {
      return mJobs[jobId].pipeline->stopJob();
    }

    throw std::invalid_argument("Job with ID >" + jobId + "< not found!");
  }

  static auto getState(const std::string jobId)
      -> std::tuple<joda::pipeline::Pipeline::ProgressIndicator, joda::pipeline::Pipeline::State>
  {
    if(mJobs.contains(jobId)) {
      return mJobs[jobId].pipeline->getState();
    }
    throw std::invalid_argument("Job with ID >" + jobId + "< not found!");
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

      for(const auto &[uid, processor] : mJobs) {
        auto [_, state] = processor.pipeline->getState();
        if(state == Pipeline::State::FINISHED) {
          processor.future.wait();
          toDelete.emplace(uid);
          joda::log::logInfo("Analyze with process id >" + uid + "< finished!");
        }
      }

      for(const auto &uid : toDelete) {
        mJobs.erase(uid);
      }
      std::this_thread::sleep_for(2.5s);
    }
  }

  struct Job
  {
    std::shared_ptr<Pipeline> pipeline;
    std::future<void> future;
  };
  static inline std::map<std::string, Job> mJobs;
  static inline std::shared_ptr<std::thread> mMainThread;
  static inline bool mStopped = false;
  static inline int mJobCount = 0;
};
}    // namespace joda::pipeline
