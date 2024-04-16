///
/// \file      duration_count.h
/// \author    Joachim Danmayr
/// \date      2023-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include "backend/logger/console_logger.hpp"

class DurationCount
{
public:
  struct TimeDely
  {
    std::chrono::system_clock::time_point t_start;
    std::string mComment;
  };

  static uint32_t start(std::string comment)
  {
    totalCnt++;
    srand((unsigned) time(0) + totalCnt);
    uint32_t randNr = (rand() % INT32_MAX) + 1;
    std::lock_guard<std::mutex> lock(mLock);
    mDelays[randNr] = {.t_start = std::chrono::high_resolution_clock::now(), .mComment = comment};
    return randNr;
  }
  static void stop(uint32_t rand)
  {
    auto t_end = std::chrono::high_resolution_clock::now();
    std::lock_guard<std::mutex> lock(mLock);
    double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - mDelays[rand].t_start).count();
    joda::log::logTrace(mDelays[rand].mComment + ": " + std::to_string(elapsed_time_ms) + " ms.");
    mDelays.erase(rand);
  }

private:
  static inline std::map<uint32_t, TimeDely> mDelays;
  static inline uint32_t totalCnt = 0;
  static inline std::mutex mLock;
};
