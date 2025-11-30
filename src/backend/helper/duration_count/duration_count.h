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
    std::chrono::steady_clock::time_point t_start;
    std::string mComment;
  };

  struct TimeStats
  {
    std::chrono::steady_clock::duration timeCount;
    int64_t cnt = 0;
  };

  explicit DurationCount(const std::string_view comment) : mDelay{std::chrono::steady_clock::now(), std::string(comment)}
  {
  }

  ~DurationCount()
  {
    stop();
  }

  void stop()
  {
    if(!mStopped) {
      mStopped = true;

      const auto t_end     = std::chrono::steady_clock::now();
      const auto durations = t_end - mDelay.t_start;

      // Efficient string building (small vector optimization will avoid allocations)
      std::string msg;
      msg.reserve(mDelay.mComment.size() + 32);
      msg += mDelay.mComment;
      msg += ": ";
      msg += std::to_string(std::chrono::duration<double, std::milli>(durations).count());
      msg += " ms.";

      joda::log::logTrace(msg);

      // Lock once, lookup once
      {
        std::lock_guard<std::mutex> lock(mLock);
        auto &entry = mStats[mDelay.mComment];
        entry.cnt++;
        entry.timeCount += durations;
      }
    }
  }

  static void printStats(double nrOfImages, const std::filesystem::path &outputDir);
  static void resetStats();

private:
  DurationCount::TimeDely mDelay;

  static inline std::map<std::string, TimeStats> mStats;
  static inline uint32_t totalCnt = 0;
  static inline std::mutex mLock;
  static inline std::chrono::time_point<std::chrono::steady_clock> mStartTime;
  bool mStopped = false;
};
