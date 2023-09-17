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
#include <filesystem>
#include <memory>
#include <set>
#include <thread>
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
  [[nodiscard]] auto getFilesList() const -> const std::vector<std::string> &
  {
    return mListOfImagePaths;
  }

  ///
  /// \brief      Returns a file on a specific index
  /// \author     Joachim Danmayr
  ///
  [[nodiscard]] auto getFileAt(uint32_t idx) const -> std::string
  {
    if(idx < mListOfImagePaths.size()) {
      return mListOfImagePaths.at(idx);
    }
    return "";
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
      if(!is_directory(i->path())) {
        if(ALLOWED_EXTENSIONS.contains(i->path().extension().string())) {
          mListOfImagePaths.push_back(i->path().string());
        }
      }
      if(mIsStopped) {
        break;
      }
    }
    mIsRunning = false;
  }

  /////////////////////////////////////////////////////
  static inline const std::set<std::string> ALLOWED_EXTENSIONS = {".tif", ".tiff", ".btif", ".btiff", ".btf"};
  std::string mWorkingDirectory;
  std::vector<std::string> mListOfImagePaths;
  bool mIsStopped = false;
  bool mIsRunning = false;
  std::shared_ptr<std::thread> mWorkerThread;
};

}    // namespace joda::helper
