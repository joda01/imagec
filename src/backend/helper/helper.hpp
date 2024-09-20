#pragma once

#include <opencv2/core/hal/interface.h>
#include <array>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>

namespace joda::helper {

inline auto timeNowToString() -> std::tuple<std::string, std::chrono::system_clock::time_point>
{
  auto now               = std::chrono::system_clock::now();
  std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
  std::tm now_tm         = *std::localtime(&now_time_t);
  std::stringstream ss;
  ss << std::put_time(&now_tm, "%Y-%m-%dT%H%M%S");
  std::string now_str = ss.str();
  return {now_str, now};
}

inline auto getFileNameFromPath(const std::filesystem::path &filePathIn) -> std::string
{
  std::regex pattern("[^.a-zA-Z0-9_-]");

  // Use the regex_replace function to replace all matches with an empty string
  return std::regex_replace(filePathIn.filename().string(), pattern, "");
}

inline auto getFolderNameFromPath(const std::filesystem::path &filePathIn) -> std::string
{
  std::regex pattern("[^\\/\\a-zA-Z0-9_-]");

  // Use the regex_replace function to replace all matches with an empty string
  return std::regex_replace(filePathIn.parent_path().string(), pattern, "");
}

inline std::string execCommand(const std::string &cmd, int &out_exitStatus)
{
#ifndef _WIN32
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
#endif
  return "";
}

inline void stringReplace(std::string &str, const std::string &searchStr, const std::string &replaceStr)
{
  size_t pos = str.find(searchStr);
  while(pos != std::string::npos) {
    str.replace(pos, searchStr.length(), replaceStr);
    pos = str.find(searchStr, pos + replaceStr.length());
  }
}

///
/// \brief      Converts a a string to a number
/// \author     Joachim Danmayr
///
inline auto stringToNumber(const std::string &str) -> int
{
  int result = 0;
  for(char c : str) {
    if(isdigit(c)) {
      result = result * 10 + (c - '0');    // Convert digit character to integer
    } else if(isalpha(c)) {
      result = result * 10 + (toupper(c) - 'A' + 1);    // Convert alphabetic character to integer
    } else {
      std::cerr << "Invalid character encountered: " << c << std::endl;
    }
  }
  return result;
};

inline std::string timepointToIsoString(const std::chrono::system_clock::time_point &tp)
{
  std::time_t t = std::chrono::system_clock::to_time_t(tp);
  if(t < 0) {
    return "";
  }
  std::tm tm = *std::gmtime(&t);    // Get UTC time
  char buffer[80];
  std::strftime(buffer, 80, "%Y-%m-%dT%H-%M-%S", &tm);
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;
  std::sprintf(buffer + 80, ".%03lldZ", static_cast<long long>(milliseconds.count()));
  return std::string(buffer);
}

}    // namespace joda::helper

namespace joda::types {
struct Progress
{
  uint32_t finished = 0;
  uint32_t total    = 0;
};

}    // namespace joda::types
