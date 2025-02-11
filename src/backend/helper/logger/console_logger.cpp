///
/// \file      console_logger.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
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

const int MAX_OUTPUT_LINES = 15;

bool mWithBuffer   = true;
LogLevel mLogLevel = LogLevel::DEBUG;
std::mutex mWriteMutex;
std::deque<std::string> outBuffer;

void setLogLevel(LogLevel logLevel)
{
  mLogLevel = logLevel;
}

void setConsoleLog(bool withBuffer)
{
  mWithBuffer = withBuffer;
  if(withBuffer) {
    // Clear the entire screen and move the cursor to the top-left
    std::cout << "\033[2J\033[H";
  } else {
    std::cout << "\033[15;1H";
  }
}

void printOrAddToBuffer(const std::string &tmp)
{
  if(mWithBuffer) {
    std::lock_guard<std::mutex> lock(mWriteMutex);
    outBuffer.emplace_back(tmp);
  } else {
    std::cout << tmp << std::endl;
  }
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
  if(mLogLevel < LogLevel::ERROR) {
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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void updateTopLine(float progress, float total)
{
  std::lock_guard<std::mutex> lock(mWriteMutex);

  // Save cursor position
  std::cout << "\033[s";

  // Move cursor to the top-left
  std::cout << "\033[1;1H";

  // Clear the first 4 lines (progress bar area)
  for(int i = 0; i < 4; ++i) {
    std::cout << "\033[K";    // Clear line
    if(i < 3)
      std::cout << "\n";
  }

  // Move cursor back to the top-left
  std::cout << "\033[1;1H";

  // Display the progress bar
  int barWidth = 50;
  float ratio  = static_cast<float>(progress) / total;
  int pos      = static_cast<int>(barWidth * ratio);

  std::cout << "Analyze running ...\n[";
  for(int i = 0; i < barWidth; ++i) {
    if(i < pos)
      std::cout << "=";
    else if(i == pos)
      std::cout << ">";
    else
      std::cout << " ";
  }
  std::cout << "] " << int(ratio * 100.0) << " %\n";

  std::cout << "\n";
  std::cout << "\n";

  // Clear the old output lines below the progress bar
  for(int i = 0; i < MAX_OUTPUT_LINES; ++i) {
    std::cout << "\033[K\n";
  }

  // Move cursor back to the start of output area (5th line)
  std::cout << "\033[5;1H";

  // Print the last 10 lines in reverse (newest first)
  int cnt = 0;
  for(auto it = outBuffer.rbegin(); it != outBuffer.rend(); ++it) {
    std::cout << *it << "\n";
    cnt++;
    if(cnt >= MAX_OUTPUT_LINES) {
      break;
    }
  }

  // Restore cursor position for further outputs
  std::cout << "\033[u" << std::flush;
}

}    // namespace joda::log
