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
#include <opencv2/core/mat.hpp>

class BioformatsLoader
{
public:
  static cv::Mat loadEntireImage(const std::string &filename, int directory)
  {
    JNIEnv *env;
    JavaVM *jvm;

    JavaVMInitArgs vm_args;
    JavaVMOption options[1];
    options[0].optionString = "-Djava.class.path=bioformats/bioformats.jar";
    vm_args.version         = JNI_VERSION_1_8;
    vm_args.nOptions        = 1;
    vm_args.options         = options;

    // Create the JVM
    JNI_CreateJavaVM(&jvm, (void **) &env, &vm_args);

    // Call the BioFormatsWrapper.readImageBytes() method
    jclass cls        = env->FindClass("BioFormatsWrapper");
    jmethodID mid     = env->GetStaticMethodID(cls, "readImage", "(Ljava/lang/String;)[B");
    jstring filePath  = env->NewStringUTF(filename.c_str());
    jbyteArray result = (jbyteArray) env->CallStaticObjectMethod(cls, mid, filePath);
    env->DeleteLocalRef(filePath);

    // Convert the returned byte array to C++ bytes
    jsize length = env->GetArrayLength(result);
    jbyte *bytes = env->GetByteArrayElements(result, NULL);

    // Process the byte data as needed
    for(jsize i = 0; i < length; i++) {
      std::cout << static_cast<char>(bytes[i]);    // Print or save the bytes
    }

    // Clean up
    env->ReleaseByteArrayElements(result, bytes, 0);
    jvm->DestroyJavaVM();
  }
};
