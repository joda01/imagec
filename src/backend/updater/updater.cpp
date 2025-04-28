///
/// \file      updater.cpp
/// \author    Joachim Danmayr
/// \date      2025-04-28
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "updater.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

namespace joda::updater {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
Updater::Updater()
{
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto Updater::checkForUpdates(CheckForUpdateResponse &response) -> Updater::Status
{
  // https://imagec.org/downloads/imagec-x64-linux-bundle.json
  std::string receivedString;
  nlohmann::json versionInformationOnServer = nlohmann::json::parse(receivedString);

  if(versionInformationOnServer.contains("version")) {
    std::string remoteVersion = versionInformationOnServer["version"];
    response.actVersion       = Version::getVersion();
    response.newVersion       = remoteVersion;
    if(versionInformationOnServer.contains("timestamp")) {
      response.newVersionReleaseDate = versionInformationOnServer["timestamp"];
    }

    if(remoteVersion != Version::getVersion()) {
      return Updater::Status::NEWER_VERSION_AVAILABLE;
    } else {
      return Updater::UP_TO_DATE;
    }
  }

  return Updater::UPDATE_SERVER_NOT_REACHABLE;
}

}    // namespace joda::updater
