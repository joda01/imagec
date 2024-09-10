///
/// \file      setting.cpp
/// \author    Joachim Danmayr
/// \date      2024-06-12
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "setting.hpp"
#include "backend/helper/logger/console_logger.hpp"

void SettingParserLog::print() const
{
  std::string toLog = "[" + commandNameOfOccurrence + "] " + message;
  switch(severity) {
    case INFO:
      joda::log::logInfo(toLog);
      break;
    case WARNING:
      joda::log::logWarning(toLog);
      break;
    case ERROR:
      joda::log::logError(toLog);
      break;
  }
}
