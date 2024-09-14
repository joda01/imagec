conan profile detect --force
conan install . --profile conan/profile_linux --output-folder=build --build=missing
cd build

cmake .. -G "Unix Makefiles" -DTAG_NAME="$TAG_NAME" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_TOOLCHAIN_FILE="build/Release/generators/conan_toolchain.cmake"
cmake --build . --config Release
