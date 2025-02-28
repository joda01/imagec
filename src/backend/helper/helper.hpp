#pragma once

#include <bits/chrono.h>
#include <opencv2/core/hal/interface.h>
#include <array>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>

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

inline bool stringContains(const std::string &text, const std::string &searchFor)
{
  // Check if "helloe" is in the string
  return text.find(searchFor) != std::string::npos;
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

inline std::string toLower(const std::string &input)
{
  std::string result = input;
  std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
  return result;
}

inline std::string timepointToIsoString(const std::chrono::system_clock::time_point &tp)
{
  std::time_t t = std::chrono::system_clock::to_time_t(tp);
  if(t < 0) {
    return "";
  }
  std::tm tm       = *std::gmtime(&t);    // Get UTC time
  char buffer[120] = {0};
  std::strftime(buffer, 80, "%Y-%m-%dT%H-%M-%S", &tm);
  auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) % 1000;
  std::sprintf(buffer + 80, ".%03lldZ", static_cast<long long>(milliseconds.count()));
  return std::string(buffer);
}

inline std::string timepointToDelay(const std::chrono::system_clock::time_point &pastTime)
{
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  auto delta                                = now - pastTime;

  auto duration   = duration_cast<std::chrono::seconds>(delta).count();
  std::string pre = " sec";
  if(duration >= 60) {
    duration = duration_cast<std::chrono::minutes>(delta).count();
    pre      = " min";
    if(duration >= 60) {
      duration = duration_cast<std::chrono::hours>(delta).count();
      pre      = " hrs";
      if(duration >= 24) {
        duration = duration_cast<std::chrono::days>(delta).count();
        pre      = " dys";
        if(duration >= 7) {
          duration = duration_cast<std::chrono::weeks>(delta).count();
          pre      = " wks";
          if(duration >= 4) {
            duration = duration_cast<std::chrono::months>(delta).count();
            pre      = " mos";
            if(duration >= 12) {
              duration = duration_cast<std::chrono::years>(delta).count();
              pre      = " yrs";
            }
          }
        }
      }
    }
  }

  return std::to_string(duration) + pre;
}

inline std::string getDurationAsString(const std::chrono::system_clock::time_point &t1, const std::chrono::system_clock::time_point &t2)
{
  // Calculate the duration between t1 and t2 in seconds
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1);

  // Extract minutes and seconds
  auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration).count();
  auto seconds = duration.count() % 60;

  // Create a formatted string
  std::ostringstream oss;
  oss << minutes << " min. " << seconds << " sec.";

  return oss.str();
}

// Function to trim whitespace from the beginning and end of a string
inline std::string trim(const std::string &str)
{
  auto start = str.find_first_not_of(" \t\n\r");
  if(start == std::string::npos)
    return "";    // All whitespace
  auto end = str.find_last_not_of(" \t\n\r");
  return str.substr(start, end - start + 1);
}

inline std::string shrinkString(const std::string &str, size_t maxLength)
{
  if(str.length() <= maxLength) {
    return str;    // No need to shrink
  } else {
    return str.substr(0, maxLength);    // Shrink to maxLength
  }
}

// Function to split a string by spaces
inline std::vector<std::string> split(const std::string &str, const std::vector<char> &delimiters)
{
  std::unordered_set<char> delimiter_set(delimiters.begin(), delimiters.end());
  std::vector<std::string> result;
  std::string current;

  for(char ch : str) {
    if(delimiter_set.find(ch) != delimiter_set.end()) {
      // If the current character is a delimiter
      if(!current.empty()) {
        result.push_back(current);
        current.clear();
      }
    } else {
      // Add non-delimiter character to the current segment
      current += ch;
    }
  }
  // Add the last segment if it exists
  if(!current.empty()) {
    result.push_back(current);
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
