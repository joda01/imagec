#cd build
#cmake .. -G "Unix Makefiles" -DTAG_NAME="$TAG_NAME" -DCMAKE_TOOLCHAIN_FILE="build/Release/generators/conan_toolchain.cmake"
#cmake --build . --config Release --target imagec --parallel 8
#cd ..
rm -rf build/build/java
rm -rf build/build/plugins
rm -rf build/build/platforms
cp -r resources/java build/build
cd build/build/java
unzip jre_linux.zip
cd ..
mkdir plugins
mkdir platforms
cp -r /root/.conan/data/qt/6.7.1/_/_/build/fe6021deefd72c51ff7bb88e4d708f081fa46747/build/Release/qtbase/plugins/* ./plugins
cp -r /root/.conan/data/qt/6.7.1/_/_/build/fe6021deefd72c51ff7bb88e4d708f081fa46747/build/Release/qtbase/plugins/platforms/* ./platforms
cp /workspaces/imagec/resources/launcher/imagec.sh imagec.sh
cd /workspaces/imagec
