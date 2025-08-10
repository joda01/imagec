#!/bin/sh
set -e  # Exit immediately if any command fails

TAG_NAME=$1
CONAN_IMAGEC_ARTIFACTORY_PW=$2
WITH_CUDA="False"
GITHUB_WORKSPACE=$4
USERS_DIR="/Users/runner"


echo "Start linux build ..."
echo "TAG_NAME: $TAG_NAME"
echo "CONAN_IMAGEC_ARTIFACTORY_PW: $CONAN_IMAGEC_ARTIFACTORY_PW"
echo "WITH_CUDA: $WITH_CUDA"
echo "GITHUB_WORKSPACE: $GITHUB_WORKSPACE"

#
#
#
install_dependencies() {
    #
    # Install conan
    #
    pip install conan
}


#
#
#
fetch_external_libs() {
    WORKING_DIR=$(pwd)

    conan remote remove conancenter
    conan remote add imageclibs https://imagec.org:4431/artifactory/api/conan/imageclibs
    conan remote login imageclibs writer -p $CONAN_IMAGEC_ARTIFACTORY_PW
    conan profile detect
    cp "$WORKING_DIR/conan/profile_macos" $USERS_DIR/.conan2/profiles/default

    conan install . \
      --profile "$WORKING_DIR/conan/profile_macos" \
      --output-folder=build \
      -o:a "&:with_cuda=$WITH_CUDA"
}

#
#
#
build(){
    cd build
    cmake .. \
        -G "Unix Makefiles" \
        -DTAG_NAME="$TAG_NAME" \
        -DWITH_CUDA="$WITH_CUDA" \
        -DCMAKE_BUILD_TYPE="Release" \
        -DCMAKE_POLICY_DEFAULT_CMP0091=NEW \
        -DCMAKE_TOOLCHAIN_FILE="build/Release/generators/conan_toolchain.cmake" \
        -DCUDA_TOOLKIT_ROOT_DIR="/usr/local/cuda" \
        -DCMAKE_CUDA_COMPILER="/usr/local/cuda/bin/nvcc"
    cmake --build . --config Release --target imagec --parallel 8
    cd ..
}

#
#
#
fix_dylib_paths() {
    APP_PATH="$1"   # e.g. MyApp.app
    FRAMEWORKS_DIR="$APP_PATH/Contents/Frameworks"

    if [ ! -d "$FRAMEWORKS_DIR" ]; then
        echo "No Frameworks directory found at $FRAMEWORKS_DIR"
        return 1
    fi

    echo "Fixing dylib paths in $APP_PATH"

    # Find all binaries (app executable + any dylibs/frameworks)
    find "$APP_PATH/Contents/MacOS" "$FRAMEWORKS_DIR" -type f | while read BIN; do
        if file "$BIN" | grep -q "Mach-O"; then
            echo "Processing: $BIN"

            # List linked libraries
            otool -L "$BIN" | awk 'NR>1 {print $1}' | while read DEP; do
                # Skip system libraries
                if [[ "$DEP" == /System/* ]] || [[ "$DEP" == /usr/lib/* ]]; then
                    continue
                fi

                BASENAME=$(basename "$DEP")

                # If dependency exists in our Frameworks dir, rewrite path
                if [ -f "$FRAMEWORKS_DIR/$BASENAME" ]; then
                    echo "  Fixing $DEP -> @executable_path/../Frameworks/$BASENAME"
                    install_name_tool -change "$DEP" "@executable_path/../Frameworks/$BASENAME" "$BIN"
                else
                    # Handle .framework style paths
                    FRAMEWORK_NAME=$(echo "$DEP" | sed -n 's|.*/\([^/]*\.framework\)/.*|\1|p')
                    if [ -n "$FRAMEWORK_NAME" ] && [ -d "$FRAMEWORKS_DIR/$FRAMEWORK_NAME" ]; then
                        echo "  Fixing $DEP -> @executable_path/../Frameworks/$FRAMEWORK_NAME/$(basename "$DEP")"
                        install_name_tool -change "$DEP" "@executable_path/../Frameworks/$FRAMEWORK_NAME/$(basename "$DEP")" "$BIN"
                    fi
                fi
            done
        fi
    done
}

#
#
#
pack(){
    chmod -R 777 build/build
    cd build/build
    mkdir -p output
    cd output
    mkdir -p ./plugins
    mkdir -p ./models
    mkdir -p ./lib
    mkdir -p ./java
    
    cp -r ../imagec.app imagec.app

    #
    # Copy libs
    #
    mkdir -p imagec.app/Contents/Frameworks
    chmod +x imagec.app/Contents/MacOS/imagec

    cp $USERS_DIR/.conan2/p/*/p/lib/libQt6Core.6.dylib imagec.app/Contents/Frameworks/
    cp $USERS_DIR/.conan2/p/*/p/lib/libQt6Gui.6.dylib imagec.app/Contents/Frameworks/
    cp $USERS_DIR/.conan2/p/*/p/lib/libQt6Widgets.6.dylib imagec.app/Contents/Frameworks/
    cp $USERS_DIR/.conan2/p/*/p/lib/libQt6Svg.6.dylib imagec.app/Contents/Frameworks/
   
    #cp /usr/lib/x86_64-linux-gnu/libxcb-cursor.so.0 .
    #cp /lib/x86_64-linux-gnu/libbsd.so.0 .
    #cp /lib/x86_64-linux-gnu/libmd.so.0 .
    #cp /lib/x86_64-linux-gnu/libGL.so.1 .
    #cp /lib/x86_64-linux-gnu/libxcb.so.1 .
    #cp /lib/x86_64-linux-gnu/libGLdispatch.so.0 .
    #cp /lib/x86_64-linux-gnu/libGLX.so.0 .
    #cp /lib/x86_64-linux-gnu/libXau.so.6 .
    #cp /lib/x86_64-linux-gnu/libXdmcp.so.6 .

    cp $USERS_DIR/.conan2/p/*/p/lib/libtorch_cpu.dylib imagec.app/Contents/Frameworks/
    cp $USERS_DIR/.conan2/p/*/p/lib/libtorch.dylib imagec.app/Contents/Frameworks/
    cp $USERS_DIR/.conan2/p/*/p/lib/libc10.dylib imagec.app/Contents/Frameworks/
    cp $USERS_DIR/.conan2/p/*/p/lib/libtorch_global_deps.dylib imagec.app/Contents/Frameworks/
    cp $USERS_DIR/.conan2/p/*/p/lib/libshm.dylib imagec.app/Contents/Frameworks/
    cp $USERS_DIR/.conan2/p/*/p/lib/libomp.dylib imagec.app/Contents/Frameworks/
    #if [[ "$WITH_CUDA" == "True" ]]; then
    #  cp $USERS_DIR/.conan2/p/*/p/lib/libtorch_cuda.so .
    #  cp $USERS_DIR/.conan2/p/*/p/lib/libc10_cuda.so .
    #  cp $USERS_DIR/.conan2/p/*/p/lib/libcudart-218eec4c.so.12 .
    #  cp $USERS_DIR/.conan2/p/*/p/lib/libcublas-f6c022dc.so.12 .
    #  cp $USERS_DIR/.conan2/p/*/p/lib/libcublasLt-4ef47ce6.so.12 .
    #  cp $USERS_DIR/.conan2/p/*/p/lib/libcudnn.so.9 .
    #  cp $USERS_DIR/.conan2/p/*/p/lib/libcudnn_graph.so.9 .
    #  cp $USERS_DIR/.conan2/p/*/p/lib/libcudnn_heuristic.so.9 .
    #  cp $USERS_DIR/.conan2/p/*/p/lib/libcudnn_engines_runtime_compiled.so.9 .
    #  cp $USERS_DIR/.conan2/p/*/p/lib/libcudnn_cnn.so.9 .
    #  cp $USERS_DIR/.conan2/p/*/p/lib/libcudnn_engines_precompiled.so.9 .
    #fi

    #
    # Copy qt plugins
    #
    mkdir -p imagec.app/Contents/PlugIns
    cp -r $USERS_DIR/.conan2/p/*/p/./plugins/* imagec.app/Contents/PlugIns/
    cp -r ${GITHUB_WORKSPACE}/resources/templates ./templates

    #
    # Needed on mac os to set the path to the libs correct
    #
    fix_dylib_paths imagec.app

    #
    # Copy JAVA
    #
    mkdir -p imagec.app/Contents/Java

    cd imagec.app/Contents/Java
    cp ${GITHUB_WORKSPACE}/resources/java/bioformats.jar .
    cp ${GITHUB_WORKSPACE}/resources/java/BioFormatsWrapper.class .
    cp -r ${GITHUB_WORKSPACE}/resources/java/jre_macos_arm.zip .
    unzip jre_macos_arm.zip
    rm -rf jre_macos_arm.zip
    cd ../../..

    #
    # Sign
    #
    codesign --deep --force --sign - imagec.app

    # xattr -rd com.apple.quarantine test.app
}

install_dependencies
fetch_external_libs
build
pack
