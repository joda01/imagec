///
/// \file      console_logger.hpp
/// \author    Joachim Danmayr
/// \date      2023-05-14
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <ostream>
#include <sstream>    // std::ostringstream
#include <string>
#include <vector>

namespace joda::log {

enum class LogLevel
{
  OFF     = -1,
  ERROR_  = 0,
  WARNING = 1,
  INFO    = 2,
  DEBUG   = 3,
  TRACE   = 4,
  VERBOSE = 5
};

extern void initLogger();
extern void joinLogger();
extern void setLogLevel(LogLevel logLevel);
extern std::string getCurrentDateTimeISO();
extern void logError(const std::string &message);
extern void logWarning(const std::string &message);
extern void logInfo(const std::string &message);
extern void logDebug(const std::string &message);
extern void logTrace(const std::string &message);
extern void logProgress(float progress, const std::string &message);
extern auto getLogBuffer() -> const std::vector<std::string> &;
extern auto logBufferToHtml() -> std::string;

}    // namespace joda::log
