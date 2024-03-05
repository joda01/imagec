
ARG DEBIAN_VERSION="11.6-slim"

FROM debian:$DEBIAN_VERSION AS live

# Avoid warnings by switching to noninteractive
ENV DEBIAN_FRONTEND=noninteractive

# This Dockerfile's base image has a non-root user with sudo access. Use the "remoteUser"
# property in devcontainer.json to use it. On Linux, the container user's GID/UIDs
# will be updated to match your local UID/GID (when using the dockerFile property).
# See https://aka.ms/vscode-remote/containers/non-root-user for details.
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# [Optional] Update UID/GID if needed
RUN   if [ "$USER_GID" != "1000" ] || [ "$USER_UID" != "1000" ]; then \
    groupmod --gid $USER_GID $USERNAME \
    && usermod --uid $USER_UID --gid $USER_GID $USERNAME \
    && chown -R $USER_UID:$USER_GID /home/$USERNAME; \
    fi

RUN apt-get update && apt-get install -y autoconf automake libtool curl make g++ unzip checkinstall gdb ninja-build wget libssl-dev


RUN wget https://github.com/Kitware/CMake/archive/refs/tags/v3.25.2.tar.gz -O cmake.tar.gz && \
    tar xf cmake.tar.gz && \
    rm cmake.tar.gz &&\
    cd CMake-* && \
    ./bootstrap --prefix=/usr --no-qt-gui --parallel=8 && \
    make -j8 && \
    make install && \
    cd .. && \
    rm -rf CMake*

RUN apt-get update && apt-get install -y pkg-config git default-jre


#
# Catch2 unit test framework
#
RUN git clone --recursive -b v3.3.1 --depth=1 https://github.com/catchorg/Catch2.git /catch2 && \
    cd /catch2 && \
    cmake -Bbuild -H. -DBUILD_TESTING=OFF && \
    cmake --build build/ --target install &&\
    cd / &&\
    rm -rf /catch2




#
# LLVM toolchain
#
RUN echo "deb http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye-15 main" >> /etc/apt/sources.list &&\
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key| apt-key add - &&\
    apt-get update && apt-get install -y clang-format-15 clangd-15 &&\
    ln -s /usr/bin/clangd-15 /usr/bin/clangd &&\
    ln -s /usr/bin/clang-format-15 /usr/bin/clang-format


## LIBS ###################################################

#
# JSON
#
RUN git clone --recursive -b v3.11.2 --depth 1 https://github.com/nlohmann/json.git /json && \
    cd /json &&\
    cmake -S ./ -DJSON_BuildTests=OFF  &&\
    make -j4 &&\
    make install &&\
    checkinstall  --pkgname="json" --pkgversion="1.0.0" --pkgrelease="1" --install=no --backup=no --nodoc -D && \
    cp /json/json_1.0.0-1_amd64.deb /json.deb  && \
    cd / &&\
    rm -rf /json


RUN apt-get update && apt-get install -y libcurl4-openssl-dev
RUN ldconfig
RUN useradd $USERNAME
ENV DEBIAN_FRONTEND=dialog


#
# XML parser
#
RUN git clone -b v1.13 https://github.com/joda01/pugixml.git 
RUN cd ./pugixml &&\
    mkdir build &&\
    cd build &&\
    cmake .. &&\
    make -j4 &&\
    make install

RUN apt-get update && apt-get install -y default-jdk

#
# protobuf
#
RUN git clone --recurse-submodules -b v25.2 --depth 1  https://github.com/protocolbuffers/protobuf.git
RUN cd protobuf &&\
    cmake . &&\
    cmake -Bbuild -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 -DABSL_PROPAGATE_CXX_STD=ON . &&\
    cmake --build . --parallel 4 &&\
    cmake --install .

RUN ldconfig

#
# OPENCV
#
RUN git clone -b 4.9.0 --depth 1 https://github.com/opencv/opencv.git
RUN cd ./opencv &&\
    mkdir build &&\
    cd build &&\
    cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DWITH_CUDA=ON -DCMAKE_INSTALL_PREFIX=/usr/local .. &&\
    make -j7 &&\
    make install



#
# QT
#

RUN apt-get update &&\
    apt-get install -y build-essential perl python git &&\
    apt-get install -y libfontconfig1-dev libdbus-1-dev libfreetype6-dev libicu-dev libinput-dev libxkbcommon-dev libsqlite3-dev libssl-dev


RUN git clone https://code.qt.io/qt/qt5.git qt6
RUN cd qt6 &&\
    git switch 6.3 &&\
    perl init-repository

RUN apt-get update && apt-get install -y libxcb-util-dev libxkbcommon-x11-dev \
    libfontconfig1-dev \
    libfreetype6-dev \
    libx11-dev \
    libx11-xcb-dev \
    libxext-dev \
    libxfixes-dev \
    libxi-dev \
    libxrender-dev \
    libxcb1-dev \
    libxcb-cursor-dev \
    libxcb-glx0-dev \
    libxcb-keysyms1-dev \
    libxcb-image0-dev \
    libxcb-shm0-dev \
    libxcb-icccm4-dev \
    libxcb-sync-dev \
    libxcb-xfixes0-dev \
    libxcb-shape0-dev \
    libxcb-randr0-dev \
    libxcb-render-util0-dev \
    libxcb-util-dev \
    libxcb-xinerama0-dev \
    libxcb-xkb-dev \
    libxkbcommon-dev

RUN apt install -y libgl1-mesa-dev libglu1-mesa-dev libx11-xcb-dev

RUN mkdir qt6-build &&\
    cd qt6-build &&\
    ls -l ../qt6/ &&\
    rm -rf ../qt6/qtwebengine &&\
    ../qt6/configure -opensource -confirm-license -release -xcb -xcb-xlib -bundled-xcb-xinput -prefix /opt/Qt6 &&\
    cmake --build . --parallel 6 &&\
    cmake --install .




#
# libtiff
#
RUN git clone -b v4.5.1 --depth 1 https://gitlab.com/libtiff/libtiff.git /libtiff &&\
    cd libtiff && \
    cmake -DBUILD_SHARED_LIBS=OFF . &&\
    cmake --build . --config Release --target install &&\
    cp -r libtiff/*.h  /usr/local/include



#
# xlsx writer
#
RUN git clone -b RELEASE_1.1.5 --depth 1 https://github.com/jmcnamara/libxlsxwriter.git /libxlsxwriter &&\
    cd libxlsxwriter && \
    mkdir build &&\
    cd build &&\
    cmake .. &&\
    cmake --build . &&\
    make install





#
# This target is used to build the project
#
FROM debian:$DEBIAN_VERSION AS build


RUN apt-get update && apt-get install -y autoconf automake libtool curl make g++ unzip checkinstall gdb ninja-build wget libssl-dev


RUN wget https://github.com/Kitware/CMake/archive/refs/tags/v3.25.2.tar.gz -O cmake.tar.gz && \
    tar xf cmake.tar.gz && \
    rm cmake.tar.gz &&\
    cd CMake-* && \
    ./bootstrap --prefix=/usr --no-qt-gui --parallel=8 && \
    make -j8 && \
    make install && \
    cd .. && \
    rm -rf CMake*

RUN apt-get update && apt-get install -y pkg-config git default-jre
RUN apt-get update && apt-get install -y libcurl4-openssl-dev default-jdk




#
# LLVM toolchain
#
RUN echo "deb http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye-15 main" >> /etc/apt/sources.list &&\
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key| apt-key add - &&\
    apt-get update && apt-get install -y clang-format-15 clangd-15 &&\
    ln -s /usr/bin/clangd-15 /usr/bin/clangd &&\
    ln -s /usr/bin/clang-format-15 /usr/bin/clang-format



COPY --from=live /opt /opt
COPY --from=live /usr /usr
COPY --from=live /lib /lib
COPY --from=live /bin /bin

RUN ldconfig
