FROM ubuntu:bionic-20190718
LABEL role="runtime"

ENV http_proxy http://proxy-chain.intel.com:911
ENV https_proxy http://proxy-chain.intel.com:911
RUN echo 'Acquire::http::Proxy "http://proxy-chain.intel.com:911";' > /etc/apt/apt.conf.d/proxy.conf

RUN apt-get update && apt-get install -y \
        curl \
        git \
        libpng16-16 \
        ocl-icd-opencl-dev \
    && rm -rf /var/lib/apt/lists/*

# Install yq, a command-line YAML parser (required for parsing test_config.yml)
RUN curl -sSL \
      https://github.com/mikefarah/yq/releases/download/2.4.0/yq_linux_amd64 \
      -o /usr/local/bin/yq && \
    chmod +x /usr/local/bin/yq

# Setup irepo
RUN curl -sSL https://github.intel.com/raw/GSDI/irepo/master/install.sh | bash

# Setup qb-asset
# RUN chmod +x ~/.irepo/.irepo/tools/gta-asset/qb-asset
# ENV PATH=$PATH:~/.irepo/.irepo/tools/gta-asset

# Download and install NEO runtime
ARG QB_USER
ARG QB_PASSWORD
# TODO: qb-asset fails to pull this correctly, so we have to manually curl
# everything.
# RUN qb-asset pull \
#       ubit/gfx/dynamic/run/products/gfx-driver/ci/master \
#       gfx-driver-ci-master-2758 \
#       -u $QB_USER \
#       -p $QB_PASSWORD
ARG UBIT_PREFIX="https://ubitstore.intel.com/webstores/fm/sfa/Artifacts/Graphics/Builds/cogd/dynamic/run/builds/b101/7051101/artifacts/Linux/Ubuntu/ReleaseInternal/linux64"
RUN echo "machine ubitstore.intel.com" > ~/.netrc && \
    echo "login $QB_USER" >> ~/.netrc && \
    echo "password $QB_PASSWORD" >> ~/.netrc && \
    mkdir /tmp/neo-packages && \
    cd /tmp/neo-packages && \
    curl -k --netrc -O $UBIT_PREFIX/intel-gmmlib-2.0-2758.u16.04-release-internal.x86_64.deb && \
    curl -k --netrc -O $UBIT_PREFIX/intel-igc-core-2.0-2758.u16.04-release-internal.x86_64.deb && \
    curl -k --netrc -O $UBIT_PREFIX/intel-igc-media-2.0-2758.u16.04-release-internal.x86_64.deb && \
    curl -k --netrc -O $UBIT_PREFIX/intel-igc-opencl-2.0-2758.u16.04-release-internal.x86_64.deb && \
    curl -k --netrc -O $UBIT_PREFIX/intel-opencl-2.0-2758.u16.04-release-internal.x86_64.deb && \
    dpkg -i ./*.deb && \
    cd - && \
    rm -rf /tmp/neo-packages

CMD ["/bin/bash"]
