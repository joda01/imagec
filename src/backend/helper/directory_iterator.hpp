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
/// \brief     A short description what happens here.
///

#pragma once

#include <algorithm>
#include <exception>
#include <filesystem>
#include <memory>
#include <set>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include "file_info.hpp"
#include "helper.hpp"

namespace joda::helper {

using namespace std::filesystem;

template <class T>
concept FileInfo_t = std::is_base_of<FileInfo, T>::value;

template <FileInfo_t FILEINFO>
class DirectoryWatcher
{
public:
  DirectoryWatcher(const std::set<std::string> &supportedFileFormats) : mSupportedFormats(supportedFileFormats)
  {
  }

  ~DirectoryWatcher()
  {
    stop();
  }

  ///
  /// \brief      Find all images in the given infolder and its subfolder.
  /// \author     Joachim Danmayr
  ///
  inline void setWorkingDirectory(const std::string &inputFolder)
  {
    if(mWorkingDirectory != inputFolder) {
      mWorkingDirectory = inputFolder;
      if(inputFolder.empty()) {
        mListOfImagePaths.clear();
      } else {
        stop();
        mWorkerThread = std::make_shared<std::thread>(&DirectoryWatcher::lookForImagesInFolderAndSubfolder, this);
      }
    }
  }

  ///
  /// \brief      Returns the selected working directory
  /// \author     Joachim Danmayr
  ///
  inline const std::string &getWorkingDirectory()
  {
    return mWorkingDirectory;
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
  [[nodiscard]] auto getFilesList() const -> const std::vector<FILEINFO> &
  {
    return mListOfImagePaths;
  }

  ///
  /// \brief      Returns a file on a specific index
  /// \author     Joachim Danmayr
  ///
  [[nodiscard]] auto getFileAt(uint32_t idx) const -> FILEINFO
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

private:
  ///
  /// \brief      Find all images in the given infolder and its subfolder.
  /// \author     Joachim Danmayr
  ///
  void lookForImagesInFolderAndSubfolder()
  {
    mIsRunning = true;
    mIsStopped = false;
    mListOfImagePaths.clear();

    for(recursive_directory_iterator i(mWorkingDirectory), end; i != end; ++i) {
      try {
        if(!is_directory(i->path())) {
          FILEINFO fi;
          auto supported = fi.parseFile(*i, mSupportedFormats);
          if(supported) {
            mListOfImagePaths.push_back(fi);
          }
        }
      } catch(const std::exception &ex) {
        std::cout << ex.what() << std::endl;
      }
      if(mIsStopped) {
        break;
      }
    }
    mIsRunning = false;
  }

  /////////////////////////////////////////////////////
  std::set<std::string> mSupportedFormats;
  std::string mWorkingDirectory;
  std::vector<FILEINFO> mListOfImagePaths;
  bool mIsStopped = false;
  bool mIsRunning = false;
  std::shared_ptr<std::thread> mWorkerThread;
};

}    // namespace joda::helper
