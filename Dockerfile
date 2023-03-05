#-------------------------------------------------------------------------------------------------------------
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License. See https://go.microsoft.com/fwlink/?linkid=2090316 for license information.
#-------------------------------------------------------------------------------------------------------------

FROM mcr.microsoft.com/vscode/devcontainers/base:0-debian-9

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


RUN git clone -b 4.7.0 https://github.com/opencv/opencv.git &&\
    cd ./opencv &&\
    mkdir build &&\
    cd build &&\
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local .. &&\
    make -j4 &&\
    make install


RUN apt-get update && apt-get install -y pkg-config

#
# Catch2 unit test framework
#
RUN git clone --recursive -b v3.3.1 --depth 1 https://github.com/catchorg/Catch2.git /catch2 && \
 cd /catch2 && \
 cmake -Bbuild -H. -DBUILD_TESTING=OFF && \
 cmake --build build/ --target install &&\
 cd / &&\
 rm -rf /catch2

RUN sudo apt install -y default-jre


COPY lib/libtiff-4.1 /libtiff-4.1
RUN cd /libtiff-4.1 && make
RUN mkdir -p /usr/local/include/libtiff
RUN cp -r /libtiff-4.1/tiff-4.1/libtiff/*.h  /usr/local/include/libtiff
RUN cp -r /libtiff-4.1/libtiff.a  /usr/local/lib/libtiff.a

# Switch back to dialog for any ad-hoc use of apt-get
ENV DEBIAN_FRONTEND=dialog
