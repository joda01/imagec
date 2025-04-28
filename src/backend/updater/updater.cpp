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
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib/httplib.h>
#include <exception>
#include <memory>
#include <string>
#include <thread>
#include "backend/helper/logger/console_logger.hpp"
#include "backend/user_settings/user_settings.hpp"
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

Updater::~Updater()
{
  if(mThread != nullptr && mThread->joinable()) {
    mThread->join();
  }
}

///
/// \brief      Download version file.
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto downloadVersionFile() -> std::string
{
  httplib::Client cli("https://imagec.org:443");
  auto res = cli.Get("/downloads/imagec-x64-linux-bundle.json");
  if(res && res->status == 200) {
    return res->body;
  } else {
    if(res) {
      joda::log::logWarning("Could not check for updates: HTTP error: " + std::to_string(static_cast<int32_t>(res->status)));
    } else {
      joda::log::logWarning("Could not check for updates: Request error: " + std::to_string(static_cast<int32_t>(res.error())));
    }
  }
  return "";
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Updater::triggerCheckForUpdates()
{
  if(mThread != nullptr) {
    return;
  }
  if(mThread && mThread->joinable()) {
    mThread->join();
  }
  mThread = std::make_unique<std::thread>([this]() {
    joda::log::logInfo("Checking for updates ...");
    mStatus                    = Updater::PENDING;
    std::string receivedString = downloadVersionFile();
    if(receivedString.empty()) {
      mStatus = Updater::UPDATE_SERVER_NOT_REACHABLE;
    } else {
      try {
        nlohmann::json versionInformationOnServer = nlohmann::json::parse(receivedString);
        if(versionInformationOnServer.contains("version")) {
          std::string remoteVersion = versionInformationOnServer["version"];
          mResponse.actVersion      = Version::getVersion();
          mResponse.newVersion      = remoteVersion;
          if(versionInformationOnServer.contains("timestamp")) {
            mResponse.newVersionReleaseDate = versionInformationOnServer["timestamp"];
          }
          if(remoteVersion != Version::getVersion()) {
            mStatus = Updater::Status::NEWER_VERSION_AVAILABLE;
            joda::log::logInfo("Update available: " + Version::getVersion() + " -> " + mResponse.newVersion + "!");
          } else {
            mStatus = Updater::UP_TO_DATE;
            joda::log::logInfo("No update available!");
          }
        } else {
          mStatus = Updater::UPDATE_SERVER_NOT_REACHABLE;
          joda::log::logWarning("Could not check for updates: Wrong file format: Missing version field!");
        }
      } catch(const std::exception &ex) {
        mStatus = Updater::UPDATE_SERVER_NOT_REACHABLE;
        joda::log::logWarning("Could not check for updates: Wrong file format: " + std::string(ex.what()));
      }
    }
  });
}

auto Updater::getCheckForUpdateResponse(CheckForUpdateResponse &response) -> Status
{
  response = mResponse;
  return mStatus;
}

}    // namespace joda::updater
