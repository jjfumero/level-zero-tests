FROM ubuntu:18.04

ENV http_proxy http://proxy-chain.intel.com:911
ENV https_proxy http://proxy-chain.intel.com:911
RUN echo 'Acquire::http::Proxy "http://proxy-chain.intel.com:911";' > /etc/apt/apt.conf.d/proxy.conf

RUN apt-get update && apt-get install -y \
        build-essential \
        ccache \
        cmake \
        curl \
        git \
        libpapi-dev \
        ninja-build \
        ocl-icd-opencl-dev \
    && rm -rf /var/lib/apt/lists/*

# Setup irepo
RUN curl -sSL https://github.intel.com/raw/GSDI/irepo/master/install.sh | bash

# Install level-zero
# https://gitlab.devtools.intel.com/one-api/level_zero/tags/loki-0.1.6
# --force-all is used to avoid having to install NEO
ARG GFX_ASSETS_FM_JFROG_TOKEN
ENV GFX_ASSETS_FM_JFROG_TOKEN=$GFX_ASSETS_FM_JFROG_TOKEN
RUN mkdir /tmp/loki-packages && \
    cd /tmp/loki-packages && \
    curl -H "X-JFrog-Art-Api: ${GFX_ASSETS_FM_JFROG_TOKEN}" -O https://gfx-assets.fm.intel.com/artifactory/gfx-compute-build-assets/level_zero/master/449/artifacts/ubuntu18/intel-loki-core_0.2.0-449~bionic_release_amd64.deb && \
    curl -H "X-JFrog-Art-Api: ${GFX_ASSETS_FM_JFROG_TOKEN}" -O https://gfx-assets.fm.intel.com/artifactory/gfx-compute-build-assets/level_zero/master/449/artifacts/ubuntu18/intel-loki-devel_0.2.0-449~bionic_release_amd64.deb && \
    dpkg --force-all -i ./*.deb && \
    cd - && \
    rm -rf /tmp/loki-packages

CMD ["/bin/bash"]
