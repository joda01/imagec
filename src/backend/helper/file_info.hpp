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
/// \brief     A short description what happens here.
///

#pragma once

#include <filesystem>
#include <iostream>
#include <set>
#include <string>

///
/// \class      FileInfo
/// \author     Joachim Danmayr
/// \brief      List of supported file extensions
///
class FileInfo
{
public:
  virtual bool parseFile(const std::filesystem::directory_entry &path,
                         const std::set<std::string> &supportedFileFormats)
  {
    auto ext = path.path().extension().string();
    if(supportedFileFormats.contains(ext)) {
      mPath = path.path();
      return true;
    }
    return false;
  }

  [[nodiscard]] const std::filesystem::path &getFilePath() const
  {
    return mPath;
  }

  [[nodiscard]] std::string getFilename() const
  {
    std::string filename = mPath.filename().string();
    return filename;
  }

  // operator const std::filesystem::path &() const
  //{
  //   return mPath;
  // }

protected:
  /////////////////////////////////////////////////////
  std::filesystem::path mPath;
};
