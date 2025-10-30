#!/bin/sh
set -e  # Exit immediately if any command fails

TAG_NAME=$1
CONAN_IMAGEC_ARTIFACTORY_PW=$2
WITH_CUDA=$3
GITHUB_WORKSPACE=$4
USERS_DIR="/root"

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
  # NVIDIA Toolkit
  #
  if [ "$WITH_CUDA" = "True" ]; then
    echo "Install cuda toolkit ..."
    wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2404/x86_64/cuda-keyring_1.1-1_all.deb
    dpkg -i cuda-keyring_1.1-1_all.deb
    apt-get update
    apt-get install -y cuda-toolkit-12-8
    export PATH=/usr/local/cuda/bin:$PATH
    export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH
  fi

  #
  # Install conan
  #
  pip install conan numpy
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
    sudo cp "$WORKING_DIR/conan/profile_linux" /root/.conan2/profiles/default
    
    conan install . \
      --profile "$WORKING_DIR/conan/profile_linux" \
      --output-folder=build \
      --build=missing \
      -o:a "&:with_cuda=$WITH_CUDA"
    chmod 777 -R $USERS_DIR/.conan2
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
    WORKING_DIR=$(pwd)

    chmod -R 777 build/build
    cd build/build
    mkdir -p output
    cd output
    mkdir -p ./plugins
    mkdir -p ./models
    mkdir -p ./lib
    mkdir -p ./java
    strip ../imagec
    cp ../imagec imagec
    cp -r $USERS_DIR/.conan2/p/*/p/./plugins/* ./plugins
    cp -r ${WORKING_DIR}/resources/templates ./templates
    cp ${WORKING_DIR}/resources/launcher/imagec.sh imagec.sh
    cd lib
    cp $USERS_DIR/.conan2/p/*/p/lib/libQt6Core.so.6 .
    cp $USERS_DIR/.conan2/p/*/p/lib/libQt6Gui.so.6 .
    cp $USERS_DIR/.conan2/p/*/p/lib/libQt6Widgets.so.6 .
    cp $USERS_DIR/.conan2/p/*/p/lib/libQt6XcbQpa.so.6 .
    cp $USERS_DIR/.conan2/p/*/p/lib/libQt6Svg.so.6 .
    cp /usr/lib/x86_64-linux-gnu/libxcb-cursor.so.0 .
    cp /usr/lib/x86_64-linux-gnu/libGLdispatch.so.0 .
    cp /usr/lib/x86_64-linux-gnu/libGL.so.1 .
    cp /usr/lib/x86_64-linux-gnu/libGLX.so.0 .

    cp /lib/x86_64-linux-gnu/libbsd.so.0 .
    cp /lib/x86_64-linux-gnu/libmd.so.0 .
    cp /lib/x86_64-linux-gnu/libxcb.so.1 .
    cp /lib/x86_64-linux-gnu/libXau.so.6 .
    cp /lib/x86_64-linux-gnu/libXdmcp.so.6 .

    cp $USERS_DIR/.conan2/p/*/p/lib/libtorch_cpu.so .
    cp $USERS_DIR/.conan2/p/*/p/lib/libtorch.so .
    cp $USERS_DIR/.conan2/p/*/p/lib/libc10.so .
    cp $USERS_DIR/.conan2/p/*/p/lib/libgomp-98b21ff3.so.1 .
    if [ "$WITH_CUDA" = "True" ]; then
      cp /usr/local/cuda-12.8/targets/x86_64-linux/lib/libcudart.so.12 .
      cp $USERS_DIR/.conan2/p/*/p/lib/libtorch_cuda.so .
      cp $USERS_DIR/.conan2/p/*/p/lib/libc10_cuda.so .
      cp $USERS_DIR/.conan2/p/*/p/lib/libcudart-218eec4c.so.12 .
      cp $USERS_DIR/.conan2/p/*/p/lib/libcublas-f6c022dc.so.12 .
      cp $USERS_DIR/.conan2/p/*/p/lib/libcublasLt-4ef47ce6.so.12 .
      cp $USERS_DIR/.conan2/p/*/p/lib/libcudnn.so.9 .
      cp $USERS_DIR/.conan2/p/*/p/lib/libcudnn_graph.so.9 .
      cp $USERS_DIR/.conan2/p/*/p/lib/libcudnn_heuristic.so.9 .
      cp $USERS_DIR/.conan2/p/*/p/lib/libcudnn_engines_runtime_compiled.so.9 .
      cp $USERS_DIR/.conan2/p/*/p/lib/libcudnn_cnn.so.9 .
      cp $USERS_DIR/.conan2/p/*/p/lib/libcudnn_engines_precompiled.so.9 .
    fi
          
    cd ..
    chmod +x imagec
    chmod +x imagec.sh
    cd java
    cp ${WORKING_DIR}/resources/java/bioformats.jar .
    cp ${WORKING_DIR}/resources/java/BioFormatsWrapper.class .
    cp ${WORKING_DIR}/resources/java/jre_linux.zip .
    unzip jre_linux.zip
    rm -rf jre_linux.zip
    cd ..
}

install_dependencies
fetch_external_libs
build
pack
