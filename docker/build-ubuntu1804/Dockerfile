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

# install devtool and gta-asset tools
# TODO: using a patched version that fixes several issues for now, waiting for
# the fixes to get merged to master.
RUN git clone https://gitlab.devtools.intel.com/ledettwy/devtool.git ~/.devtool && \
    cd ~/.devtool && \
    git fetch origin ledettwy/mount_as_root_dir && \
    git checkout f7390409fe05e4a3f201ffb876aaf34555113314 && \
    ./dt download_tools
ENV PATH "$PATH:~/.devtool:~/.devtool/.deps/tools/gta-asset"

CMD ["/bin/bash"]
