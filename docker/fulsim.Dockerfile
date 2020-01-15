FROM ubuntu:bionic-20190718

ENV http_proxy http://proxy-chain.intel.com:911
ENV https_proxy http://proxy-chain.intel.com:911
RUN echo 'Acquire::http::Proxy "http://proxy-chain.intel.com:911";' > /etc/apt/apt.conf.d/proxy.conf

RUN apt-get update && apt-get install -y \
        curl \
        git \
        lib32stdc++6 \
        libc6-i386 \
        libstdc++6 \
        unzip \
    && rm -rf /var/lib/apt/lists/*

# Setup irepo and gta-asset
RUN curl -sSL https://gitlab.devtools.intel.com/vtt/sws/irepo/raw/master/install.sh | bash && \
    chmod +x /root/.irepo/.irepo/tools/gta-asset/gta-asset
ENV PATH=$PATH:/root/.irepo/.irepo/tools/gta-asset

# install fulsim
ARG ARTIFACTORY_USER
ENV ARTIFACTORY_USER=$ARTIFACTORY_USER
ARG ARTIFACTORY_PASSWORD
ENV ARTIFACTORY_PASSWORD=$ARTIFACTORY_PASSWORD
ARG FULSIM_NAME
ARG FULSIM_VERSION
RUN gta-asset pull \
      gfx-compute-fulsim-assets-igk/Fulsim ${FULSIM_NAME} ${FULSIM_VERSION} \
      --dest-dir=/tmp/fulsim \
      -u $ARTIFACTORY_USER \
      -p $ARTIFACTORY_PASSWORD && \
    unzip /tmp/fulsim/Linux*.zip -d /fulsim && \
    rm -rf /tmp/fulsim && \
    chmod +x /fulsim/AubLoad
ENV PATH=$PATH:/fulsim

# TBX server port
EXPOSE 4321/tcp
