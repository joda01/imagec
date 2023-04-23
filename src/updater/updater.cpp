///
/// \file      updater.cpp
/// \author    Joachim Danmayr
/// \date      2023-04-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "updater.hpp"
#include <httplib.h>
#include <stdio.h>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <memory>
#include <thread>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json   = nlohmann::json;

///
/// \brief      Constructor
/// \author     Joachim Danmayr
/// \param[in]  pathToBinary Path to the original binary
///
Updater::Updater(int argc, char **argv) : argc(argc), argv(argv)
{
  mGetRemoteVersionHash = std::make_shared<std::thread>(&Updater::getRemoteFileVersionAndHash, this);
}

Updater::~Updater()
{
  mStopped = true;
  if(mGetRemoteVersionHash->joinable()) {
    mGetRemoteVersionHash->join();
  }
}

///
/// \brief      Download new version from update server and override old one
/// \author     Joachim Danmayr
///
bool Updater::updateProgram()
{
  mDownloadProgress = 0;
  mDownloadState    = DownloadState::IN_PROGRESS;
  try {
    httplib::Client cli("https://a3w6k4.myvserver.online:4443");
    auto res = cli.Get("/imagec", [this](uint64_t len, uint64_t total) {
      mDownloadProgress = (int) (len * 100 / total);
      // printf("%lld / %lld bytes => %d%% complete\n", len, total, mDownloadProgress);
      return true;    // return 'false' if you want to cancel the request.
    });
    if(res && res->status == 200) {
      std::ofstream out("imagec_new", std::ios::binary);
      out.write(res->body.data(), res->body.size());
      out.close();

      try {
        fs::rename("imagec_new", std::string(argv[0]));
        fs::permissions(std::string(argv[0]), fs::perms::owner_exec | fs::perms::owner_all | fs::perms::group_exec |
                                                  fs::perms::group_all | fs::perms::others_exec);

      } catch(const fs::filesystem_error &e) {
        std::cerr << "Error moving file: " << e.what() << std::endl;
        mDownloadState = DownloadState::ERROR;
        return false;
      }
      mDownloadState = DownloadState::SUCCESSFUL;
      return true;
    }

    mDownloadState = DownloadState::ERROR;
    return false;
  } catch(const std::exception &) {
    mDownloadState = DownloadState::ERROR;
    return false;
  }
}

///
/// \brief      Returns download state
/// \author     Joachim Danmayr
///
auto Updater::getDownloadProgress() -> std::tuple<int32_t, DownloadState>
{
  return {mDownloadProgress, mDownloadState};
}

///
/// \brief      Get sha256 and version from remote
/// \author     Joachim Danmayr
///
void Updater::getRemoteFileVersionAndHash()
{
  mRemoteVersion = "";
  mRemoteHash    = "";
  while(!mStopped) {
    try {
      httplib::Client cli("https://a3w6k4.myvserver.online:4443");
      std::cout << "GET" << std::endl;
      auto res = cli.Get("/imagec_hash.json");
      if(res && res->status == 200) {
        std::string js(res->body.data(), res->body.size());
        auto parsed    = json::parse(js);
        mRemoteVersion = parsed["version"];
        mRemoteHash    = parsed["sha256"];
        mStopped       = true;
      } else {
        std::cout << "Error" << std::endl;
      }
    } catch(const std::exception &ex) {
      std::cout << ex.what() << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
}

///
/// \brief      Restart program
/// \author     Joachim Danmayr
///
void Updater::restart()
{
  // Check if this is the first run of the program

  // Get the program name and arguments
  auto restartThread = [&]() {
    std::string program_name = argv[0];
    std::string arguments    = "";
    for(int i = 1; i < argc; ++i) {
      arguments += " " + std::string(argv[i]);
    }

    // Execute the program again with the same arguments
    std::string command = program_name + arguments;
    std::cout << command << std::endl;
    std::system(command.c_str());
  };

  std::thread t(restartThread);
  t.detach();
  sleep(5);
}
