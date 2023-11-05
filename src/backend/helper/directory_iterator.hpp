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
#include <thread>
#include "file_info.hpp"
#include "helper.hpp"

namespace joda::helper {

using namespace std::filesystem;

class ImageFileContainer
{
public:
  ImageFileContainer()
  {
  }

  ~ImageFileContainer()
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
        mWorkerThread = std::make_shared<std::thread>(&ImageFileContainer::lookForImagesInFolderAndSubfolder, this);
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
  void waitForFinished()
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
    return FileInfo{};
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
          FileInfo fi(*i);
          if(fi.isSupported()) {
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

  std::string mWorkingDirectory;
  std::vector<FileInfo> mListOfImagePaths;
  bool mIsStopped = false;
  bool mIsRunning = false;
  std::shared_ptr<std::thread> mWorkerThread;
};

}    // namespace joda::helper
