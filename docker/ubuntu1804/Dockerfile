FROM ubuntu:bionic-20190718

ENV http_proxy http://proxy-chain.intel.com:911
ENV https_proxy http://proxy-chain.intel.com:911
RUN echo 'Acquire::http::Proxy "http://proxy-chain.intel.com:911";' > /etc/apt/apt.conf.d/proxy.conf

RUN apt-get update && apt-get install -y \
        build-essential \
        ccache \
        clang-format-7 \
        cmake \
        curl \
        git \
        libpapi-dev \
        libpng-dev \
        libva-dev \
        ninja-build \
        ocl-icd-opencl-dev \
        opencl-headers \
    && rm -rf /var/lib/apt/lists/*

RUN curl -L https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.tar.gz -O && \
    tar xf boost_1_70_0.tar.gz && \
    rm boost_1_70_0.tar.gz && \
    cd boost_1_70_0 && \
    ./bootstrap.sh && \
    ./b2 install \
      address-model=64 \
      --with-program_options \
      --with-timer \
      --with-chrono \
      --with-log \
      --with-system \
      -j`nproc` && \
    cd .. && \
    rm -rf boost_1_70_0

# Setup irepo
RUN curl -sSL https://github.intel.com/raw/GSDI/irepo/master/install.sh | bash

CMD ["/bin/bash"]
