# rm -rf ./build/build
mkdir -p ./build/build
mkdir -p ./build/build/plugins
mkdir -p ./build/build/platforms

cmake --build build --target imagec --parallel 4
#cmake --build build --target tests --parallel 4
cp /usr/lib/x86_64-linux-gnu/libicuio.so.67 ./build/build
cp /usr/lib/x86_64-linux-gnu/libicui18n.so.67 ./build/build
cp /usr/lib/x86_64-linux-gnu/libicuuc.so.67 ./build/build
cp /usr/lib/x86_64-linux-gnu/libicudata.so.67 ./build/build
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


#cp /usr/lib/x86_64-linux-gnu/libicuio.so.67 ./build/build/platforms
#cp /usr/lib/x86_64-linux-gnu/libicui18n.so.67 ./build/build/platforms
#cp /usr/lib/x86_64-linux-gnu/libicuuc.so.67 ./build/build/platforms

cp -r /opt/Qt6/plugins/* ./build/build/plugins

cp /opt/Qt6/plugins/platforms/* ./build/build/platforms/

cp imagec.sh ./build/build
chmod +x ./build/build/imagec.sh
