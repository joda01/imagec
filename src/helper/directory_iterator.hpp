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

#include <bits/types/FILE.h>
#include <algorithm>
#include <filesystem>
#include <set>
#include "helper.hpp"

namespace joda::helper {

using namespace std::filesystem;

class ImageFileContainer
{
public:
  ImageFileContainer()
  {
  }

  ///
  /// \brief      Find all images in the given infolder and its subfolder.
  /// \author     Joachim Danmayr
  ///
  inline void lookForImagesInFolderAndSubfolder(const std::string &inputFolder)
  {
    mIsStopped = false;
    mListOfImagePaths.clear();

    for(recursive_directory_iterator i(inputFolder), end; i != end; ++i) {
      if(!is_directory(i->path())) {
        if(ALLOWED_EXTENSIONS.contains(i->path().extension())) {
          mListOfImagePaths.push_back(i->path());
        }
      }
      if(mIsStopped) {
        break;
      }
    }
  }

  ///
  /// \brief      Stops looking for files
  /// \author     Joachim Danmayr
  ///
  void stop()
  {
    mIsStopped = true;
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
  [[nodiscard]] auto getFileAt(uint32_t idx) const -> const std::string
  {
    if(idx < mListOfImagePaths.size()) {
      return mListOfImagePaths.at(idx);
    } else {
      return "";
    }
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
  /////////////////////////////////////////////////////
  static inline const std::set<std::string> ALLOWED_EXTENSIONS = {".tif", ".tiff", ".btif", ".btiff", ".btf"};
  std::vector<std::string> mListOfImagePaths;
  bool mIsStopped = false;
};

}    // namespace joda::helper
