cmake_minimum_required (VERSION 3.5)

if(NOT CLANG_TIDY)
  find_package(Python3)
endif()

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()


if(POLICY CMP0026)
  cmake_policy(SET CMP0026 NEW)
endif()

if(POLICY CMP0051)
  cmake_policy(SET CMP0051 NEW)
endif()

if(POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()

if(POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW)
endif()


# Project name and version
project (imagec
         VERSION 1.0.0
         HOMEPAGE_URL https://imagec.org
         LANGUAGES CXX C)

# Get the current date and time
string(TIMESTAMP BUILD_TIME "%Y-%m-%d %H:%M:%S")

if(NOT DEFINED TAG_NAME)
    message(FATAL_ERROR "TAG_NAME variable not provided!")
endif()

if("${TAG_NAME}" STREQUAL "")
  set(VERSION "devel-build")
else()
  set(VERSION "${TAG_NAME}")
endif()

message("Tag name is ${TAG_NAME}")

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if (APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/lib")
else ()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")
endif()

# Options
set(WITH_ONNX "With onnx support" ON)
set(WITH_PYTORCH "With pytorch support" ON)
set(WITH_TENSORFLOW "With tensorflow support" ON)


# Get the current user and host
execute_process(COMMAND whoami OUTPUT_VARIABLE BUILD_USER OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND hostname OUTPUT_VARIABLE BUILD_HOST OUTPUT_STRIP_TRAILING_WHITESPACE)


SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
#SET(CMAKE_CXX_FLAGS "-g -O0")
SET(CMAKE_GENERATOR "Unix Makefiles")
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


find_package( OpenCV REQUIRED )
find_package( PugiXML )
find_package( JNI REQUIRED )
find_package( Java COMPONENTS REQUIRED)
find_package( protobuf CONFIG REQUIRED )
find_package( libxlsxwriter CONFIG REQUIRED)
find_package( nlohmann_json CONFIG REQUIRED)
find_package( DuckDB CONFIG REQUIRED)
find_package( Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg)
find_package( ONNX REQUIRED )
find_package( ryml REQUIRED )
find_package( OpenSSL REQUIRED )


SET(DuckDB_USE_STATIC_LIBS TRUE)

set(LIBS
  ${OpenCV_LIBS}
  pugixml::pugixml
  Qt6::Widgets
  Qt6::Core
  Qt6::Svg
  libxlsxwriter::libxlsxwriter
  duckdb::duckdb
  nlohmann_json::nlohmann_json
  onnx::onnx
  ryml::ryml
  openssl::openssl
)

if(WITH_ONNX)
  find_package(onnxruntime REQUIRED)
  set(LIBS ${LIBS} onnxruntime::onnxruntime)
endif()

if(WITH_PYTORCH)
  find_package( Torch REQUIRED )
  set(LIBS ${LIBS} ${TORCH_LIBRARIES})
endif()

if(WITH_TENSORFLOW)
  find_package( tensorflowlite REQUIRED )
  set(LIBS ${LIBS} tensorflow::tensorflowlite)
endif()


set(RESOURCE_FILES
    resources/qt_conf.qrc
    resources/resource.qrc
    resources/icons.qrc
)

qt_add_resources(RESOURCE_SOURCES ${RESOURCE_FILES})

include_directories("./src")
include_directories(${JNI_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})


file(GLOB_RECURSE SOURCES
  ./src/*.cpp
  ./src/*.c
  ./src/*.cc
 ${RESOURCE_SOURCES}
)

# Exclude test files from source
list(FILTER SOURCES EXCLUDE REGEX ".*_test.cpp$")

add_executable(${PROJECT_NAME} ${SOURCES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
    CMAKE_AUTOMOC ON
)

target_sources(${PROJECT_NAME} PRIVATE ${RESOURCE_SOURCES})
target_compile_definitions(${PROJECT_NAME} PUBLIC BS_THREAD_POOL_ENABLE_PRIORITY DUCKDB_BUILD_LIBRARY )

if(WITH_ONNX)
  add_compile_definitions(WITH_ONNX)
endif()

if(WITH_PYTORCH)
  add_compile_definitions(WITH_PYTORCH)
endif()

if(WITH_TENSORFLOW)
  add_compile_definitions(WITH_TENSORFLOW)
endif()


set_target_properties( ${PROJECT_NAME}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
)

target_link_libraries(${PROJECT_NAME} ${LIBS})

configure_file(
  "${PROJECT_SOURCE_DIR}/src/build_info.h.in"
  "${PROJECT_SOURCE_DIR}/src/build_info.h"
)


###########################################################################################
# Unittest

set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

file(GLOB_RECURSE TEST_SOURCES
  ./src/*.cpp
  ./src/*.cc
  ./src/*.c
  ./test/*.cpp
   ${RESOURCE_SOURCES}

)
# Exclude main file from source
list(FILTER TEST_SOURCES EXCLUDE REGEX "/main\\.cpp$")

add_executable(tests ${TEST_SOURCES})
set_property(TARGET tests PROPERTY CXX_STANDARD 20)
target_sources(tests PRIVATE ${RESOURCE_SOURCES})
set_target_properties( tests
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
  WIN32_EXECUTABLE ON
  MACOSX_BUNDLE ON
  CMAKE_AUTOMOC ON
)

find_package(Catch2 3 REQUIRED)

set(TESTING_LIBS
  Catch2::Catch2
)

set_property(TARGET tests PROPERTY CXX_STANDARD 20)
target_link_libraries(tests ${LIBS} ${TESTING_LIBS})
target_compile_definitions(tests PUBLIC UNIT_TEST BS_THREAD_POOL_ENABLE_PRIORITY)
