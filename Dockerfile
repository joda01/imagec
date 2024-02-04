
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

RUN apt-get update && apt-get install -y pkg-config git default-jre google-mock libgmock-dev


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

#
# WXWidgets
#
RUN git clone --recurse-submodules -b v3.2.2.1 --depth 1  https://github.com/wxWidgets/wxWidgets.git /wxWidgets
RUN apt install -y libgtk-3-dev
RUN cd wxWidgets &&\
    mkdir gtk-build  &&\
    cd gtk-build  &&\
    ../configure --disable-shared --enable-unicode --with-gtk=3 &&\
    make -j6 &&\
    make install


RUN apt-get update && apt-get install -y default-jdk


#
# protobuf
#
RUN git clone --recurse-submodules -b v25.2 --depth 1  https://github.com/protocolbuffers/protobuf.git
RUN cd protobuf &&\
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
# libtiff
#
RUN git clone -b v4.5.1 --depth 1 https://gitlab.com/libtiff/libtiff.git /libtiff &&\
    cd libtiff && \
    cmake -DBUILD_SHARED_LIBS=OFF . &&\
    cmake --build . --config Release --target install &&\
    cp -r libtiff/*.h  /usr/local/include

#FROM live as build
#
#RUN mkdir ./build
#COPY ./ ./build
#RUN cd ./build &&\
#    ./cleanup.sh &&\
#    ./build.sh
#
#
#FROM debian:$DEBIAN_VERSION AS run
#
#RUN mkdir -p /imagec
#
#COPY --from=build ./build/build/build/imagec /imagec/imagec
#COPY ./imagec_gui /imagec/imagec_gui
#COPY ./imagec_models /imagec/imagec_models
#
#WORKDIR /imagec
#ENTRYPOINT ["sh", "-c", "cd /imagec && ./imagec"]
