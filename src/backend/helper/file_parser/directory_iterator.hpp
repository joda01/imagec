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
  explicit DirectoryWatcher(const std::set<std::string> &supportedFileFormats = {
                                ".tif", ".tiff", ".btif", ".btiff", ".btf", ".jpg", ".jpeg", ".vsi", ".ics", ".czi", ".nd2",
                                ".lif", ".lei",  ".fli",  ".scn",   ".sxm", ".lim", ".oir",  ".top", ".stk", ".nd",  ".bip",
                                ".fli", ".msr",  ".dm3",  ".dm4",   ".img", ".cr2", ".ch5",  ".dib", ".ims", ".pic", ".raw",
                                ".1sc", ".std",  ".spc",  ".avi",   ".cif", ".sif", ".aim",  ".svs", ".arf", ".sld"});
  ~DirectoryWatcher()
  {
    stop();
  }

  void setWorkingDirectory(const std::filesystem::path &inputFolder);
  inline std::string getWorkingDirectory()
  {
    return mWorkingDirectory.generic_string();
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
  [[nodiscard]] auto getFilesList() const -> const std::vector<std::filesystem::path> &
  {
    return mListOfImagePaths;
  }

  ///
  /// \brief      Get first file of list
  /// \author     Joachim Danmayr
  ///
  [[nodiscard]] auto gitFirstFile() const -> std::filesystem::path
  {
    for(const auto &path : mListOfImagePaths) {
      return path;
    }

    return {};
  }

  ///
  /// \brief      Returns list of found files
  /// \author     Joachim Danmayr
  ///
  [[nodiscard]] auto getFilesListAt() const -> const std::vector<std::filesystem::path> &
  {
    return mListOfImagePaths;
  }

  ///
  /// \brief      Returns list of found files
  /// \author     Joachim Danmayr
  ///
  [[nodiscard]] auto getDirectoryAt() const -> std::filesystem::path
  {
    return mWorkingDirectory;
  }

  ///
  /// \brief     Get number of found files
  /// \author    Joachim Danmayr
  ///
  [[nodiscard]] auto getNrOfFiles() const -> size_t
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
  /////////////////////////////////////////////////////
  void lookForImagesInFolderAndSubfolder();
  bool parseFile(const std::filesystem::directory_entry &path)
  {
    auto ext = path.path().extension().generic_string();
    return mSupportedFormats.contains(ext);
  }

  /////////////////////////////////////////////////////
  std::vector<std::function<void(State)>> mCallbacks;
  std::set<std::string> mSupportedFormats;
  std::filesystem::path mWorkingDirectory;
  std::vector<std::filesystem::path> mListOfImagePaths;
  bool mIsStopped = false;
  bool mIsRunning = false;
  std::unique_ptr<std::thread> mWorkerThread;
};

}    // namespace joda::filesystem
