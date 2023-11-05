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

///
/// \class      FileInfo
/// \author     Joachim Danmayr
/// \brief      List of supported file extensions
///
class FileInfo
{
public:
  enum class Decoder
  {
    UNSUPPORTED,
    TIFF,
    JPG,
    BIOFORMATS
  };
  FileInfo()
  {
  }
  FileInfo(const std::filesystem::directory_entry &path)
  {
    parseFile(path);
  }

  FileInfo(const std::string &path)
  {
    std::filesystem::directory_entry pathObj(path);
    parseFile(pathObj);
  }

  [[nodiscard]] Decoder getDecoder() const
  {
    return mDecoder;
  }

  [[nodiscard]] bool isSupported() const
  {
    return mDecoder != Decoder::UNSUPPORTED;
  }

  [[nodiscard]] const std::string &getPath() const
  {
    return mPath;
  }

  operator const std::string &() const
  {
    return mPath;
  }

private:
  /////////////////////////////////////////////////////
  static inline const std::set<std::string> JPG_EXTENSIONS        = {".jpg", ".jpeg"};
  static inline const std::set<std::string> TIF_EXTENSIONS        = {".tif", ".tiff", ".btif", ".btiff", ".btf"};
  static inline const std::set<std::string> BIOFORMATS_EXTENSIONS = {".vsi"};

  std::string mPath;
  Decoder mDecoder = Decoder::UNSUPPORTED;

  void parseFile(const std::filesystem::directory_entry &path)
  {
    auto ext = path.path().extension().string();
    if(TIF_EXTENSIONS.contains(ext)) {
      mDecoder = Decoder::TIFF;
      mPath    = path.path().string();
    } else if(BIOFORMATS_EXTENSIONS.contains(ext)) {
      mDecoder = Decoder::BIOFORMATS;
      mPath    = path.path().string();

    } else {
      mPath    = "";
      mDecoder = Decoder::UNSUPPORTED;
    }
  }
};
