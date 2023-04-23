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
  ss << std::put_time(&now_tm, "%Y-%m-%dT%H%M%S");
  std::string now_str = ss.str();
  return now_str;
}

inline auto getFileNameFromPath(const std::string &path) -> std::string
{
  return path;
}
}    // namespace joda::helper
