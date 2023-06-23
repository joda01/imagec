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
/// \brief     A short description what happens here.
///

#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>    // std::ostringstream
#include <string>

namespace joda::log {

// Color codes for console formatting
const std::string RESET_COLOR  = "\033[0m";
const std::string RED_COLOR    = "\033[31m";
const std::string GREEN_COLOR  = "\033[32m";
const std::string YELLOW_COLOR = "\033[33m";
const std::string BLUE_COLOR   = "\033[34m";

inline std::string getCurrentDateTimeISO()
{
  auto now                = std::chrono::system_clock::now();
  std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
  std::tm localTime       = *std::localtime(&currentTime);

  std::ostringstream oss;
  oss << std::put_time(&localTime, "%Y-%m-%dT%H:%M:%S");
  return oss.str();
}

inline void logError(const std::string &message)
{
  std::string currentDateTimeISO = getCurrentDateTimeISO();
  std::cout << RED_COLOR << "[ERR] " << RESET_COLOR << "[" << currentDateTimeISO << "] " << message << std::endl;
}

inline void logWarning(const std::string &message)
{
  std::string currentDateTimeISO = getCurrentDateTimeISO();
  std::cout << YELLOW_COLOR << "[WARN]" << RESET_COLOR << "[" << currentDateTimeISO << "] " << message << std::endl;
}

inline void logInfo(const std::string &message)
{
  std::string currentDateTimeISO = getCurrentDateTimeISO();
  std::cout << BLUE_COLOR << "[INFO]" << RESET_COLOR << "[" << currentDateTimeISO << "] " << message << std::endl;
}

}    // namespace joda::log
