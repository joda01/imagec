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
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lopencv_imgcodecs -Wno-unknown-pragmas -lz -O0 -lm -std=c++20")
SET(CMAKE_FIND_LIBRARY_SUFFIXES .a .so)
SET(CMAKE_GENERATOR "Unix Makefiles")
SET(CMAKE_EXPORT_COMPILE_COMMANDS true)



set(OpenCV_STATIC ON)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
find_package(PkgConfig REQUIRED)
find_package(httplib REQUIRED)


#
# Link to libraries
#
set(LIBS
  ${OpenCV_LIBS}
  /usr/local/lib/libtiff.a
  #/usr/local/lib/libz.a
  #/usr/local/lib/libjpeg.a
  httplib::httplib
  /usr/lib/x86_64-linux-gnu/libuuid.a
  /usr/local/lib/libpugixml.a
)

include_directories("./src")
include_directories("/usr/local/include/libtiff")


###########################################################################################
# Source file
###########################################################################################
file(GLOB_RECURSE SOURCES
  ./src/*.cpp
  ./src/*.c
  /usr/local/include/libtiff/*.h
)

# Exclude test files from source
list(FILTER SOURCES EXCLUDE REGEX ".*_test.cpp$")

#
# Create library or executable
#
if(MAKE_LIBRARY)
  add_library(${PROJECT_NAME} SHARED ${SOURCES})
  # Installation script
  install(TARGETS ${PROJECT_NAME} DESTINATION lib)
  install(FILES ./include/${PROJECT_NAME} DESTINATION include)          # Adapt here if neccessary
else()
  add_executable(${PROJECT_NAME} ${SOURCES})
endif()

set_target_properties( ${PROJECT_NAME}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
)

target_link_libraries(${PROJECT_NAME} ${LIBS})
target_compile_definitions(${PROJECT_NAME} PUBLIC CPPHTTPLIB_OPENSSL_SUPPORT)

###########################################################################################
# Unittest
###########################################################################################
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

file(GLOB_RECURSE TEST_SOURCES
  ./src/*.cpp
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

target_link_libraries(tests ${LIBS} ${TESTING_LIBS})
#include(CTest)
#include(Catch)

#catch_discover_tests(tests)
target_compile_definitions(tests PUBLIC UNIT_TEST)
