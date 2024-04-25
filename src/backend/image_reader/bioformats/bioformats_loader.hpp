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
#include <iostream>
#include <mutex>
#include <string>
#include "backend/helper/ome_parser/ome_info.hpp"
#include <opencv2/core/mat.hpp>

class BioformatsLoader
{
public:
  /////////////////////////////////////////////////////
  static cv::Mat loadEntireImage(const std::string &filename, int directory, uint16_t series);
  static auto getOmeInformation(const std::string &filename, uint16_t series)
      -> std::tuple<joda::ome::OmeInfo, ImageProperties>;
  static void init();
  static void destroy();
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
  static inline jmethodID mGetImageProperties;
  static inline jmethodID mReadImage;
};
