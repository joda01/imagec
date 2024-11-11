///
/// \file      bioformats.cpp
/// \author    Joachim Danmayr
/// \date      2023-11-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "image_reader.hpp"
#include <jni.h>
#include <stdio.h>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "backend/commands/image_functions/resize/resize.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <nlohmann/json.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <dlfcn.h>
#include <cstdlib>
#else
#include <dlfcn.h>
#endif

#ifndef JNI_CREATEVM
#define JNI_CREATEVM "JNI_CreateJavaVM"
#endif

namespace joda::image::reader {

#ifdef _WIN32
wchar_t *ConvertToWideString(const char *narrowString)
{
  // Get the required buffer size
  int bufferSize = MultiByteToWideChar(CP_ACP, 0, narrowString, -1, NULL, 0);

  // Allocate a buffer
  wchar_t *wideString = new wchar_t[bufferSize];

  // Convert the narrow string to wide string
  MultiByteToWideChar(CP_ACP, 0, narrowString, -1, wideString, bufferSize);

  return wideString;
}
#endif

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ImageReader::setPath()
{
#ifdef _WIN32
  std::string javaHome = "java\\jre_win";
  std::string javaBin  = javaHome + "\\bin";
  SetEnvironmentVariable("JAVA_HOME", javaHome.data());
  const char *path    = std::getenv("PATH");
  std::string newPath = javaBin + std::string(";") + path;
  SetEnvironmentVariable("PATH", javaBin.c_str());
#elif defined(__APPLE__)
  std::string javaHome = "java/jre_macos_arm";
  std::string javaBin  = javaHome + "/bin";
  setenv("JAVA_HOME", javaHome.c_str(), 1);
  std::string path = std::getenv("PATH");
  path             = javaBin + ":" + path;
  setenv("PATH", path.c_str(), 1);
#else
  std::string javaHome = "java/jre_linux";
  std::string javaBin  = javaHome + "/bin";
  setenv("JAVA_HOME", javaHome.c_str(), 1);
  std::string path = std::getenv("PATH");
  path             = javaBin + ":" + path;
  setenv("PATH", path.c_str(), 1);
#endif
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ImageReader::init()
{
  setPath();

  using myFunc = jint (*)(JavaVM * *pvm, void **penv, void *args);
  myFunc JNI_CreateJavaVM;

#ifdef _WIN32
  HINSTANCE jvmDll = LoadLibrary(TEXT("./java/jre_win/bin/server/jvm.dll"));
#elif defined(__APPLE__)
  void *jvmDll     = dlopen("./java/jre_macos_arm/lib/server/libjvm.dylib", RTLD_LAZY);
#else
  void *jvmDll = dlopen("./java/jre_linux/lib/amd64/server/libjvm.so", RTLD_LAZY);
#endif

  if(jvmDll == NULL) {
    std::cerr << "Failed to load jvm lib" << std::endl;
    return;
  }

#ifdef _WIN32
  JNI_CreateJavaVM = reinterpret_cast<myFunc>(GetProcAddress(jvmDll, JNI_CREATEVM));
#else
  JNI_CreateJavaVM = reinterpret_cast<myFunc>(dlsym(jvmDll, JNI_CREATEVM));
#endif

  if(JNI_CreateJavaVM == nullptr) {
    std::cout << "Could not JNI_CreateJavaV" << std::endl;
    return;
  }

  try {
    /*  Set the version field of the initialization arguments for JNI v1.4. */
    initArgs.version = JNI_VERSION_1_8;

/* Now, you want to specify the directory for the class to run in the classpath.
 * with  Java2, classpath is passed in as an option.
 * Note: You must specify the directory name in UTF-8 format. So, you wrap
 *       blocks of code in #pragma convert statements.
 */
#ifdef _WIN32
    options[0].optionString = const_cast<char *>("-Djava.class.path=./;java/bioformats.jar;java");
#else
    options[0].optionString = const_cast<char *>("-Djava.class.path=./:java/bioformats.jar:java");
#endif

    initArgs.options  = options; /* Pass in the classpath that has been set up. */
    initArgs.nOptions = 1;       /* Pass in classpath and version options */

    /*  Create the JVM -- a nonzero return code indicates there was
     *  an error. Drop back into EBCDIC and write a message to stderr
     *  before exiting the program.
     *  Note:  This will run the default JVM and JDK which is 32bit JDK 6.0.
     *  If you want to run a different JVM and JDK, set the JAVA_HOME environment
     *  variable to the home directory of the JVM you want to use
     *  (prior to the CreateJavaVM() call).
     */
    if(JNI_CreateJavaVM(&myJVM, (void **) &myGlobEnv, &initArgs) != 0) {
      std::cout << "JAVA VM ERROR" << std::endl;
      mJVMInitialised = false;
    } else {
      mJVMInitialised = true;

      mBioformatsClass = myGlobEnv->FindClass("BioFormatsWrapper");
      if(mBioformatsClass == NULL) {
        if(myGlobEnv->ExceptionOccurred()) {
          myGlobEnv->ExceptionDescribe();
        }
        std::cout << "Error: Class not found!" << std::endl;
      } else {
        mGetImageProperties = myGlobEnv->GetStaticMethodID(mBioformatsClass, "getImageProperties", "(Ljava/lang/String;I)Ljava/lang/String;");
        mReadImage          = myGlobEnv->GetStaticMethodID(mBioformatsClass, "readImage", "(Ljava/lang/String;IIIII)[B");
        mReadImageTile      = myGlobEnv->GetStaticMethodID(mBioformatsClass, "readImageTile", "(Ljava/lang/String;IIIIIIIII)[B");
        mGetImageInfo       = myGlobEnv->GetStaticMethodID(mBioformatsClass, "readImageInfo", "(Ljava/lang/String;II)[I");
      }
    }
  } catch(const std::exception &ex) {
    std::cout << "JAVA VM ERROR: " << ex.what() << std::endl;
    mJVMInitialised = false;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ImageReader::destroy()
{
  if(nullptr != myJVM) {
    myJVM->DestroyJavaVM();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
/*
std::string ImageReader::getJavaVersion()
{
  jclass systemClass = myEnv->FindClass("java/lang/System");
  jmethodID getPropertyMethod =
      myEnv->GetStaticMethodID(systemClass, "getProperty", "(Ljava/lang/String;)Ljava/lang/String;");
  jstring propertyName = myEnv->NewStringUTF("java.version");
  jstring javaVersion =
      static_cast<jstring>(myEnv->CallStaticObjectMethod(systemClass, getPropertyMethod, propertyName));
  const char *javaVersionStr = myEnv->GetStringUTFChars(javaVersion, nullptr);
  myEnv->ReleaseStringUTFChars(javaVersion, javaVersionStr);
  return javaVersionStr;
}
*/

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
cv::Mat ImageReader::loadEntireImage(const std::string &filename, const Plane &imagePlane, uint16_t series, uint16_t resolutionIdx,
                                     const joda::ome::OmeInfo &ome)
{
  // Takes 150 ms
  if(myJVM != nullptr && mJVMInitialised && imagePlane.c >= 0 && imagePlane.z >= 0 && imagePlane.t >= 0) {
    // std::lock_guard<std::mutex> lock(mReadMutex);

    JNIEnv *myEnv;
    myJVM->AttachCurrentThread((void **) &myEnv, NULL);
    jstring filePath = myEnv->NewStringUTF(filename.c_str());

    //
    // ReadImage Info
    //
    int32_t imageHeight     = ome.getImageHeight(resolutionIdx);
    int32_t imageWidth      = ome.getImageWidth(resolutionIdx);
    int32_t bitDepth        = ome.getBitDepth(resolutionIdx);
    int32_t rgbChannelCount = ome.getRGBchannelCount(resolutionIdx);
    bool isInterleaved      = ome.getIsInterleaved(resolutionIdx);

    //
    // Load image
    //
    jbyteArray readImg  = (jbyteArray) myEnv->CallStaticObjectMethod(mBioformatsClass, mReadImage, filePath, static_cast<int>(series),
                                                                     static_cast<int>(resolutionIdx), imagePlane.z, imagePlane.c, imagePlane.t);
    cv::Mat loadedImage = convertImageToMat(myEnv, readImg, imageWidth, imageHeight, bitDepth, rgbChannelCount, isInterleaved);
    //
    // Cleanup
    //
    myEnv->DeleteLocalRef(filePath);
    myJVM->DetachCurrentThread();
    return loadedImage;
  }

  return cv::Mat{};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
cv::Mat ImageReader::loadThumbnail(const std::string &filename, const Plane &imagePlane, uint16_t series, const joda::ome::OmeInfo &ome)
{
  // Takes 150 ms
  if(nullptr != myJVM && mJVMInitialised && imagePlane.c >= 0 && imagePlane.z >= 0 && imagePlane.t >= 0) {
    // std::lock_guard<std::mutex> lock(mReadMutex);

    int32_t resolutionIdx = static_cast<int32_t>(ome.getResolutionCount().size()) - 1;
    auto resolution       = ome.getResolutionCount(series).at(resolutionIdx);

    if(resolution.imageMemoryUsage > 209715200) {
      joda::log::logWarning("Cannot create thumbnail. Pyramid to big: >" + std::to_string(resolution.imageMemoryUsage) + "< Bytes.");
      return cv::Mat{};
    }
    JNIEnv *myEnv;
    myJVM->AttachCurrentThread((void **) &myEnv, NULL);
    jstring filePath = myEnv->NewStringUTF(filename.c_str());

    //
    // ReadImage Info
    //
    int32_t imageHeight     = ome.getImageHeight(resolutionIdx);
    int32_t imageWidth      = ome.getImageWidth(resolutionIdx);
    int32_t bitDepth        = ome.getBitDepth(resolutionIdx);
    int32_t rgbChannelCount = ome.getRGBchannelCount(resolutionIdx);
    bool isInterleaved      = ome.getIsInterleaved(resolutionIdx);

    //
    // Read image
    //
    jbyteArray readImg    = (jbyteArray) myEnv->CallStaticObjectMethod(mBioformatsClass, mReadImage, filePath, static_cast<int>(series),
                                                                       static_cast<int>(resolutionIdx), imagePlane.z, imagePlane.c, imagePlane.t);
    jsize totalSizeLoaded = myEnv->GetArrayLength(readImg);

    // This is the image information
    cv::Mat loadedImage = convertImageToMat(myEnv, readImg, imageWidth, imageHeight, bitDepth, rgbChannelCount, isInterleaved);

    myEnv->DeleteLocalRef(filePath);
    myJVM->DetachCurrentThread();
    return cv::Mat(joda::image::func::Resizer::resizeWithAspectRatio(loadedImage, 256, 256));
  }

  return cv::Mat{};
}

///
/// \brief      Used to load (very) large TIFF images that cannot be loaded as a whole into RAM.
///             Prerequisite is that the TIF is saved as a tiled TIFF.
///             With this method it is possible to load tile by tile, and it is possible to load
///             more than one tile and create a composite image from these tiles.
///
///             Loads >nrOfTilesToRead< tiles of a tiled TIFF image starting at >offset<
///             The tiles are read in squares and assembled into an image.
///             Offset is the number of composite tiles to read.
///
///             If sqrt(nrOfTilesToRead) is not a multiplier of nrOfTilesX / nrOfTilesY
///             the image is padded with black "virtual" tiles.
///             Example shows an image with size 4x7 tiles and nrOfTilesToRead = 9
///             Numbers in brackets are padded part of the image
///
///
///         (6) +-----+-----+-----+-----+-----+-----+-----+-----+-----+
///             |(3)  |(3)  |(3)  |(4)  |(4)  |(4)  |(5)  |(5)  |(5)  |
///             |     |     |     |     |     |     |     |     |     |
///         (5) +-----+-----+-----+-----+-----+-----+-----+-----+-----+
///             |(3)  |(3)  |(3)  |(4)  |(4)  |(4)  |(5)  |(5)  |(5)  |
///             |     |     |     |     |     |     |     |     |     |
///           4 +-----+-----+-----+-----+-----+-----+-----+-----+-----+
///             |3    |3    |3    |4    |4    |4    |5    |(5)  |(5)  |
///             |     |     |     |     |     |     |     |     |     |
///           3 +-----+-----+-----+-----+-----+-----+-----+-----+-----+
///             |0    |0    |0    |1    |1    |1    |2    |(2)  |(2)  |
///             |     |     |     |     |     |     |     |     |     |
///           2 +-----+-----+-----+-----+-----+-----+-----+-----+-----+
///             |0    |0    |0    |1    |1    |1    |2    |(2)  |(2)  |
///             |     |     |     |     |     |     |     |     |     |
///           1 +-----+-----+-----+-----+-----+-----+-----+-----+-----+
///             |0    |0    |0    |1    |1    |1    |2    |(2)  |(2)  |
///             |     |     |     |     |     |     |     |     |     |
///           0 +-----+-----+-----+-----+-----+-----+-----+-----+-----+
///             0     1     2     3     4     5     6     (7)   (8)
///
/// \author     Joachim Danmayr
///
/// \param[in]  filename  Name of the file to load
/// \param[in]  directory  If it is multi document tiff, the index of
///                       the document which should be loaded
/// \param[in]  offset    Composite tile number to load
/// \param[in]  nrOfTilesToRead Nr of tiles which should form one composite image
/// \return Loaded composite image
///
cv::Mat ImageReader::loadImageTile(const std::string &filename, const Plane &imagePlane, uint16_t series, uint16_t resolutionIdx,
                                   const joda::ome::TileToLoad &tile, const joda::ome::OmeInfo &ome)
{
  if(nullptr != myJVM && mJVMInitialised && imagePlane.c >= 0 && imagePlane.z >= 0 && imagePlane.t >= 0) {
    JNIEnv *myEnv = nullptr;
    myJVM->AttachCurrentThread(reinterpret_cast<void **>(&myEnv), nullptr);
    jstring filePath = myEnv->NewStringUTF(filename.c_str());

    //
    // ReadImage Info
    //
    int32_t imageHeight     = ome.getImageHeight(resolutionIdx);
    int32_t imageWidth      = ome.getImageWidth(resolutionIdx);
    int32_t bitDepth        = ome.getBitDepth(resolutionIdx);
    int32_t rgbChannelCount = ome.getRGBchannelCount(resolutionIdx);
    bool isInterleaved      = ome.getIsInterleaved(resolutionIdx);

    //
    // Calculate tile position
    //
    int32_t offsetX          = tile.tileX * tile.tileWidth;
    int32_t offsetY          = tile.tileY * tile.tileHeight;
    int32_t tileWidthToLoad  = tile.tileWidth;
    int32_t tileHeightToLoad = tile.tileHeight;
    if(offsetX + tile.tileWidth > imageWidth) {
      tileWidthToLoad = tile.tileWidth - ((offsetX + tile.tileWidth) - imageWidth);
    }

    if(offsetY + tile.tileHeight > imageHeight) {
      tileHeightToLoad = tile.tileHeight - ((offsetY + tile.tileHeight) - imageHeight);
    }

    //
    // Load image
    //
    auto i1       = DurationCount::start("Load from filesystm");
    auto *readImg = (jbyteArray) myEnv->CallStaticObjectMethod(mBioformatsClass, mReadImageTile, filePath, static_cast<int>(series),
                                                               static_cast<int>(resolutionIdx), imagePlane.z, imagePlane.c, imagePlane.t, offsetX,
                                                               offsetY, tileWidthToLoad, tileHeightToLoad);

    DurationCount::stop(i1);
    //
    // Assign image data
    //
    cv::Mat image = convertImageToMat(myEnv, readImg, tileWidthToLoad, tileHeightToLoad, bitDepth, rgbChannelCount, isInterleaved);

    //
    // Cleanup
    //
    myEnv->DeleteLocalRef(filePath);
    myJVM->DetachCurrentThread();
    return image;
  }
  throw std::runtime_error("Could not open image!");
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto ImageReader::getOmeInformation(const std::filesystem::path &filename) -> joda::ome::OmeInfo
{
  const int32_t series = 0;
  if(nullptr != myJVM && mJVMInitialised) {
    auto id = DurationCount::start("Get OEM");
    JNIEnv *myEnv;
    myJVM->AttachCurrentThread((void **) &myEnv, NULL);

    jstring filePath = myEnv->NewStringUTF(filename.string().c_str());
    jstring result   = (jstring) myEnv->CallStaticObjectMethod(mBioformatsClass, mGetImageProperties, filePath, static_cast<int>(series));
    myEnv->DeleteLocalRef(filePath);
    const char *stringChars = myEnv->GetStringUTFChars(result, NULL);
    std::string omeXML(stringChars);
    myEnv->ReleaseStringUTFChars(result, stringChars);
    joda::ome::OmeInfo omeInfo;

    omeInfo.loadOmeInformationFromXMLString(omeXML);    ///\todo this method can throw an excaption

    myJVM->DetachCurrentThread();
    DurationCount::stop(id);
    return omeInfo;
  }
  return {};
}

cv::Mat ImageReader::convertImageToMat(JNIEnv *myEnv, const jbyteArray &readImg, int32_t imageWidth, int32_t imageHeight, int32_t bitDepth,
                                       int32_t rgbChannelCount, bool isInterleaved)
{
  //
  //
  //
  jsize totalSizeLoaded = myEnv->GetArrayLength(readImg);

  auto format       = CV_16UC1;
  int32_t widthTmp  = imageWidth;
  int32_t heightTmp = imageHeight;
  if(bitDepth == 16 && rgbChannelCount != 3) {
    format = CV_16UC1;
  } else if(bitDepth == 16 && rgbChannelCount != 3) {
    format = CV_8UC1;
  } else if(bitDepth == 8 && rgbChannelCount == 3 && isInterleaved) {
    format = CV_8UC3;
  } else if(bitDepth == 8 && rgbChannelCount == 3 && !isInterleaved) {
    format    = CV_8UC1;
    heightTmp = imageHeight * 3;
    widthTmp  = imageWidth;
  }

  cv::Mat image = cv::Mat::zeros(heightTmp, widthTmp, format);
  myEnv->GetByteArrayRegion(readImg, 0, totalSizeLoaded, (jbyte *) image.data);

  //
  // Handling of special formats
  //

  // 16 bit grayscale
  if(format == CV_16UC1) {
    return image;
  }

  // 8 bit grayscale
  if(format == CV_8UC1) {
    cv::Mat img16bit;
    image.convertTo(img16bit, CV_16U, 256);    // Scale 8-bit values (0–255) to 16-bit (0–65535)
  }

  // Interleaved RGB image
  if(format == CV_8UC3) {
    cvtColor(image, image, cv::COLOR_RGB2BGR);
    return image;
  }

  // Planar RGB image
  if(format == CV_8UC1 && rgbChannelCount == 3) {
    // Step 1: Split the non-interleaved matrix into separate R, G, B channels
    cv::Mat redChannel   = image(cv::Rect(0, 0, imageWidth, imageHeight));                  // First part (R)
    cv::Mat greenChannel = image(cv::Rect(0, imageHeight, imageWidth, imageHeight));        // Second part (G)
    cv::Mat blueChannel  = image(cv::Rect(0, 2 * imageHeight, imageWidth, imageHeight));    // Third part (B)

    // Step 2: Merge the separate channels into one BGR interleaved image
    std::vector<cv::Mat> channels = {blueChannel, greenChannel, redChannel};    // OpenCV uses BGR
    cv::Mat bgrImage              = cv::Mat::zeros(cv::Size(imageWidth, imageHeight), CV_8UC3);
    cv::merge(channels, bgrImage);
    return bgrImage;
  }

  throw std::invalid_argument("Not supported image format!");
}

//     jsize imageArraySize = myEnv->GetArrayLength(readImg);
}    // namespace joda::image::reader
