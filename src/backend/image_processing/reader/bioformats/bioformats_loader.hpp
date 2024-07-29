///
/// \file      bioformats.hpp
/// \author    Joachim Danmayr
/// \date      2023-11-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <jni.h>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <string>
#include "backend/helper/ome_parser/ome_info.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::image {

class BioformatsLoader
{
public:
  /////////////////////////////////////////////////////
  static cv::Mat loadImageTile(const std::string &filename, uint16_t directory, uint16_t series, int offset,
                               int nrOfTilesToRead, uint16_t resolutionIdx);
  static cv::Mat loadEntireImage(const std::string &filename, int directory, uint16_t series, uint16_t resolutionIdx);
  static auto getOmeInformation(const std::string &filename) -> joda::ome::OmeInfo;
  static void init();
  static void destroy();
  static auto calculateTileXYoffset(int32_t nrOfTilesToRead, int32_t offset, int64_t width, int64_t height,
                                    int64_t tilewidth, int64_t tileheight) -> std::tuple<int64_t, int64_t>;
  // static std::string getJavaVersion();

private:
  /////////////////////////////////////////////////////
  static void setPath();

  /////////////////////////////////////////////////////
  static inline std::mutex mReadMutex{};
  static inline JavaVMOption options[1];        /* Options array -- use options to set classpath */
  static inline JavaVMInitArgs initArgs;        /* Virtual Machine (VM) initialization structure, passed by*/
  static inline jobjectArray args;              /* The String[] itself */
  static inline JavaVM *myJVM        = nullptr; /* JavaVM pointer set by call to JNI_CreateJavaVM */
  static inline JNIEnv *myGlobEnv    = nullptr; /* JNIEnv pointer set by call to JNI_CreateJavaVM */
  static inline bool mJVMInitialised = false;

  static inline jclass mBioformatsClass;
  static inline jmethodID mGetImageInfo;
  static inline jmethodID mGetImageProperties;
  static inline jmethodID mReadImage;
  static inline jmethodID mReadImageTile;
};
}    // namespace joda::image
