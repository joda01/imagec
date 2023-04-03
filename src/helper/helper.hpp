#pragma once

#include <opencv2/core/hal/interface.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace joda::helper {

inline auto timeNowToString() -> std::string
{
  auto now               = std::chrono::system_clock::now();
  std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
  std::tm now_tm         = *std::localtime(&now_time_t);
  std::stringstream ss;
  ss << std::put_time(&now_tm, "%Y-%m-%dT%H:%M:%S");
  std::string now_str = ss.str();
  return now_str;
}
}    // namespace joda::helper

inline auto getFileNameFromPath(const std::string &path) -> std::string
{
  return path;
}

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBSTRS "************************************************************"
#define PBWIDTH 60

struct Progress
{
  uint64 actNr;
  uint64 totalNr;
};

inline void printProgress(Progress main, Progress sub)
{
  //
  // Paint main
  //
  {
    if(main.totalNr == 0) {
      main.totalNr = 1;
    }
    double percentage = (double) main.actNr / (double) main.totalNr;
    int val           = (int) (percentage * 100);
    int lpad          = (int) (percentage * PBWIDTH);
    int rpad          = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
    std::cout << " " << std::to_string(main.actNr) << "/" << std::to_string(main.totalNr) << std::endl;
  }
  //
  // Paint sub
  //
  {
    if(sub.totalNr == 0) {
      sub.totalNr = 1;
    }
    double percentage = (double) sub.actNr / (double) sub.totalNr;
    int val           = (int) (percentage * 100);
    int lpad          = (int) (percentage * PBWIDTH);
    int rpad          = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTRS, rpad, "");
    fflush(stdout);
    std::cout << " " << std::to_string(sub.actNr) << "/" << std::to_string(sub.totalNr) << "\n" << std::endl;
  }
}
