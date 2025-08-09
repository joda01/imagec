TAG_NAME=$1
CONAN_IMAGEC_ARTIFACTORY_PW=$2
WITH_CUDA=$3
GITHUB_WORKSPACE=$4

echo "Start linux build ..."
echo "TAG_NAME: $TAG_NAME"
echo "CONAN_IMAGEC_ARTIFACTORY_PW: $CONAN_IMAGEC_ARTIFACTORY_PW"
echo "WITH_CUDA: $WITH_CUDA"
echo "GITHUB_WORKSPACE: $GITHUB_WORKSPACE"


#
#
#
fetch_external_libs() {
    conan remote remove conancenter
    conan remote add imageclibs https://imagec.org:4431/artifactory/api/conan/imageclibs
    conan remote login imageclibs writer -p $CONAN_IMAGEC_ARTIFACTORY_PW
    if [[ -f "/root/.conan2/profiles/default" ]]; then
      "Loaded from cache"
    else
      conan profile detect
    fi
    conan install . \
      --profile conan/profile_linux \
      --output-folder=build \
      --build=missing \
      -o:a "&:with_cuda=$WITH_CUDA"
    chmod 777 -R /root/.conan2
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
pack(){
    chmod -R 777 build/build
    cd build/build
    mkdir -p output
    cd output
    mkdir -p ./plugins
    mkdir -p ./models
    mkdir -p ./lib
    mkdir -p ./java
    cp ../imagec imagec
    cp -r /root/.conan2/p/*/p/./plugins/* ./plugins
    cp -r ${GITHUB_WORKSPACE}/resources/templates ./templates
    cp ${GITHUB_WORKSPACE}/resources/launcher/imagec.sh imagec.sh
    cd lib
    cp /root/.conan2/p/*/p/lib/libQt6Core.so.6 .
    cp /root/.conan2/p/*/p/lib/libQt6Gui.so.6 .
    cp /root/.conan2/p/*/p/lib/libQt6Widgets.so.6 .
    cp /root/.conan2/p/*/p/lib/libQt6XcbQpa.so.6 .
    cp /root/.conan2/p/*/p/lib/libQt6Svg.so.6 .
    cp /usr/lib/x86_64-linux-gnu/libxcb-cursor.so.0 .
    cp /lib/x86_64-linux-gnu/libbsd.so.0 .
    cp /lib/x86_64-linux-gnu/libmd.so.0 .
    cp /lib/x86_64-linux-gnu/libGL.so.1 .
    cp /lib/x86_64-linux-gnu/libxcb.so.1 .
    cp /lib/x86_64-linux-gnu/libGLdispatch.so.0 .
    cp /lib/x86_64-linux-gnu/libGLX.so.0 .
    cp /lib/x86_64-linux-gnu/libXau.so.6 .
    cp /lib/x86_64-linux-gnu/libXdmcp.so.6 .

    cp /root/.conan2/p/*/p/lib/libtorch_cpu.so .
    cp /root/.conan2/p/*/p/lib/libtorch.so .
    cp /root/.conan2/p/*/p/lib/libc10.so .
    cp /root/.conan2/p/*/p/lib/libgomp-98b21ff3.so.1 .
    if [[ "$WITH_CUDA" == "True" ]]; then
      cp /root/.conan2/p/*/p/lib/libtorch_cuda.so .
      cp /root/.conan2/p/*/p/lib/libc10_cuda.so .
      cp /root/.conan2/p/*/p/lib/libcudart-d0da41ae.so.11.0 .
      cp /root/.conan2/p/*/p/lib/libcublas-3b81d170.so.11 .
      cp /root/.conan2/p/*/p/lib/libcublasLt-b6d14a74.so.11 .
      cp /root/.conan2/p/*/p/lib/libcudnn.so.9 .
      cp /root/.conan2/p/*/p/lib/libcudnn_graph.so.9 .
      cp /root/.conan2/p/*/p/lib/libcudnn_heuristic.so.9 .
      cp /root/.conan2/p/*/p/lib/libcudnn_engines_runtime_compiled.so.9 .
      cp /root/.conan2/p/*/p/lib/libcudnn_cnn.so.9 .
      cp /root/.conan2/p/*/p/lib/libcudnn_engines_precompiled.so.9 .
    fi
          
    cd ..
    strip imagec
    chmod +x imagec
    chmod +x imagec.sh
    cd java
    cp ${GITHUB_WORKSPACE}/resources/java/bioformats.jar .
    cp ${GITHUB_WORKSPACE}/resources/java/BioFormatsWrapper.class .
    cp -r ${GITHUB_WORKSPACE}/resources/java/jre_linux.zip .
    unzip jre_linux.zip
    rm -rf jre_linux.zip
    cd ..
}


fetch_external_libs
build
pack
