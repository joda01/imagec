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
/// \brief     A short description what happens here.
///

#include "bioformats_loader.hpp"

#include <jni.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "backend/helper/duration_count/duration_count.h"
#include <nlohmann/json.hpp>
#include <opencv2/core/mat.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#ifndef JNI_CREATEVM
#define JNI_CREATEVM "JNI_CreateJavaVM"
#endif

namespace joda::image {

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
void BioformatsLoader::setPath()
{
#ifdef _WIN32
  std::string javaHome = "java\\jre_win";
  std::string javaBin  = javaHome + "\\bin";
  SetEnvironmentVariable(L"JAVA_HOME", ConvertToWideString(javaHome.data()));
  const char *path    = std::getenv("PATH");
  std::string newPath = javaBin + std::string(";") + path;
  SetEnvironmentVariable(L"PATH", ConvertToWideString(javaBin.c_str()));

#else
  std::string javaHome = "java/jre_win";
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
void BioformatsLoader::init()
{
  setPath();

  using myFunc = jint (*)(JavaVM * *pvm, void **penv, void *args);
  myFunc JNI_CreateJavaVM;

#ifdef _WIN32
  HINSTANCE jvmDll = LoadLibrary(TEXT("./java/jre_win/bin/server/jvm.dll"));
#else
  void *jvmDll     = dlopen("./java/jre_linux/lib/amd64/server/libjvm.so", RTLD_LAZY);
#endif

  if(jvmDll == NULL) {
    std::cerr << "Failed to load jvm lib" << std::endl;
  }

#ifdef _WIN32
  JNI_CreateJavaVM = reinterpret_cast<myFunc>(GetProcAddress(jvmDll, JNI_CREATEVM));
#else
  JNI_CreateJavaVM = reinterpret_cast<myFunc>(dlsym(jvmDll, JNI_CREATEVM));
#endif

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
        mGetImageProperties = myGlobEnv->GetStaticMethodID(mBioformatsClass, "getImageProperties",
                                                           "(Ljava/lang/String;II)Ljava/lang/String;");
        mReadImage          = myGlobEnv->GetStaticMethodID(mBioformatsClass, "readImage", "(Ljava/lang/String;II)[B");
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
void BioformatsLoader::destroy()
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
std::string BioformatsLoader::getJavaVersion()
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
cv::Mat BioformatsLoader::loadEntireImage(const std::string &filename, int directory, uint16_t series)
{
  // Takes 150 ms
  if(mJVMInitialised) {
    // std::lock_guard<std::mutex> lock(mReadMutex);

    JNIEnv *myEnv;
    myJVM->AttachCurrentThread((void **) &myEnv, NULL);
    jstring filePath = myEnv->NewStringUTF(filename.c_str());
    jstring result   = (jstring) myEnv->CallStaticObjectMethod(mBioformatsClass, mGetImageProperties, filePath, 0,
                                                               static_cast<int>(series));
    const char *stringChars = myEnv->GetStringUTFChars(result, NULL);

    joda::ome::OmeInfo omeInfo;
    omeInfo.loadOmeInformationFromXMLString(std::string(stringChars));    ///\todo this method can throw an excaption
    const auto &props = omeInfo.getImageInfo();

    cv::Mat retValue   = cv::Mat::zeros(props.imageHeight, props.imageWidth, CV_16UC1);
    jbyteArray readImg = (jbyteArray) myEnv->CallStaticObjectMethod(mBioformatsClass, mReadImage, filePath, directory,
                                                                    static_cast<int>(series));
    if(props.bits == 8) {
      myEnv->GetByteArrayRegion(readImg, 0, props.imageWidth * props.imageHeight, (jbyte *) retValue.data);
    } else if(props.bits == 16) {
      myEnv->GetByteArrayRegion(readImg, 0, props.imageWidth * props.imageHeight * 2, (jbyte *) retValue.data);
    }

    myEnv->DeleteLocalRef(filePath);
    myJVM->DetachCurrentThread();
    return retValue;
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
auto BioformatsLoader::getOmeInformation(const std::string &filename, uint16_t series) -> joda::ome::OmeInfo
{
  if(mJVMInitialised) {
    auto id = DurationCount::start("Get OEM");
    JNIEnv *myEnv;
    myJVM->AttachCurrentThread((void **) &myEnv, NULL);

    jstring filePath = myEnv->NewStringUTF(filename.c_str());
    jstring result   = (jstring) myEnv->CallStaticObjectMethod(mBioformatsClass, mGetImageProperties, filePath, 0,
                                                               static_cast<int>(series));
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

//     jsize imageArraySize = myEnv->GetArrayLength(readImg);
}    // namespace joda::image
