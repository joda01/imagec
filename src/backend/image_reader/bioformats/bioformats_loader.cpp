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
#include "backend/image_reader/image_reader.hpp"
#include <opencv2/core/mat.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#ifndef JNI_CREATEVM
#define JNI_CREATEVM "JNI_CreateJavaVM"
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
  SetEnvironmentVariable("JAVA_HOME", javaHome.data());
  const char *path    = std::getenv("PATH");
  std::string newPath = javaBin + std::string(";") + path;
  SetEnvironmentVariable("PATH", javaBin.c_str());

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
  JNI_CreateJavaVM = reinterpret_cast<myFunc>(GetProcAddress(jvmDll, TEXT(JNI_CREATEVM)));
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
    if(JNI_CreateJavaVM(&myJVM, (void **) &myEnv, &initArgs) != 0) {
      std::cout << "JAVA VM ERROR" << std::endl;
      mJVMInitialised = false;
    } else {
      mJVMInitialised = true;
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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
cv::Mat BioformatsLoader::loadEntireImage(const std::string &filename, int directory, uint16_t series)
{
  if(mJVMInitialised) {
    std::lock_guard<std::mutex> lock(mReadMutex);

    myJVM->AttachCurrentThread((void **) &myEnv, NULL);

    // Call the BioFormatsWrapper.readImageBytes() method
    jclass cls = myEnv->FindClass("BioFormatsWrapper");
    if(cls == NULL) {
      if(myEnv->ExceptionOccurred()) {
        myEnv->ExceptionDescribe();
      }
      std::cout << "Error: Class not found!" << std::endl;
    } else {
      //
      // Read image
      //
      jmethodID mid =
          myEnv->GetStaticMethodID(cls, "readImage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)[[I");
      jstring filePath     = myEnv->NewStringUTF(filename.c_str());
      jstring directoryStr = myEnv->NewStringUTF(std::to_string(directory).c_str());
      jstring seriesStr    = myEnv->NewStringUTF(std::to_string(series).c_str());

      jobjectArray result = (jobjectArray) myEnv->CallStaticObjectMethod(cls, mid, filePath, directoryStr, seriesStr);
      myEnv->DeleteLocalRef(filePath);

      jsize rows = myEnv->GetArrayLength(result);

      jintArray row = (jintArray) myEnv->GetObjectArrayElement(result, 0);
      jsize cols    = myEnv->GetArrayLength(row);

      jint *data;

      cv::Mat retValue = cv::Mat::zeros(cols, rows, CV_16UC1);
      for(int i = 0; i < rows; i++) {
        row  = (jintArray) myEnv->GetObjectArrayElement(result, i);
        cols = myEnv->GetArrayLength(row);
        data = myEnv->GetIntArrayElements(row, nullptr);
        for(int j = 0; j < cols; j++) {
          retValue.at<uint16_t>(j, i) = (uint16_t) (data[j] & 0xFFFF);
        }
        myEnv->ReleaseIntArrayElements(row, data, JNI_ABORT);
      }

      myJVM->DetachCurrentThread();
      // Clean up
      // myEnv->ReleaseObjectArrayElements(result, bytes, 0);

      return retValue;
    }
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
auto BioformatsLoader::getOmeInformation(const std::string &filename, uint16_t series)
    -> std::tuple<joda::ome::OmeInfo, ImageProperties>
{
  if(mJVMInitialised) {
    std::lock_guard<std::mutex> lock(mReadMutex);
    myJVM->AttachCurrentThread((void **) &myEnv, NULL);
    // Call the BioFormatsWrapper.readImageBytes() method
    jclass cls = myEnv->FindClass("BioFormatsWrapper");
    if(cls == NULL) {
      // Print diagnostic information
      printf("Error: Class not found!\n");
      myEnv->ExceptionDescribe();    // Print more information about the exception
    }

    jmethodID mid = myEnv->GetStaticMethodID(
        cls, "getImageProperties", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
    jstring filePath     = myEnv->NewStringUTF(filename.c_str());
    jstring directoryStr = myEnv->NewStringUTF(std::to_string(0).c_str());
    jstring seriesStr    = myEnv->NewStringUTF(std::to_string(series).c_str());
    jstring result       = (jstring) myEnv->CallStaticObjectMethod(cls, mid, filePath, directoryStr, seriesStr);
    myEnv->DeleteLocalRef(filePath);
    // Convert the returned byte array to C++ bytes
    const char *stringChars = myEnv->GetStringUTFChars(result, NULL);
    std::string jsonResult(stringChars);
    myEnv->ReleaseStringUTFChars(result, stringChars);
    joda::ome::OmeInfo omeInfo;
    ImageProperties props = omeInfo.loadOmeInformationFromJsonString(jsonResult);
    myJVM->DetachCurrentThread();

    return {omeInfo, props};
  }
  return {{}, {}};
}
