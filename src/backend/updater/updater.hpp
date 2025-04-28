///
/// \file      updater.hpp
/// \author    Joachim Danmayr
/// \date      2025-04-28
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/user_settings/user_settings.hpp"
#include "version.h"

namespace joda::updater {

///
/// \class      Updater
/// \author     Joachim Danmayr
/// \brief      Auto update service
///
class Updater
{
public:
  struct CheckForUpdateResponse
  {
    std::string actVersion;
    std::string newVersion;
    std::string newVersionReleaseDate;
  };
  enum Status
  {
    UP_TO_DATE,
    NEWER_VERSION_AVAILABLE,
    UPDATE_SERVER_NOT_REACHABLE,
  };

  /////////////////////////////////////////////////////
  Updater();
  auto checkForUpdates(CheckForUpdateResponse &response) -> Status;

private:
  /////////////////////////////////////////////////////
};

}    // namespace joda::updater
