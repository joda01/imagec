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
#include <string>
#include <opencv2/core/mat.hpp>

class BioformatsLoader
{
public:
  static cv::Mat loadEntireImage(const std::string &filename, int directory)
  {
    JavaVMInitArgs initArgs; /* Virtual Machine (VM) initialization structure, passed by*/

    JavaVM *myJVM;           /* JavaVM pointer set by call to JNI_CreateJavaVM */
    JNIEnv *myEnv;           /* JNIEnv pointer set by call to JNI_CreateJavaVM */
    char *myClasspath;       /* Changeable classpath 'string' */
    jclass myClass;          /* The class to call, 'NativeHello'. */
    jmethodID mainID;        /* The method ID of its 'main' routine. */
    jclass stringClass;      /* Needed to create the String[] arg for main */
    jobjectArray args;       /* The String[] itself */
    JavaVMOption options[1]; /* Options array -- use options to set classpath */
    int fd0, fd1, fd2;       /* file descriptors for IO */

    /*  Set the version field of the initialization arguments for JNI v1.4. */
    initArgs.version = JNI_VERSION_1_8;

    /* Now, you want to specify the directory for the class to run in the classpath.
     * with  Java2, classpath is passed in as an option.
     * Note: You must specify the directory name in UTF-8 format. So, you wrap
     *       blocks of code in #pragma convert statements.
     */
    options[0].optionString = "-Djava.class.path=bioformats";

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
    if(JNI_CreateJavaVM(&myJVM, (void **) &myEnv, (void *) &initArgs) != 0) {
      exit(1);
    }

    // Call the BioFormatsWrapper.readImageBytes() method
    jclass cls        = myEnv->FindClass("BioFormatsWrapper");
    jmethodID mid     = myEnv->GetStaticMethodID(cls, "readImage", "(Ljava/lang/String;)[B");
    jstring filePath  = myEnv->NewStringUTF(filename.c_str());
    jbyteArray result = (jbyteArray) myEnv->CallStaticObjectMethod(cls, mid, filePath);
    myEnv->DeleteLocalRef(filePath);

    // Convert the returned byte array to C++ bytes
    jsize length = myEnv->GetArrayLength(result);
    jbyte *bytes = myEnv->GetByteArrayElements(result, NULL);

    // Process the byte data as needed
    for(jsize i = 0; i < length; i++) {
      std::cout << std::to_string(static_cast<char>(bytes[i])) << std::endl;
      ;    // Print or save the bytes
    }

    // Clean up
    myEnv->ReleaseByteArrayElements(result, bytes, 0);
    myJVM->DestroyJavaVM();
  }
};
