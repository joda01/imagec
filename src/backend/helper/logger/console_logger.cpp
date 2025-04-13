///
/// \file      console_logger.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "console_logger.hpp"
#include <deque>
#include <mutex>
#include <regex>
#include <unordered_map>
#include <vector>

namespace joda::log {

// Color codes for console formatting
const std::string RESET_COLOR  = "\033[0m";
const std::string RED_COLOR    = "\033[31m";
const std::string GREEN_COLOR  = "\033[32m";
const std::string GRAY_COLOR   = "\033[37m";
const std::string YELLOW_COLOR = "\033[33m";
const std::string BLUE_COLOR   = "\033[34m";

LogLevel mLogLevel = LogLevel::TRACE;
std::mutex mWriteMutex;
float lastProgress = 0;
std::vector<std::string> logBuffer;

void setLogLevel(LogLevel logLevel)
{
  mLogLevel = logLevel;
}

auto getLogBuffer() -> const std::vector<std::string> &
{
  return logBuffer;
}

std::string ansiToHtml(const std::string &input)
{
  std::unordered_map<int, std::string> ansiColorMap = {
      {30, "#000000"},    // black
      {31, "#ff0000"},    // red
      {32, "#00ff00"},    // green
      {33, "#ffff00"},    // yellow
      {34, "#0000ff"},    // blue
      {35, "#ff00ff"},    // magenta
      {36, "#00ffff"},    // cyan
      {37, "#a9a9a9"},    // gray
                          // Add more as needed
  };

  std::string output;
  std::regex ansiRegex("\033\\[(\\d+)m");
  std::smatch match;
  std::string::const_iterator searchStart(input.cbegin());
  bool spanOpen = false;

  while(std::regex_search(searchStart, input.cend(), match, ansiRegex)) {
    output += std::string(searchStart, match[0].first);    // Text before match
    int code = std::stoi(match[1]);

    if(code == 0) {
      if(spanOpen) {
        output += "</span>";
        spanOpen = false;
      }
    } else if(ansiColorMap.count(code)) {
      if(spanOpen) {
        output += "</span>";
      }
      output += "<span style=\"color:" + ansiColorMap[code] + ";\">";
      spanOpen = true;
    }

    searchStart = match.suffix().first;
  }

  output += std::string(searchStart, input.cend());

  if(spanOpen) {
    output += "</span>";
  }

  return output;
}

auto logBufferToHtml() -> std::string
{
  auto joinWithNewlines = [](const std::vector<std::string> &lines) -> std::string {
    std::ostringstream oss;
    for(size_t i = 0; i < lines.size(); ++i) {
      oss << lines[i];
      if(i != lines.size() - 1) {
        oss << "<br/>";
      }
    }
    return oss.str();
  };

  std::string result = joinWithNewlines(logBuffer);
  return ansiToHtml(result);
}

void printOrAddToBuffer(const std::string &tmp)
{
  std::cout << tmp << std::endl;
  logBuffer.emplace_back(tmp);
}

std::string toPercentString(float ratio)
{
  auto percent = int(ratio * 100.0);
  std::string percentString;
  if(percent < 10) {
    percentString = "  " + std::to_string(percent) + "%";
  } else if(percent < 100) {
    percentString = " " + std::to_string(percent) + "%";
  } else {
    percentString = std::to_string(percent) + "%";
  }
  return percentString;
}

std::string getCurrentDateTimeISO()
{
  auto now                = std::chrono::system_clock::now();
  std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
  std::tm localTime       = *std::localtime(&currentTime);

  std::ostringstream oss;
  oss << std::put_time(&localTime, "%Y-%m-%dT%H:%M:%S");
  return oss.str();
}

void logError(const std::string &message)
{
  if(mLogLevel < LogLevel::ERROR_) {
    return;
  }
  std::string currentDateTimeISO = getCurrentDateTimeISO();
  std::string tmp                = RED_COLOR + "[ERR] " + RESET_COLOR + "[" + currentDateTimeISO + "] " + message;
  printOrAddToBuffer(tmp);
}

void logWarning(const std::string &message)
{
  if(mLogLevel < LogLevel::WARNING) {
    return;
  }
  std::string currentDateTimeISO = getCurrentDateTimeISO();
  std::string tmp                = YELLOW_COLOR + "[WARN]" + RESET_COLOR + "[" + currentDateTimeISO + "] " + message;
  printOrAddToBuffer(tmp);
}

void logInfo(const std::string &message)
{
  if(mLogLevel < LogLevel::INFO) {
    return;
  }
  std::string currentDateTimeISO = getCurrentDateTimeISO();
  std::string tmp                = BLUE_COLOR + "[INFO]" + RESET_COLOR + "[" + currentDateTimeISO + "] " + message;
  printOrAddToBuffer(tmp);
}

void logDebug(const std::string &message)
{
  if(mLogLevel < LogLevel::DEBUG) {
    return;
  }
  std::string currentDateTimeISO = getCurrentDateTimeISO();
  std::string tmp                = GREEN_COLOR + "[DEBG]" + RESET_COLOR + "[" + currentDateTimeISO + "] " + message;
  printOrAddToBuffer(tmp);
}

void logTrace(const std::string &message)
{
  if(mLogLevel < LogLevel::TRACE) {
    return;
  }
  std::string currentDateTimeISO = getCurrentDateTimeISO();
  std::string tmp                = GRAY_COLOR + "[TRACE]" + RESET_COLOR + "[" + currentDateTimeISO + "] " + message;
  printOrAddToBuffer(tmp);
}

void logProgress(float ratio, const std::string &message)
{
  lastProgress                   = ratio;
  std::string currentDateTimeISO = getCurrentDateTimeISO();
  std::string tmp                = GRAY_COLOR + "[" + toPercentString(ratio) + "]" + RESET_COLOR + "[" + currentDateTimeISO + "] " + message;
  printOrAddToBuffer(tmp);
}

}    // namespace joda::log
