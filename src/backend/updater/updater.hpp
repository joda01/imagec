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

#include <memory>
#include <thread>
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
    PENDING,
    UP_TO_DATE,
    NEWER_VERSION_AVAILABLE,
    UPDATE_SERVER_NOT_REACHABLE,
  };

  /////////////////////////////////////////////////////
  Updater();
  ~Updater();
  auto getCheckForUpdateResponse(CheckForUpdateResponse &response) -> Status;
  void triggerCheckForUpdates();

private:
  /////////////////////////////////////////////////////
  Status mStatus = Status::PENDING;
  CheckForUpdateResponse mResponse;
  std::unique_ptr<std::thread> mThread;
};

}    // namespace joda::updater
