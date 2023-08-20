#!/bin/bash

# Delete old cmake files
rm -f -r ./build

BUILD_TYPE="Debug"

if [ ! -z "${1}" ]; then
    BUILD_TYPE="$1"
fi

mkdir ./build

echo "Building with profile ${BUILD_TYPE}"

# Executes cmake and make to build the project
cmake -S . -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -B build


cmake -S . -DCMAKE_BUILD_TYPE="Debug" -B build
