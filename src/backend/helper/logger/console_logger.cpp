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

void setLogLevel(LogLevel logLevel)
{
  mLogLevel = logLevel;
}

void printOrAddToBuffer(const std::string &tmp)
{
  std::cout << tmp << std::endl;
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
