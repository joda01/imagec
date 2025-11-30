///
/// \file      directory_iterator.cüü
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

#include "directory_iterator.hpp"
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>
#include "backend/helper/logger/console_logger.hpp"

namespace joda::filesystem {

DirectoryWatcher::DirectoryWatcher(const std::set<std::string> &supportedFileFormats) : mSupportedFormats(supportedFileFormats)
{
}

///
/// \brief      Find all images in the given infolder and its subfolder.
/// \author     Joachim Danmayr
///
void DirectoryWatcher::setWorkingDirectory(const std::filesystem::path &inputFolder)
{
  if(mWorkingDirectory != inputFolder) {
    mWorkingDirectory = inputFolder;
  }
}

///
/// \brief      Add file
/// \author     Joachim Danmayr
///
void DirectoryWatcher::lookForImages()
{
  if(mWorkingDirectory.empty()) {
    mListOfImagePaths.clear();
    for(const auto &callback : mCallbacks) {
      callback(State::FINISHED);
    }
  } else {
    stop();
    mWorkerThread = std::make_unique<std::thread>(&DirectoryWatcher::lookForImagesInFolderAndSubfolder, this);
  }
}

///
/// \brief      Add file
/// \author     Joachim Danmayr
///
void DirectoryWatcher::addFile(const std::filesystem::path &file)
{
  try {
    if(!is_directory(file)) {
      auto supported = parseFile(file);
      if(supported) {
        mListOfImagePaths.push_back(file);
      }
    }
  } catch(const std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }
}

///
/// \brief      Find all images in the given infolder and its subfolder.
/// \author     Joachim Danmayr
///
void DirectoryWatcher::lookForImagesInFolderAndSubfolder()
{
  mIsRunning = true;
  mIsStopped = false;
  for(const auto &callback : mCallbacks) {
    callback(State::RUNNING);
  }

  mListOfImagePaths.clear();
  if(!mWorkingDirectory.empty() && std::filesystem::exists(mWorkingDirectory)) {
    try {
      for(recursive_directory_iterator i(mWorkingDirectory), end; i != end; ++i) {
        addFile(i->path());
        if(mIsStopped) {
          break;
        }
      }
    } catch(const std::exception &ex) {
      joda::log::logError("File iterator: " + std::string(ex.what()));
    }
  }
  mIsRunning = false;
  for(const auto &callback : mCallbacks) {
    callback(State::FINISHED);
  }

  (void) mWorkerThread.release();
}

}    // namespace joda::filesystem
