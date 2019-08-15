FROM ubuntu:bionic-20190718 AS ubuntu_base
FROM ubuntu_base AS build

ENV http_proxy http://proxy-chain.intel.com:911
ENV https_proxy http://proxy-chain.intel.com:911
RUN echo 'Acquire::http::Proxy "http://proxy-chain.intel.com:911";' > /etc/apt/apt.conf.d/proxy.conf

RUN apt-get update && apt-get install -y \
        build-essential \
        cmake \
        git \
        python \
    && rm -rf /var/lib/apt/lists/*

RUN git clone \
      -b khronos/spirv-3.6.1 \
      https://github.com/KhronosGroup/SPIRV-LLVM.git \
      /root/llvm && \
    export LLVM_SRC_ROOT="/root/llvm" && \
    cd "$LLVM_SRC_ROOT/tools" && \
    git clone -b spirv-1.1 https://github.com/KhronosGroup/SPIR clang && \
    cd "$LLVM_SRC_ROOT" && \
    mkdir build && \
    cd build && \
    cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX:PATH=/clang-spir .. && \
    make -j`nproc` && \
    make install

FROM ubuntu_base

COPY --from=build /clang-spir /clang-spir

ENV PATH "/clang-spir/bin:${PATH}"

ENTRYPOINT [ \
    "/clang-spir/bin/clang", \
    "-cc1", \
    "-emit-spirv", \
    "-triple", \
    "spir64-unknown-unknown", \
    "-cl-std=CL2.0", \
    "-include", "opencl.h", \
    "-x", "cl" \
]

CMD ["/bin/bash"]
