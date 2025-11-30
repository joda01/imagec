///
/// \file      bioformats.hpp
/// \author    Joachim Danmayr
/// \date      2023-11-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <jni.h>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include "backend/helper/ome_parser/ome_info.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::image::reader {

class ImageReader
{
public:
  /////////////////////////////////////////////////////
  ImageReader(const std::filesystem::path &imageFileName);
  ~ImageReader();

  cv::Mat loadImageTile(joda::enums::PlaneId imagePlane, uint16_t series, uint16_t resolutionIdx, joda::ome::TileToLoad tile,
                        const joda::ome::OmeInfo &ome) const;
  cv::Mat loadEntireImage(joda::enums::PlaneId imagePlane, uint16_t series, uint16_t resolutionIdx, const joda::ome::OmeInfo &ome) const;

  cv::Mat loadThumbnail(joda::enums::PlaneId directory, uint16_t series, const joda::ome::OmeInfo &ome) const;

  auto getOmeInformation(uint16_t series, const ome::PhyiscalSize &defaultSettings) const -> joda::ome::OmeInfo;

  static void init(uint64_t reservedRamForVMInBytes);
  static void destroy();

  const std::filesystem::path &getImagePath() const
  {
    return mImagePath;
  }

private:
  /////////////////////////////////////////////////////
  cv::Mat loadImage();
  jobject mJavaImageReadObject;
  std::filesystem::path mImagePath;

  /////////////////////////////////////////////////////
  static void setPath();
  static cv::Mat convertImageToMat(JNIEnv *myEnv, const jbyteArray &readImg, int32_t imageWidth, int32_t imageHeight, int32_t bitDepth,
                                   int32_t rgbChannelCount, bool isInterleaved, bool isLittleEndian);
  static void convertImageToMat(cv::Mat &image, int32_t imageWidth, int32_t imageHeight, int32_t bitDepth, int32_t rgbChannelCount,
                                bool isInterleaved, bool isLittleEndian);

  static void bigEndianToLittleEndian(cv::Mat &inOut, uint32_t format);

  /////////////////////////////////////////////////////
  static inline std::mutex mReadMutex{};
  static inline JavaVMOption options[2];        /* Options array -- use options to set classpath */
  static inline JavaVMInitArgs initArgs;        /* Virtual Machine (VM) initialization structure, passed by*/
  static inline jobjectArray args;              /* The String[] itself */
  static inline JavaVM *myJVM        = nullptr; /* JavaVM pointer set by call to JNI_CreateJavaVM */
  static inline JNIEnv *myGlobEnv    = nullptr; /* JNIEnv pointer set by call to JNI_CreateJavaVM */
  static inline bool mJVMInitialised = false;

  static inline jclass mBioformatsClass;
  static inline jmethodID mConstructor;

  static inline jmethodID mGetImageProperties;
  static inline jmethodID mReadImage;
  static inline jmethodID mReadImageTile;
  static inline jmethodID mClose;
};
}    // namespace joda::image::reader
