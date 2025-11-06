///
/// \file      bioformats.cpp
/// \author    Joachim Danmayr
/// \date      2023-11-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "image_reader.hpp"
#include <jni.h>
#include <stdio.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
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
#include <libgen.h>
#include <mach-o/dyld.h>
#else
#include <dlfcn.h>
#endif

#ifndef JNI_CREATEVM
#define JNI_CREATEVM "JNI_CreateJavaVM"
#endif

namespace joda::image::reader {

#ifdef __APPLE__
std::string getAppContentsPath()
{
  char buf[PATH_MAX];
  uint32_t size = sizeof(buf);
  if(_NSGetExecutablePath(buf, &size) != 0) {
    throw std::runtime_error("Executable path buffer too small");
  }
  // If inside .app bundle: MyApp.app/Contents/MacOS/imagec
  auto baseDir = std::filesystem::canonical(buf);
  // If inside .app bundle: MyApp.app/Contents
  return baseDir.parent_path().parent_path();
}
#endif

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
  std::string contentsPath = getAppContentsPath();
  std::string javaHome     = contentsPath + "/Java/jre_macos_arm";
  std::string javaBin      = javaHome + "/bin";
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

  joda::log::logInfo("Java HOME: " + javaHome);
  joda::log::logInfo("Java BIN: " + javaBin);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ImageReader::init(uint64_t reservedRamForVMInBytes)
{
  setPath();

  using myFunc = jint (*)(JavaVM * *pvm, void **penv, void *args);
  myFunc JNI_CreateJavaVM;

#ifdef _WIN32
  HINSTANCE jvmDll = LoadLibrary(TEXT("./java/jre_win/bin/server/jvm.dll"));
#elif defined(__APPLE__)
  std::string contentsPath = getAppContentsPath();
  std::string jvmLibPath   = contentsPath + "/Java/jre_macos_arm/lib/server/libjvm.dylib";
  joda::log::logInfo("Java DLL: " + jvmLibPath);
  void *jvmDll     = dlopen((const char *) jvmLibPath.c_str(), RTLD_LAZY);
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
#elif defined(__APPLE__)
    std::string jarPath        = contentsPath + "/Java/bioformats.jar";
    std::string otherClassPath = contentsPath + "/Java";
    std::string classPath      = "-Djava.class.path=./:" + jarPath + ":java:" + otherClassPath;
    options[0].optionString    = const_cast<char *>(classPath.c_str());
#else
    options[0].optionString = const_cast<char *>("-Djava.class.path=./:java/bioformats.jar:java");
#endif
    int32_t jvmRam          = static_cast<int32_t>(std::ceil(static_cast<double>(reservedRamForVMInBytes) / 1000000.0));
    std::string ramReserved = "-Xmx" + std::to_string(jvmRam) + "m";
    options[1].optionString = const_cast<char *>(ramReserved.data());

    initArgs.options  = options; /* Pass in the classpath that has been set up. */
    initArgs.nOptions = 2;       /* Pass in classpath and version options */

    /*  Create the JVM -- a nonzero return code indicates there was
     *  an error. Drop back into EBCDIC and write a message to stderr
     *  before exiting the program.
     *  Note:  This will run the default JVM and JDK which is 32bit JDK 6.0.
     *  If you want to run a different JVM and JDK, set the JAVA_HOME environment
     *  variable to the home directory of the JVM you want to use
     *  (prior to the CreateJavaVM() call).
     */
    if(JNI_CreateJavaVM(&myJVM, reinterpret_cast<void **>(&myGlobEnv), &initArgs) != 0) {
      std::cout << "JAVA VM ERROR" << std::endl;
      mJVMInitialised = false;
    } else {
      jclass localCls = myGlobEnv->FindClass("BioFormatsWrapper");
      if(localCls == nullptr) {
        if(myGlobEnv->ExceptionOccurred() != nullptr) {
          myGlobEnv->ExceptionDescribe();
        }
        joda::log::logError("Could not found BioFormats class!");
        exit(7);
      } else {
        mBioformatsClass = static_cast<jclass>(myGlobEnv->NewGlobalRef(localCls));
        myGlobEnv->DeleteLocalRef(localCls);
        mGetImageProperties = myGlobEnv->GetStaticMethodID(mBioformatsClass, "getImageProperties", "(Ljava/lang/String;I)Ljava/lang/String;");
        mReadImage          = myGlobEnv->GetStaticMethodID(mBioformatsClass, "readImage", "(Ljava/lang/String;IIIII)[B");
        mReadImageTile      = myGlobEnv->GetStaticMethodID(mBioformatsClass, "readImageTile", "(Ljava/lang/String;IIIIIIIII)[B");
        mGetImageInfo       = myGlobEnv->GetStaticMethodID(mBioformatsClass, "readImageInfo", "(Ljava/lang/String;II)[I");
        joda::log::logTrace("JVM initialized!");
        mJVMInitialised = true;
      }
    }
  } catch(const std::exception &ex) {
    joda::log::logError("JAVA VM ERROR " + std::string(ex.what()));
    exit(7);
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
cv::Mat ImageReader::loadEntireImage(const std::string &filename, const joda::enums::PlaneId &imagePlane, uint16_t series, uint16_t resolutionIdx,
                                     const joda::ome::OmeInfo &ome)
{
  // Takes 150 ms
  if(myJVM != nullptr && mJVMInitialised && imagePlane.cStack >= 0 && imagePlane.zStack >= 0 && imagePlane.tStack >= 0) {
    // std::lock_guard<std::mutex> lock(mReadMutex);

    JNIEnv *myEnv;
    myJVM->AttachCurrentThread(reinterpret_cast<void **>(&myEnv), nullptr);
    jstring filePath = myEnv->NewStringUTF(filename.c_str());

    //
    // ReadImage Info
    //
    int32_t imageHeight     = ome.getImageHeight(series, resolutionIdx);
    int32_t imageWidth      = ome.getImageWidth(series, resolutionIdx);
    int32_t bitDepth        = ome.getBitDepth(series, resolutionIdx);
    int32_t rgbChannelCount = ome.getRGBchannelCount(series, resolutionIdx);
    bool isInterleaved      = ome.getIsInterleaved(series, resolutionIdx);
    bool isLittleEndian     = ome.getIsLittleEndian(series, resolutionIdx);

    //
    // Load image
    //
    if(series >= ome.getNrOfSeries()) {
      series = static_cast<uint16_t>(ome.getNrOfSeries() - 1);
    }
    jbyteArray readImg = static_cast<jbyteArray>(myEnv->CallStaticObjectMethod(mBioformatsClass, mReadImage, filePath, static_cast<int>(series),
                                                                               static_cast<int>(resolutionIdx), imagePlane.zStack, imagePlane.cStack,
                                                                               imagePlane.tStack));
    bool exception     = false;
    if(myEnv->ExceptionCheck() != 0u) {
      myEnv->ExceptionDescribe();
      myEnv->ExceptionClear();
      exception = true;
    }

    if(readImg == nullptr || exception) {
      myEnv->DeleteLocalRef(filePath);
      myJVM->DetachCurrentThread();
      joda::log::logError("Cannot load image tile info for >" + filename + "<, nullptr in result!");
      return {};
    }

    cv::Mat loadedImage = convertImageToMat(myEnv, readImg, imageWidth, imageHeight, bitDepth, rgbChannelCount, isInterleaved, isLittleEndian);
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
cv::Mat ImageReader::loadThumbnail(const std::string &filename, joda::enums::PlaneId imagePlane, uint16_t series, const joda::ome::OmeInfo &ome)
{
  const int32_t THUMBNAIL_SIZE = 1024;

  // Takes 150 ms
  if(nullptr != myJVM && mJVMInitialised && imagePlane.cStack >= 0 && imagePlane.zStack >= 0 && imagePlane.tStack >= 0) {
    // std::lock_guard<std::mutex> lock(mReadMutex);
    if(series >= ome.getNrOfSeries()) {
      series = static_cast<uint16_t>(ome.getNrOfSeries() - 1);
    }
    // Find the pyramid with the best matching resolution for thumbnail creation

    auto lookForSmallestImage = [&ome](int32_t seriesIn) -> std::tuple<int32_t, ome::OmeInfo::ImageInfo::Pyramid> {
      int32_t resolutionIdx                       = 0;
      ome::OmeInfo::ImageInfo::Pyramid resolution = ome.getResolutionCount(seriesIn).at(0);
      for(int idx = 0; idx < static_cast<int>(ome.getResolutionCount(seriesIn).size()); idx++) {
        resolution = ome.getResolutionCount(seriesIn).at(idx);
        if(resolution.imageWidth <= THUMBNAIL_SIZE || resolution.imageHeight <= THUMBNAIL_SIZE) {
          resolutionIdx = idx;
          break;
        }
      }
      return {resolutionIdx, resolution};
    };

    int32_t resolutionIdx = 0;
    uint16_t nrOfSeries   = static_cast<uint16_t>(ome.getNrOfSeries());
    ome::OmeInfo::ImageInfo::Pyramid resolution;
    // Look for a possible thumbnail
    for(; series < nrOfSeries; series++) {
      auto [resolutionIdxTmp, resolutionTmp] = lookForSmallestImage(series);
      if(resolutionTmp.imageMemoryUsage < 838860800) {
        resolution    = resolutionTmp;
        resolutionIdx = resolutionIdxTmp;
        break;
      }
    }

    /// \todo Make preview size configurable
    if(resolution.imageMemoryUsage > 838860800) {
      joda::log::logWarning("Cannot create thumbnail. Pyramid too big: >" + std::to_string(resolution.imageMemoryUsage) + "< Bytes.");
      return cv::Mat{};
    }
    JNIEnv *myEnv;
    myJVM->AttachCurrentThread(reinterpret_cast<void **>(&myEnv), nullptr);
    jstring filePath = myEnv->NewStringUTF(filename.c_str());

    //
    // ReadImage Info
    //
    int32_t imageHeight     = ome.getImageHeight(series, resolutionIdx);
    int32_t imageWidth      = ome.getImageWidth(series, resolutionIdx);
    int32_t bitDepth        = ome.getBitDepth(series, resolutionIdx);
    int32_t rgbChannelCount = ome.getRGBchannelCount(series, resolutionIdx);
    bool isInterleaved      = ome.getIsInterleaved(series, resolutionIdx);
    bool isLittleEndian     = ome.getIsLittleEndian(series, resolutionIdx);

    //
    // Check if channel exists
    //
    imagePlane.cStack = std::min(imagePlane.cStack, ome.getNrOfChannels(series) - 1);
    imagePlane.tStack = std::min(imagePlane.tStack, ome.getNrOfTStack(series) - 1);
    imagePlane.zStack = std::min(imagePlane.zStack, ome.getNrOfZStack(series) - 1);

    //
    // Read image
    //
    jbyteArray readImg = static_cast<jbyteArray>(myEnv->CallStaticObjectMethod(mBioformatsClass, mReadImage, filePath, static_cast<int>(series),
                                                                               static_cast<int>(resolutionIdx), imagePlane.zStack, imagePlane.cStack,
                                                                               imagePlane.tStack));
    bool exception     = false;
    if(myEnv->ExceptionCheck() != 0u) {
      myEnv->ExceptionDescribe();
      myEnv->ExceptionClear();
      exception = true;
    }

    if(readImg == nullptr || exception) {
      myEnv->DeleteLocalRef(filePath);
      myJVM->DetachCurrentThread();
      joda::log::logError("Cannot load image tile info for >" + filename + "<, nullptr in result!");
      return {};
    }

    // jsize totalSizeLoaded = myEnv->GetArrayLength(readImg);

    // This is the image information
    cv::Mat loadedImage = convertImageToMat(myEnv, readImg, imageWidth, imageHeight, bitDepth, rgbChannelCount, isInterleaved, isLittleEndian);

    myEnv->DeleteLocalRef(filePath);
    myJVM->DetachCurrentThread();
    return (joda::image::func::Resizer::resizeWithAspectRatio(loadedImage, THUMBNAIL_SIZE, THUMBNAIL_SIZE));
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
cv::Mat ImageReader::loadImageTile(const std::string &filename, const joda::enums::PlaneId &imagePlane, uint16_t series, uint16_t resolutionIdx,
                                   const joda::ome::TileToLoad &tile, const joda::ome::OmeInfo &ome)
{
  if(nullptr != myJVM && mJVMInitialised && imagePlane.cStack >= 0 && imagePlane.zStack >= 0 && imagePlane.tStack >= 0) {
    JNIEnv *myEnv = nullptr;
    myJVM->AttachCurrentThread(reinterpret_cast<void **>(&myEnv), nullptr);
    jstring filePath = myEnv->NewStringUTF(filename.c_str());

    //
    // ReadImage Info
    //
    int32_t imageHeight     = ome.getImageHeight(series, resolutionIdx);
    int32_t imageWidth      = ome.getImageWidth(series, resolutionIdx);
    int32_t bitDepth        = ome.getBitDepth(series, resolutionIdx);
    int32_t rgbChannelCount = ome.getRGBchannelCount(series, resolutionIdx);
    bool isInterleaved      = ome.getIsInterleaved(series, resolutionIdx);
    bool isLittleEndian     = ome.getIsLittleEndian(series, resolutionIdx);

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
    if(series >= ome.getNrOfSeries()) {
      series = static_cast<uint16_t>(ome.getNrOfSeries() - 1);
    }
    auto i1            = DurationCount::start("Load from filesystem");
    jbyteArray readImg = static_cast<jbyteArray>(
        myEnv->CallStaticObjectMethod(mBioformatsClass, mReadImageTile, filePath, static_cast<int>(series), static_cast<int>(resolutionIdx),
                                      imagePlane.zStack, imagePlane.cStack, imagePlane.tStack, offsetX, offsetY, tileWidthToLoad, tileHeightToLoad));

    bool exception = false;
    if(myEnv->ExceptionCheck() != 0u) {
      myEnv->ExceptionDescribe();
      myEnv->ExceptionClear();
      exception = true;
    }
    DurationCount::stop(i1);
    if(readImg == nullptr || exception) {
      myEnv->DeleteLocalRef(filePath);
      myJVM->DetachCurrentThread();
      joda::log::logError("Cannot load image tile info for >" + filename + "<, nullptr in result!");
      return {};
    }
    //
    // Assign image data
    //
    cv::Mat image = convertImageToMat(myEnv, readImg, tileWidthToLoad, tileHeightToLoad, bitDepth, rgbChannelCount, isInterleaved, isLittleEndian);

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
auto ImageReader::getOmeInformation(const std::filesystem::path &filename, uint16_t series, const ome::PhyiscalSize &defaultSettings)
    -> joda::ome::OmeInfo
{
  if(nullptr != myJVM && mJVMInitialised) {
    auto id = DurationCount::start("Get OME for " + filename.string());
    JNIEnv *myEnv;
    myJVM->AttachCurrentThread(reinterpret_cast<void **>(&myEnv), nullptr);
    if(!std::filesystem::exists(filename)) {
      joda::log::logError("File >" + filename.string() + "<, does not exist!");
      return {};
    }

    jstring filePath = myEnv->NewStringUTF(filename.string().c_str());
    jstring result   = static_cast<jstring>(myEnv->CallStaticObjectMethod(mBioformatsClass, mGetImageProperties, filePath, static_cast<int>(series)));
    bool exception   = false;
    if(myEnv->ExceptionCheck() != 0u) {
      myEnv->ExceptionDescribe();
      myEnv->ExceptionClear();
      exception = true;
    }

    myEnv->DeleteLocalRef(filePath);
    if(result == nullptr || exception) {
      myJVM->DetachCurrentThread();
      DurationCount::stop(id);
      joda::log::logError("Cannot load OME info for >" + filename.string() + "<, nullptr in result!");
      return {};
    }

    const char *stringChars = myEnv->GetStringUTFChars(result, nullptr);
    std::string omeXML(stringChars);
    myEnv->ReleaseStringUTFChars(result, stringChars);

    // Parse ome
    try {
      joda::ome::OmeInfo omeInfo;
      omeInfo.loadOmeInformationFromXMLString(omeXML, defaultSettings);    ///\todo this method can throw an excaption

      myJVM->DetachCurrentThread();
      DurationCount::stop(id);
      return omeInfo;
    } catch(...) {
      return {};
    }
  }
  return {};
}

cv::Mat ImageReader::convertImageToMat(JNIEnv *myEnv, const jbyteArray &readImg, int32_t imageWidth, int32_t imageHeight, int32_t bitDepth,
                                       int32_t rgbChannelCount, bool isInterleaved, bool isLittleEndian)
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
  } else if(bitDepth == 8 && rgbChannelCount == 4 && isInterleaved) {
    format = CV_8UC4;
  } else if(bitDepth == 8 && rgbChannelCount == 3 && !isInterleaved) {
    format    = CV_8UC1;
    heightTmp = imageHeight * 3;
    widthTmp  = imageWidth;
  } else if(bitDepth == 8 && rgbChannelCount == 4 && !isInterleaved) {
    format    = CV_8UC1;
    heightTmp = imageHeight * 4;
    widthTmp  = imageWidth;
  } else if(bitDepth == 8 && rgbChannelCount == 1 && !isInterleaved) {
    format    = CV_8UC1;
    heightTmp = imageHeight;
    widthTmp  = imageWidth;
  }

  cv::Mat image = cv::Mat::zeros(heightTmp, widthTmp, format);
  myEnv->GetByteArrayRegion(readImg, 0, totalSizeLoaded, reinterpret_cast<jbyte *>(image.data));
  if(!isLittleEndian) {
    bigEndianToLittleEndian(image, static_cast<uint32_t>(format));
  }

  //
  // Handling of special formats
  //
  // 16 bit grayscale
  if(format == CV_16UC1) {
    return image;
  }

  // 8 bit grayscale interleaved
  if(format == CV_8UC1 && rgbChannelCount == 1 && !isInterleaved) {
    cv::Mat img16bit;
    image.convertTo(img16bit, CV_16U, 256);    // Scale 8-bit values (0–255) to 16-bit (0–65535)
    return img16bit;
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

  // Interleaved RGB image
  if(format == CV_8UC4) {
    cvtColor(image, image, cv::COLOR_RGBA2BGR);
    return image;
  }

  // Planar RGB image
  if(format == CV_8UC1 && rgbChannelCount >= 3) {
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

void ImageReader::bigEndianToLittleEndian(cv::Mat &inOut, uint32_t format)
{
  // 16 bit grayscale
  if(format == CV_16UC1) {
    for(size_t p = 0; p < inOut.total(); p++) {
      uint16_t tmp                                = inOut.at<uint16_t>(static_cast<int32_t>(p));
      inOut.at<uint16_t>(static_cast<int32_t>(p)) = static_cast<uint16_t>((tmp >> 8) | (tmp << 8));
    }
  }
}

//     jsize imageArraySize = myEnv->GetArrayLength(readImg);
}    // namespace joda::image::reader
