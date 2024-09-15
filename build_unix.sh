cd build
cmake .. -G "Unix Makefiles" -DTAG_NAME="$TAG_NAME" -DCMAKE_TOOLCHAIN_FILE="build/Release/generators/conan_toolchain.cmake"
cmake --build . --config Release --target imagec --parallel 8
