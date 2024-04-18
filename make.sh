#!/bin/bash

# Delete old cmake files
#rm -f -r ./build
#mkdir ./build
cmake -S . -DCMAKE_BUILD_TYPE="Debug" -B build -DCMAKE_PREFIX_PATH="/opt/Qt6/lib/cmake"
