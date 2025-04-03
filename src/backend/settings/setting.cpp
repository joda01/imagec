///
/// \file      setting.cpp
/// \author    Joachim Danmayr
/// \date      2024-06-12
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "setting.hpp"
#include "backend/helper/logger/console_logger.hpp"

void SettingParserLog::print() const
{
  std::string toLog = "[" + commandNameOfOccurrence + "] " + message;
  switch(severity) {
    case Severity::JODA_INFO:
      joda::log::logInfo(toLog);
      break;
    case Severity::JODA_WARNING:
      joda::log::logWarning(toLog);
      break;
    case Severity::JODA_ERROR:
      joda::log::logError(toLog);
      break;
  }
}
