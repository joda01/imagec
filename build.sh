


initConan(){
    pip install conan --upgrade --break-system-packages
    conan profile detect --force
    conan remote remove conancenter
    conan remote add imageclibs https://imagec.org:4431/artifactory/api/conan/imageclibs
    conan remote login imageclibs admin
}

patch(){
    echo "core.net.http:timeout = 500" >> ~/.conan2/global.conf
}

make(){
    conan install . --profile conan/profile_linux --output-folder=build --build=missing -o:a "&:with_cuda=True"
    cmake -S . -B ./build -G "Unix Makefiles" -DTAG_NAME="devel-build" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_TOOLCHAIN_FILE="./build/build/Release/generators/conan_toolchain.cmake"  -DCUDA_TOOLKIT_ROOT_DIR="/usr/local/cuda" -DCMAKE_CUDA_COMPILER="/usr/local/cuda/bin/nvcc" -DWITH_CUDA="True"
}

build(){
    cmake --build ./build --config Release --target imagec --parallel 16
   # cmake --build ./build --config Release --target tests --parallel 16
}


makeIcons() {
    TAG_NAME="devel-build"
    cd resources
    python3 get_icons.py
    cd ..
    cmake -S . -B ./build -G "Unix Makefiles" -DTAG_NAME="devel-build" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_TOOLCHAIN_FILE="./build/build/Release/generators/conan_toolchain.cmake" -DCUDA_TOOLKIT_ROOT_DIR="/usr/local/cuda" -DCMAKE_CUDA_COMPILER="/usr/local/cuda/bin/nvcc" -DWITH_CUDA="True"
}

copy(){
     cd build/build/output
     cp ../imagec imagec
}

pack(){
    rm -rf build/build/java
    rm -rf build/build/plugins
    rm -rf build/build/libs
    rm -rf build/build/lib

    cd build/build
    mkdir -p output
    cd output
    rm -rf ./templates/
    mkdir -p ./plugins
    mkdir -p ./models
    mkdir -p ./lib
    mkdir -p ./java
    mkdir -p ./templates
    
    cp ../imagec imagec
    
    
    cp ../tests tests
    cp -r /root/.conan2/p/*/p/./plugins/* ./plugins
    cp -r ../../../resources/templates/* ./templates/
    cp ../../../resources/launcher/imagec.sh imagec.sh
    cd lib
    cp /root/.conan2/p/*/p/lib/libQt6Core.so.6 .
    cp /root/.conan2/p/*/p/lib/libQt6Gui.so.6 .
    cp /root/.conan2/p/*/p/lib/libQt6Widgets.so.6 .
    cp /root/.conan2/p/*/p/lib/libQt6XcbQpa.so.6 .
    cp /root/.conan2/p/*/p/lib/libQt6Svg.so.6 .
    cp /usr/lib/x86_64-linux-gnu/libxcb-cursor.so.0 .
    cp /usr/lib/x86_64-linux-gnu/libxcb-icccm.so .
    cp /usr/lib/x86_64-linux-gnu/libGL.so.1 .
    cp /usr/lib/x86_64-linux-gnu/libGLX.so.0 .
    cp /usr/lib/x86_64-linux-gnu/libGLdispatch.so.0 .

    cp /root/.conan2/p/*/p/lib/libtorch_cpu.so .
    cp /root/.conan2/p/*/p/lib/libtorch_cuda.so .
    cp /root/.conan2/p/*/p/lib/libtorch.so .
    cp /root/.conan2/p/*/p/lib/libc10.so .
    cp /root/.conan2/p/*/p/lib/libc10_cuda.so .
    cp /root/.conan2/p/*/p/lib/libgomp-98b21ff3.so.1 .
    cp /root/.conan2/p/*/p/lib/libcudart-218eec4c.so.12 .
    cp /root/.conan2/p/*/p/lib/libcublas-f6c022dc.so.12 .
    cp /root/.conan2/p/*/p/lib/libcublasLt-4ef47ce6.so.12 .
    cp /root/.conan2/p/*/p/lib/libcudnn.so.9 .
    cp /root/.conan2/p/*/p/lib/libcudnn_graph.so.9 .
    cp /root/.conan2/p/*/p/lib/libcudnn_heuristic.so.9 .
    cp /root/.conan2/p/*/p/lib/libcudnn_engines_runtime_compiled.so.9 .
    cp /root/.conan2/p/*/p/lib/libcudnn_cnn.so.9 .
    cp /root/.conan2/p/*/p/lib/libcudnn_engines_precompiled.so.9 .

    cp /usr/local/cuda-12.8/targets/x86_64-linux/lib/libcudart.so.12 .

    cd ..
    chmod +x imagec
    chmod +x imagec.sh
    cd java
    cp -r ../../../../resources/java/bioformats.jar .
    cp -r ../../../../resources/java/BioFormatsWrapper.class .
    cp -r ../../../../resources/java/jre_linux.zip .
    #unzip  -qq jre_linux.zip
    rm -rf jre_linux.zip
    cd ..
    cd models
    cp -r ../../../../resources/models/*.onnx .
}

clean() {
    rm -rf ./build
    rm -rf CMakeUserPresets
}


# Check for arguments
if [ $# -eq 0 ]; then
    echo "ImageC build script usage:"
    echo "    [--init] Execute --init once after you initial setup the project."
    echo "    [--make] Execute --make every time some external deps have been changed or added."
    echo "    [--icons] Execute --make-icons every time a new icon has been added."
    echo "    [--build] Execute --build every time something in code has been changed."
    echo "    [--clean] To remove all build artifacts."
    exit 1
fi

# Parse arguments
for arg in "$@"; do
    case $arg in
        --init)
            # Execute init conan once after you initial setup the project
            initConan
            ;;
        --make)
            # Execute make every time some external deps have been changed or added
            make
            ;;
        --icons)
            # Execute --make-icons every time a new icon has been added.
            makeIcons
            ;;
        --build)
            # Execute build and pack every time something in code has been changed
            build
            #pack
            copy
            ;;
        --clean)
            clean
            ;;
        *)
            echo "Unknown option: $arg"
            echo "Usage: $0 [--init] [--icons] [--make] [--build] [--clean]"
            exit 1
            ;;
    esac
done
