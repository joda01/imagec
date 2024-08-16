///
/// \file      directory_iterator.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-28
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <algorithm>
#include <exception>
#include <filesystem>
#include <functional>
#include <memory>
#include <set>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>
#include "file_info.hpp"

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
  explicit DirectoryWatcher(const std::set<std::string> &supportedFileFormats = {
                                ".tif", ".tiff", ".btif", ".btiff", ".btf", ".jpg", ".jpeg", ".vsi", ".ics", ".czi"});
  ~DirectoryWatcher()
  {
    stop();
  }

  void setWorkingDirectory(const std::filesystem::path &inputFolder);
  inline std::string getWorkingDirectory()
  {
    return mWorkingDirectory.string();
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
  [[nodiscard]] auto getFilesList() const -> const std::vector<FileInfo> &
  {
    return mListOfImagePaths;
  }

  ///
  /// \brief      Returns a file on a specific index
  /// \author     Joachim Danmayr
  ///
  [[nodiscard]] auto getFileAt(uint32_t idx) const -> FileInfo
  {
    if(idx < mListOfImagePaths.size()) {
      return mListOfImagePaths.at(idx);
    }
    throw std::runtime_error("File with index idx does not exist!");
  }

  ///
  /// \brief     Get number of found files
  /// \author    Joachim Danmayr
  ///
  [[nodiscard]] auto getNrOfFiles() const -> uint32_t
  {
    return mListOfImagePaths.size();
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
  void lookForImagesInFolderAndSubfolder();

  /////////////////////////////////////////////////////
  std::vector<std::function<void(State)>> mCallbacks;
  std::set<std::string> mSupportedFormats;
  std::filesystem::path mWorkingDirectory;
  std::vector<FileInfo> mListOfImagePaths;
  bool mIsStopped = false;
  bool mIsRunning = false;
  std::unique_ptr<std::thread> mWorkerThread;
};

}    // namespace joda::filesystem
