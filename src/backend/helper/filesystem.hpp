
#pragma once

#include <filesystem>

namespace joda::filesystem {

namespace fs = std::filesystem;

inline void removeDirectory(const fs::path &path)
{
  if(fs::exists(path) && fs::is_directory(path)) {
    for(const auto &entry : fs::directory_iterator(path)) {
      if(fs::is_directory(entry)) {
        removeDirectory(entry.path());
      } else {
        fs::remove(entry);
      }
    }
    fs::remove(path);
  }
}
}    // namespace joda::filesystem
