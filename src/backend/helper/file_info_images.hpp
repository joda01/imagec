

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

  enum class Decoder
  {
    UNSUPPORTED,
    TIFF,
    JPG,
    BIOFORMATS
  };

  [[nodiscard]] Decoder getDecoder() const
  {
    return mDecoder;
  }

  bool parseFile(const std::filesystem::directory_entry &path,
                 const std::set<std::string> & /*supportedFileFormats*/) override
  {
    auto ext = path.path().extension().string();
    if(TIF_EXTENSIONS.contains(ext)) {
      mDecoder = Decoder::TIFF;
      mPath    = path.path();
      return true;
    }
    if(BIOFORMATS_EXTENSIONS.contains(ext)) {
      mDecoder = Decoder::BIOFORMATS;
      mPath    = path.path();
      return true;
    }
    mDecoder = Decoder::UNSUPPORTED;
    return false;
  }

private:
  /////////////////////////////////////////////////////
  static inline const std::set<std::string> JPG_EXTENSIONS        = {".jpg", ".jpeg"};
  static inline const std::set<std::string> TIF_EXTENSIONS        = {".tif", ".tiff", ".btif", ".btiff", ".btf"};
  static inline const std::set<std::string> BIOFORMATS_EXTENSIONS = {".vsi", ".ics", ".czi"};

  Decoder mDecoder = Decoder::UNSUPPORTED;
};
}    // namespace joda::helper::fs
