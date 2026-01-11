///
/// \file      console_logger.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <string>
#include <string_view>
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

/**
 * @brief Initializes the background worker thread.
 * Must be called before logging if async output is required.
 */
void initLogger();

/**
 * @brief Stops the worker thread and flushes remaining logs.
 */
void joinLogger();

/**
 * @brief Sets the global filter level for logging.
 */
void setLogLevel(LogLevel logLevel);

/**
 * @brief Returns a thread-safe ISO 8601 timestamp string.
 */
std::string getCurrentDateTimeISO();

// Logging API
void logError(const std::string &message);
void logWarning(const std::string &message);
void logInfo(const std::string &message);
void logDebug(const std::string &message);
void logTrace(const std::string &message);
void logProgress(float progress, const std::string &message);

/**
 * @brief Returns a copy of the current log history.
 * Thread-safe: creates a snapshot under a mutex.
 */
std::vector<std::string> getLogBuffer();

/**
 * @brief Converts the current log buffer to an HTML-formatted string.
 * This includes ANSI color code parsing.
 */
std::string logBufferToHtml();

}    // namespace joda::log
