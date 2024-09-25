
buildlibs(){
    conan profile detect --force
    conan install . --profile conan/profile_linux --output-folder=build --build=missing
    #cyclonedx-conan . --output sbom.spdx
    #conan graph info . --profile conan/profile_win_mingw --format=html > graph.html
}

buildlibsDebug(){
    #conan profile detect --force
    conan install . --profile conan/profile_linux_debug --output-folder=build --build=missing
    cyclonedx-conan . --output sbom.spdx
    conan graph info . --profile conan/profile_win_mingw --format=html > graph.html
}


build(){
    cd build
    cmake .. -G "Unix Makefiles" -DTAG_NAME="$TAG_NAME" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_TOOLCHAIN_FILE="build/Release/generators/conan_toolchain.cmake"
    cmake --build . --config Release --target imagec --parallel 16
exit
    cd ..

    rm -rf build/build/java
    rm -rf build/build/plugins
    rm -rf build/build/libs
    rm -rf build/build/lib



    cd build/build
    mkdir -p output
    cd output
    mkdir -p ./plugins
    mkdir -p ./models
    mkdir -p ./lib
    mkdir -p ./java
    cp ../imagec imagec
    cp -r /root/.conan2/p/b/*/p/./plugins/* ./plugins
    cp -r ../../../resources/templates ./templates
    cp ../../../resources/launcher/imagec.sh imagec.sh
    cd lib
    cp /root/.conan2/p/b/*/p/lib/libQt6Core.so.6 .
    cp /root/.conan2/p/b/*/p/lib/libQt6Gui.so.6 .
    cp /root/.conan2/p/b/*/p/lib/libQt6Widgets.so.6 .
    cp /root/.conan2/p/b/*/p/lib/libQt6XcbQpa.so.6 .
    cp /root/.conan2/p/b/*/p/lib/libQt6Svg.so.6 .
    cp /usr/lib/x86_64-linux-gnu/libxcb-cursor.so.0 .
    cd ..
    chmod +x imagec
    chmod +x imagec.sh
    cd java
    cp ../../../../resources/java/bioformats.jar .
    cp ../../../../resources/java/BioFormatsWrapper.class .
    cp -r ../../../../resources/java/jre_linux.zip .
    unzip jre_linux.zip
    rm -rf jre_linux.zip
    cd ..
}

#buildlibs
build
