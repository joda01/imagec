///
/// \file      console_logger.hpp
/// \author    Joachim Danmayr
/// \date      2023-05-14
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

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

namespace joda::log {

enum class LogLevel
{
  OFF     = -1,
  ERROR   = 0,
  WARNING = 1,
  INFO    = 2,
  DEBUG   = 3,
  TRACE   = 4,
  VERBOSE = 5
};

extern void setLogLevel(LogLevel logLevel);
extern void setConsoleLog(bool onOff);

extern std::string getCurrentDateTimeISO();
extern void logError(const std::string &message);
extern void logWarning(const std::string &message);
extern void logInfo(const std::string &message);
extern void logDebug(const std::string &message);
extern void logTrace(const std::string &message);
extern void updateTopLine(float progress, float total);

}    // namespace joda::log
