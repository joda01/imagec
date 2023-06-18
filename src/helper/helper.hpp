#pragma once

#include <opencv2/core/hal/interface.h>
#include <unistd.h>
#include <chrono>
#include <cstdlib>
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

inline std::string execCommand(const std::string &cmd, int &out_exitStatus)
{
  out_exitStatus = 0;
  auto pPipe     = ::popen(cmd.c_str(), "r");
  if(pPipe == nullptr) {
    throw std::runtime_error("Cannot open pipe");
  }

  std::array<char, 256> buffer;

  std::string result;

  while(not std::feof(pPipe)) {
    auto bytes = std::fread(buffer.data(), 1, buffer.size(), pPipe);
    result.append(buffer.data(), bytes);
  }

  auto rc = ::pclose(pPipe);

  if(WIFEXITED(rc)) {
    out_exitStatus = WEXITSTATUS(rc);
  }

  return result;
}

}    // namespace joda::helper

namespace joda::types {
struct Progress
{
  uint32_t finished = 0;
  uint32_t total    = 0;
};

}    // namespace joda::types
