///
/// \file      directory_iterator.cüü
/// \author    Joachim Danmayr
/// \date      2023-06-28
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "directory_iterator.hpp"

namespace joda::filesystem {

DirectoryWatcher::DirectoryWatcher(const std::set<std::string> &supportedFileFormats) :
    mSupportedFormats(supportedFileFormats)
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
    if(inputFolder.empty()) {
      mListOfImagePaths.clear();
    } else {
      stop();
      mWorkerThread = std::make_unique<std::thread>(&DirectoryWatcher::lookForImagesInFolderAndSubfolder, this);
    }
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
  mListOfImagePaths.clear();
  for(const auto &callback : mCallbacks) {
    callback(State::RUNNING);
  }

  for(recursive_directory_iterator i(mWorkingDirectory), end; i != end; ++i) {
    try {
      if(!is_directory(i->path())) {
        FileInfo fi;
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
  for(const auto &callback : mCallbacks) {
    callback(State::FINISHED);
  }

  (void) mWorkerThread.release();
}

}    // namespace joda::filesystem
