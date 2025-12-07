FROM joda001/imagec-dev:linux-latest AS live


RUN wget https://github.com/Kitware/CMake/archive/refs/tags/v3.31.9.tar.gz -O cmake.tar.gz && \
    tar xf cmake.tar.gz && \
    rm cmake.tar.gz &&\
    cd CMake-* && \
    ./bootstrap --prefix=/usr --no-qt-gui --parallel=8 && \
    make -j8 && \
    make install && \
    cd .. && \
    rm -rf CMake*
