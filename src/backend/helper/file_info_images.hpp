

///
/// \file      file_info.hpp
/// \author    Joachim Danmayr
/// \date      2023-11-05
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <filesystem>
#include <set>
#include <string>
#include "file_info.hpp"

namespace joda::helper::fs {

///
/// \class      FileInfo
/// \author     Joachim Danmayr
/// \brief      List of supported file extensions
///
class FileInfoImages : public FileInfo
{
public:
  using FileInfo::FileInfo;

  bool parseFile(const std::filesystem::directory_entry &path,
                 const std::set<std::string> & /*supportedFileFormats*/) override
  {
    auto ext = path.path().extension().string();
    if(EXTENSIONS.contains(ext)) {
      mPath = path.path();
      return true;
    }

    return false;
  }

private:
  /////////////////////////////////////////////////////
  static inline const std::set<std::string> EXTENSIONS = {".tif", ".tiff", ".btif", ".btiff", ".btf",
                                                          ".jpg", ".jpeg", ".vsi",  ".ics",   ".czi"};
};
}    // namespace joda::helper::fs
