cd build
cmake .. -G "Unix Makefiles" -DTAG_NAME="$TAG_NAME" -DCMAKE_TOOLCHAIN_FILE="build/Release/generators/conan_toolchain.cmake"
cmake --build . --config Release --target imagec --parallel 8
cd ..


rm -rf build/build/java
rm -rf build/build/plugins
cp -r resources/java build/build
cd build/build/java
unzip jre_linux.zip
cd ..

mkdir plugins
cp -r /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/plugins/* ./plugins
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Core.so .
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Core.so.6 .
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Core.so.6.7.1 .
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Gui.so .
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Gui.so.6 .
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Gui.so.6.7.1 .
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Widgets.so .
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Widgets.so.6 .
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Widgets.so.6.7.1 .
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6XcbQpa.so .
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6XcbQpa.so.6 .
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6XcbQpa.so.6.7.1 .
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Svg.so .
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Svg.so.6 .
cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Svg.so.6.7.1 .


cp /usr/lib/x86_64-linux-gnu/libxcb-cursor.so.0 libxcb-cursor.so.0
cp /workspaces/imagec/resources/launcher/imagec.sh imagec.sh
cp -r /workspaces/imagec/resources/templates ./templates
mkdir -p ./models

cd /workspaces/imagec
