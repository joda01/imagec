///
/// \file      updater.hpp
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

#pragma once

#include <memory>
#include <string>
#include <thread>

namespace joda::upd {

///
/// \class      Updater
/// \author     Joachim Danmayr
/// \brief      Updater class
///
class Updater
{
public:
  enum DownloadState
  {
    NO,
    IN_PROGRESS,
    SUCCESSFUL,
    ERROR
  };

  /////////////////////////////////////////////////////
  Updater(int argc, char **argv);
  ~Updater();
  bool updateProgram();
  auto getDownloadProgress() -> std::tuple<int32_t, DownloadState>;
  void restart();
  std::string getRemoteVersion()
  {
    return mRemoteVersion;
  }
  std::string getRemoteHash()
  {
    return mRemoteHash;
  }
  void getRemoteFileVersionAndHash();

private:
  /////////////////////////////////////////////////////

  /////////////////////////////////////////////////////
  int argc;
  char **argv;

  std::string mRemoteVersion;
  std::string mRemoteHash;
  uint32_t mDownloadProgress   = 0;
  DownloadState mDownloadState = DownloadState::NO;
  std::shared_ptr<std::thread> mGetRemoteVersionHash;
  bool mStopped = false;
};
}    // namespace joda::upd
