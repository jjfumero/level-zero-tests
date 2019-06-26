FROM ubuntu:18.04

ENV http_proxy http://proxy-chain.intel.com:911
ENV https_proxy http://proxy-chain.intel.com:911
RUN echo 'Acquire::http::Proxy "http://proxy-chain.intel.com:911";' > /etc/apt/apt.conf.d/proxy.conf

RUN apt-get update && apt-get install -y \
        build-essential \
        cmake \
        curl \
        git \
        ocl-icd-opencl-dev \
    && rm -rf /var/lib/apt/lists/*

# Setup irepo
RUN curl -sSL https://github.intel.com/raw/GSDI/irepo/master/install.sh | bash

# Install NEO and level-zero
# https://gitlab.devtools.intel.com/one-api/level_zero/tags/loki-0.1.6
# --force-all is used due to broken loki dependencies
ARG GITLAB_ONEAPI_READKEY
ENV GITLAB_ONEAPI_READKEY=$GITLAB_ONEAPI_READKEY
RUN mkdir /tmp/loki-packages && \
    cd /tmp/loki-packages && \
    curl --header "Private-Token: $GITLAB_ONEAPI_READKEY" -O https://gitlab.devtools.intel.com/one-api/level_zero/uploads/f1f6179f0e27d77a7372dc8b2cbdc995/intel-loki-core_0.1.6-400_bionic_release_amd64.deb && \
    curl --header "Private-Token: $GITLAB_ONEAPI_READKEY" -O https://gitlab.devtools.intel.com/one-api/level_zero/uploads/52e65640d5a661ba1274a8ff45b34d51/intel-loki-devel_0.1.6-400_bionic_release_amd64.deb && \
    curl --header "Private-Token: $GITLAB_ONEAPI_READKEY" -O https://gitlab.devtools.intel.com/one-api/level_zero/uploads/4f545fdf09d5604aac5db7cc8ad60bd0/intel-igc-opencl-2.0-2206.u16.04-release.x86_64.deb && \
    curl --header "Private-Token: $GITLAB_ONEAPI_READKEY" -O https://gitlab.devtools.intel.com/one-api/level_zero/uploads/8cea5eaffb36722d94632074c521b20d/intel-igc-core-2.0-2206.u16.04-release.x86_64.deb && \
    curl --header "Private-Token: $GITLAB_ONEAPI_READKEY" -O https://gitlab.devtools.intel.com/one-api/level_zero/uploads/9693274cad3ae0e9d2d139f0a39a1bdb/intel-igc-opencl-devel-2.0-2206.u16.04-release.x86_64.deb && \
    curl --header "Private-Token: $GITLAB_ONEAPI_READKEY" -O https://gitlab.devtools.intel.com/one-api/level_zero/uploads/2fa9428945e9bb4e16a3a956de820bdd/intel-gmmlib-devel-2.0-2206.u16.04-release.x86_64.deb && \
    curl --header "Private-Token: $GITLAB_ONEAPI_READKEY" -O https://gitlab.devtools.intel.com/one-api/level_zero/uploads/05d184dd029eb32b32bae8849d50438a/intel-gmmlib-2.0-2206.u16.04-release.x86_64.deb && \
    dpkg --force-all -i ./*.deb && \
    cd - && \
    rm -rf /tmp/loki-packages

CMD [ "bash" ]
