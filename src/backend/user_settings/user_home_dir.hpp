///
/// \file      user_home_dir.hpp
/// \author    Joachim Danmayr
/// \date      2025-04-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This part of the software is licensed under **AGPL-3.0**.
///

#include <QDir>
#include <filesystem>

namespace joda::user_settings {

inline std::filesystem::path getUserHomeDir(const std::string &subPath = "")
{
  std::filesystem::path homeDir;
#ifdef _WIN32
  if(subPath.empty()) {
    homeDir = std::filesystem::path(QDir::toNativeSeparators(QDir::homePath()).toStdString()) / std::filesystem::path("imagec");
  } else {
    homeDir = std::filesystem::path(QDir::toNativeSeparators(QDir::homePath()).toStdString()) / std::filesystem::path("imagec") /
              std::filesystem::path(subPath);
  }
#else
  if(subPath.empty()) {
    homeDir = std::filesystem::path(QDir::toNativeSeparators(QDir::homePath()).toStdString()) / std::filesystem::path(".imagec");
  } else {
    homeDir = std::filesystem::path(QDir::toNativeSeparators(QDir::homePath()).toStdString()) / std::filesystem::path(".imagec") /
              std::filesystem::path(subPath);
  }
#endif

  if(!std::filesystem::exists(homeDir) || !std::filesystem::is_directory(homeDir)) {
    std::filesystem::create_directories(homeDir);
  }
  return homeDir.string();
}

}    // namespace joda::user_settings
