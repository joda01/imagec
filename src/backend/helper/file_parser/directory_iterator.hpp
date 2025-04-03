///
/// \file      directory_iterator.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-28
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <algorithm>
#include <exception>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>

namespace joda::filesystem {

using namespace std::filesystem;

enum class State
{
  RUNNING,
  FINISHED
};

class DirectoryWatcher
{
public:
  explicit DirectoryWatcher(const std::set<std::string> &supportedFileFormats = {".tif", ".tiff", ".btif", ".btiff", ".btf", ".jpg", ".jpeg", ".vsi",
                                                                                 ".ics", ".czi", ".nd2"});
  ~DirectoryWatcher()
  {
    stop();
  }

  void setWorkingDirectory(uint8_t group, const std::filesystem::path &inputFolder);
  inline std::string getWorkingDirectory(uint8_t group)
  {
    return mWorkingDirectory[group].string();
  }

  ///
  /// \brief      Stops looking for files
  /// \author     Joachim Danmayr
  ///
  void stop()
  {
    mIsStopped = true;
    waitForFinished();
  }

  ///
  /// \brief      Blocks until the file search loop has been finished
  /// \author     Joachim Danmayr
  ///
  void waitForFinished() const
  {
    if(mWorkerThread && mWorkerThread->joinable()) {
      mWorkerThread->join();
    }
  }

  ///
  /// \brief      Returns true if file lookup is running
  /// \author     Joachim Danmayr
  ///
  [[nodiscard]] bool isRunning() const
  {
    return mIsRunning;
  }

  ///
  /// \brief      Returns list of found files
  /// \author     Joachim Danmayr
  ///
  [[nodiscard]] auto getFilesList() const -> const std::map<uint8_t, std::vector<std::filesystem::path>> &
  {
    return mListOfImagePaths;
  }

  ///
  /// \brief      Get first file of list
  /// \author     Joachim Danmayr
  ///
  [[nodiscard]] auto gitFirstFile() const -> std::filesystem::path
  {
    for(const auto &[_, list] : mListOfImagePaths) {
      for(const auto &path : list) {
        return path;
      }
    }
    return {};
  }

  ///
  /// \brief      Returns list of found files
  /// \author     Joachim Danmayr
  ///
  [[nodiscard]] auto getFilesListAt(uint8_t group) const -> const std::vector<std::filesystem::path> &
  {
    return mListOfImagePaths.at(group);
  }

  ///
  /// \brief     Get number of found files
  /// \author    Joachim Danmayr
  ///
  [[nodiscard]] auto getNrOfFiles() const -> uint32_t
  {
    uint32_t size = 0;
    for(const auto [_, list] : mListOfImagePaths) {
      size += list.size();
    }
    return size;
  }

  ///
  /// \brief     Add a file listener
  /// \author    Joachim Danmayr
  ///
  void addListener(const std::function<void(State)> &lookingForFilesFinished)
  {
    mCallbacks.emplace_back(lookingForFilesFinished);
  }

private:
  /////////////////////////////////////////////////////
  void lookForImagesInFolderAndSubfolder();
  bool parseFile(const std::filesystem::directory_entry &path)
  {
    auto ext = path.path().extension().string();
    return mSupportedFormats.contains(ext);
  }

  /////////////////////////////////////////////////////
  std::vector<std::function<void(State)>> mCallbacks;
  std::set<std::string> mSupportedFormats;
  std::map<uint8_t, std::filesystem::path> mWorkingDirectory;
  std::map<uint8_t, std::vector<std::filesystem::path>> mListOfImagePaths;
  bool mIsStopped = false;
  bool mIsRunning = false;
  std::unique_ptr<std::thread> mWorkerThread;
};

}    // namespace joda::filesystem
