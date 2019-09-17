FROM ubuntu:bionic-20190718
LABEL role="builder"

ENV http_proxy http://proxy-chain.intel.com:911
ENV https_proxy http://proxy-chain.intel.com:911
RUN echo 'Acquire::http::Proxy "http://proxy-chain.intel.com:911";' > /etc/apt/apt.conf.d/proxy.conf

RUN apt-get update && apt-get install -y \
        build-essential \
        ccache \
        clang-format-7 \
        clang-tidy \
        cmake \
        curl \
        git \
        libboost-all-dev \
        libpapi-dev \
        libpng-dev \
        libva-dev \
        ninja-build \
        ocl-icd-opencl-dev \
        opencl-headers \
    && rm -rf /var/lib/apt/lists/*

# Setup irepo
RUN curl -sSL https://github.intel.com/raw/GSDI/irepo/master/install.sh | bash

CMD ["/bin/bash"]
