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
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lopencv_imgcodecs -Wno-unknown-pragmas -O2 -lz -lws2_32 -lwsock32 -lm -std=c++20")
ELSE()

  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lopencv_imgcodecs -Wno-unknown-pragmas -O2 -lm -std=c++20")

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





####################################################################################################
# This function converts any file into C/C++ source code.
# Example:
# - input file: data.dat
# - output file: data.h
# - variable name declared in output file: DATA
# - data length: sizeof(DATA)
# embed_resource("data.dat" "data.h" "DATA")
####################################################################################################
function(embed_resource resource_file_name source_file_name variable_name)

    file(READ ${resource_file_name} hex_content HEX)

    string(REPEAT "[0-9a-f]" 32 column_pattern)
    string(REGEX REPLACE "(${column_pattern})" "\\1\n" content "${hex_content}")

    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " content "${content}")

    string(REGEX REPLACE ", $" "" content "${content}")

    set(array_definition "static const unsigned char ${variable_name}[] =\n{\n${content}\n};")

    set(source "// Auto generated file.\n${array_definition}\n")

    file(WRITE "${source_file_name}" "${source}")

endfunction()


#embed_resource("src/ui/wxwidgets/icons/icon_16_folder.png" "src/ui/wxwidgets/icons/icon_16_folder.h" "icon_16_folder")



#
# Link to libraries
#
IF (WIN32)
  # WINDOWS (msys)
  set(LIBS
    ${OpenCV_LIBS}
    ${OPENSSL_LIBRARIES}
    pugixml
    jpeg
    TIFF
    ${wxWidgets_LIBRARIES}
  )


ELSE()
  # LINUX
  set(LIBS
    ${OpenCV_LIBS}
    ${OPENSSL_LIBRARIES}
    pugixml
    tiff
    ${wxWidgets_LIBRARIES}
  )
ENDIF()


include_directories("./src")


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
add_executable(${PROJECT_NAME} ${SOURCES})


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
