///
/// \file      duration_count.h
/// \author    Joachim Danmayr
/// \date      2023-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include "backend/helper/logger/console_logger.hpp"

class DurationCount
{
public:
  struct TimeDely
  {
    std::chrono::system_clock::time_point t_start;
    std::string mComment;
  };

  struct TimeStats
  {
    std::chrono::system_clock::duration timeCount;
    int64_t cnt = 0;
  };

  explicit DurationCount(const std::string &comment)
  {
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    mDelay                                      = TimeDely{.t_start = start, .mComment = comment};
  }

  ~DurationCount()
  {
    std::chrono::system_clock::time_point t_end = std::chrono::system_clock::now();
    auto durations                              = t_end - mDelay.t_start;
    double elapsed_time_ms                      = std::chrono::duration<double, std::milli>(durations).count();
    joda::log::logTrace(mDelay.mComment + ": " + std::to_string(elapsed_time_ms) + " ms.");
    std::lock_guard<std::mutex> lock(mLock);
    mStats[mDelay.mComment].cnt++;
    mStats[mDelay.mComment].timeCount += durations;
  }

  static void printStats(double nrOfImages, const std::filesystem::path &outputDir);
  static void resetStats();

private:
  DurationCount::TimeDely mDelay;

  static inline std::map<std::string, TimeStats> mStats;
  static inline uint32_t totalCnt = 0;
  static inline std::mutex mLock;
  static inline std::chrono::time_point<std::chrono::system_clock> mStartTime;
};
