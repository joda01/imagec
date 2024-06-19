# rm -rf ./build/build

mkdir -p ./build/build
mkdir -p ./build/build/plugins
mkdir -p ./build/build/platforms
mkdir -p ./build/build/templates
mkdir -p ./build/build/models

cmake -S . -DTAG_NAME="$TAG_NAME" -DCMAKE_BUILD_TYPE="Release" -B build -DCMAKE_PREFIX_PATH="/opt/Qt6/lib/cmake"
cmake --build build --target imagec --config Release --parallel 8
cmake --build build --target tests --config Release --parallel 8


#exit 0

cp /usr/lib/x86_64-linux-gnu/libicuio.so.67 ./build/build
cp /usr/lib/x86_64-linux-gnu/libicui18n.so.67 ./build/build
cp /usr/lib/x86_64-linux-gnu/libicuuc.so.67 ./build/build
cp /usr/lib/x86_64-linux-gnu/libicudata.so.67 ./build/build

cp /usr/lib/x86_64-linux-gnu/libOpenGL.so.0.0.0 ./build/build
cp /usr/lib/x86_64-linux-gnu/libOpenGL.so ./build/build
cp /usr/lib/x86_64-linux-gnu/libOpenGL.so.0 ./build/build

cp /usr/lib/x86_64-linux-gnu/libpcre2-16.so ./build/build
cp /usr/lib/x86_64-linux-gnu/libpcre2-16.so.0.10.1 ./build/build
cp /usr/lib/x86_64-linux-gnu/libpcre2-16.so.0 ./build/build

cp /usr/local/lib/libduckdb.so ./build/build

cp /opt/Qt6/lib/libQt6Core.so ./build/build
cp /opt/Qt6/lib/libQt6Core.so.6 ./build/build
cp /opt/Qt6/lib/libQt6Core.so.6.3.3 ./build/build
cp /opt/Qt6/lib/libQt6Gui.so ./build/build
cp /opt/Qt6/lib/libQt6Gui.so.6 ./build/build
cp /opt/Qt6/lib/libQt6Gui.so.6.3.3 ./build/build
cp /opt/Qt6/lib/libQt6Widgets.so ./build/build
cp /opt/Qt6/lib/libQt6Widgets.so.6 ./build/build
cp /opt/Qt6/lib/libQt6Widgets.so.6.3.3 ./build/build
cp /opt/Qt6/lib/libQt6XcbQpa.so ./build/build
cp /opt/Qt6/lib/libQt6XcbQpa.so.6 ./build/build
cp /opt/Qt6/lib/libQt6XcbQpa.so.6.3.3 ./build/build
cp /opt/Qt6/lib/libQt6DBus.so ./build/build
cp /opt/Qt6/lib/libQt6DBus.so.6 ./build/build
cp /opt/Qt6/lib/libQt6DBus.so.6.3.3 ./build/build
cp -r /opt/Qt6/plugins/* ./build/build/plugins
cp /opt/Qt6/plugins/platforms/* ./build/build/platforms/
cp imagec.sh ./build/build
cp -r ./templates/*.json ./build/build/templates
chmod +x ./build/build/imagec.sh

#
#cp -r ./java ./build/build/java
