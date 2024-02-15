cmake_minimum_required (VERSION 3.5)

# Project name and version
project (imagec
         VERSION 1.0.0
         HOMEPAGE_URL https://danmayr.org
         LANGUAGES CXX C)

# 1) Choose option and adapt Line 95 if needed
option(MAKE_LIBRARY "Builds a library instead of an executable program." OFF)

SET(CMAKE_CXX20_STANDARD 20)
SET(CMAKE_CXX20_STANDARD_REQUIRED true)

IF (WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wno-unknown-pragmas -O2 -lz -lws2_32 -lwsock32 -lm -std=c++20")
  
  #SET(CMAKE_RC_COMPILER "windres.exe")
  SET(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} --include-dir C:/msys64/mingw64/include/wx-3.2 --define __WIN32__ --define __GNUWIN32__ --define WX_CPU_AMD64")

ELSE()

  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wno-unknown-pragmas -O2 -lm -std=c++20")

ENDIF()
SET(CMAKE_GENERATOR "Unix Makefiles")
SET(CMAKE_EXPORT_COMPILE_COMMANDS true)


IF (WIN32)
ELSE()
  set(CMAKE_FIND_LIBRARY_SUFFIXES .a .lib)
  SET(CMAKE_FIND_LIBRARY_SUFFIXES .a .so)

ENDIF()


set(OpenCV_STATIC ON)

include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package( OpenCV REQUIRED )
find_package( PkgConfig REQUIRED )
find_package( PugiXML )
find_package( OpenSSL  )
find_package(JNI REQUIRED )
find_package(Java COMPONENTS REQUIRED)
find_package( protobuf CONFIG REQUIRED )

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
#qt_standard_project_setup()



include_directories(${JNI_INCLUDE_DIRS})
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})


IF (WIN32)
  find_package( libjpeg-turbo )
  find_package( TIFF )
  set(ENV{WX_CONFIG} "wx-config-3.2")
  set(wxWidgets_CONFIG_EXECUTABLE "wx-config-3.2")
  set(wxWidgets_USE_LIBS)
  find_package( wxWidgets  REQUIRED)
  include(${wxWidgets_USE_FILE})
  include_directories(${wxWidgets_INCLUDE_DIRS})


ELSE ()
  find_package( Tiff )
  set(wxWidgets_USE_LIBS)
  find_package( wxWidgets REQUIRED)
  include(${wxWidgets_USE_FILE})
  include_directories(${wxWidgets_INCLUDE_DIRS})
ENDIF()

# Define libdl name
IF (NOT LIB_DL)
	IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		# Windows
		SET(LIB_DL "kernel32")
	ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		# Linux
		SET(LIB_DL "dl")
	ENDIF()
ENDIF()


#
# Link to libraries
#
IF (WIN32)
  # WINDOWS (msys)
  set(LIBS
    ${OpenCV_LIBS}
    ${OPENSSL_LIBRARIES}
    ${Protobuf_LIBRARIES}
    pugixml
    jpeg
    TIFF
    Qt6::Widgets
    ${wxWidgets_LIBRARIES}
    ${LIB_DL}
    #JNI::JVM
    #JNI::JNI
    #JNI::AWT
  )


ELSE()
  # c
  set(LIBS
    ${OpenCV_LIBS}
    ${OPENSSL_LIBRARIES}
    ${Protobuf_LIBRARIES}
    pugixml
    tiff
    Qt6::Widgets
    ${wxWidgets_LIBRARIES}
    ${LIB_DL}
    #JNI::JVM
    #JNI::JNI
    #JNI::AWT
  )
ENDIF()





include_directories("./src")

message("-->")
message("${wxWidgets_INCLUDE_DIRS}")
message("<--")

###########################################################################################
# Source file
###########################################################################################
IF (WIN32)
file(GLOB_RECURSE SOURCES
  ./src/*.cpp
  ./src/*.c
  ./src/*.cc
  /usr/local/include/libtiff/*.h
 ./src/wx.rc
)
ELSE()
file(GLOB_RECURSE SOURCES
  ./src/*.cpp
  ./src/*.c
  ./src/*.cc
  /usr/local/include/libtiff/*.h
)
ENDIF()

# Exclude test files from source
list(FILTER SOURCES EXCLUDE REGEX ".*_test.cpp$")


#qt_add_resources(helloworld imageresources
#    PREFIX "/images"
#    FILES logo.png splashscreen.png
#)
#
#qt_add_translations(helloworld
#    TS_FILES helloworld_de.ts helloworld_fr.ts)

add_executable(${PROJECT_NAME} ${SOURCES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

#
# Protobuf generate
#

# Specify the path to your .proto files
#set(PROTO_FILES src/onnx/onnx.proto 
#)
#set(PROTO_IMPORT_DIRS src/onnx/)
#protobuf_generate(
#  LANGUAGE cpp
#  TARGET ${PROJECT_NAME}
#  IMPORT_DIRS ${PROTO_IMPORT_DIRS}
#  PROTOS ${PROTO_FILES}
#  PROTOC_OUT_DIR "/workspaces/imagec/src/onnx"
#  )


set_target_properties( ${PROJECT_NAME}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
)

target_link_libraries(${PROJECT_NAME} ${LIBS})
target_link_options(${PROJECT_NAME}  PRIVATE -static-libgcc -static-libstdc++)
#target_compile_definitions(${PROJECT_NAME} PUBLIC CPPHTTPLIB_OPENSSL_SUPPORT)

IF (WIN32)

ELSE ()
  target_compile_definitions(${PROJECT_NAME} PUBLIC _FILE_OFFSET_BITS=64)
  target_compile_definitions(${PROJECT_NAME} PUBLIC __WXGTK__)
  target_compile_definitions(${PROJECT_NAME} PUBLIC WXUSINGDLL)
ENDIF()

 # -I/usr/local/lib/wx/include/gtk3-unicode-3.2 -I/usr/local/include/wx-3.2 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__ -pthread



###########################################################################################
# Unittest
###########################################################################################
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

file(GLOB_RECURSE TEST_SOURCES
  ./src/*.cpp
  ./src/*.cc
  ./src/*.c
  ./test/*.cpp
)
# Exclude main file from source
list(FILTER TEST_SOURCES EXCLUDE REGEX "main.cpp")

add_executable(tests ${TEST_SOURCES})

set_target_properties( tests
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
)

find_package(Catch2 3 REQUIRED)


set(TESTING_LIBS
  Catch2::Catch2
)

set_property(TARGET tests PROPERTY CXX_STANDARD 20)


target_link_libraries(tests ${LIBS} ${TESTING_LIBS})
#include(CTest)
#include(Catch)

#catch_discover_tests(tests)
target_compile_definitions(tests PUBLIC UNIT_TEST)
