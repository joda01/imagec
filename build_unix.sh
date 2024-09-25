
buildlibs(){
    #conan profile detect --force
    conan install . --profile conan/profile_linux --output-folder=build --build=missing
    cyclonedx-conan . --output sbom.spdx
    conan graph info . --profile conan/profile_win_mingw --format=html > graph.html
}

build(){
    cd build
    cmake .. -G "Unix Makefiles" -DTAG_NAME="$TAG_NAME" -DCMAKE_TOOLCHAIN_FILE="build/Release/generators/conan_toolchain.cmake"
    cmake --build . --config Release --target imagec --parallel 8
    cd ..

    rm -rf build/build/java
    rm -rf build/build/plugins
    rm -rf build/build/libs
    rm -rf build/build/lib
    cp -r resources/java build/build
    cd build/build/java
    unzip jre_linux.zip
    cd ..

    mkdir plugins
    mkdir lib
    mkdir -p ./models
    cp /workspaces/imagec/resources/launcher/imagec.sh imagec.sh
    cp -r /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/plugins/* ./plugins
    cp -r /workspaces/imagec/resources/templates ./templates
    cd lib
    cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Core.so.6 .
    cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Gui.so.6 .
    cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Widgets.so.6 .
    cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6XcbQpa.so.6 .
    cp /root/.conan/data/qt/6.7.1/_/_/build/*/build/Release/qtbase/lib/libQt6Svg.so.6 .
    cd ..
    cd /workspaces/imagec
}

buildlibs
build
