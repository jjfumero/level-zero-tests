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

# # API key owned by Lisanna Dettwyler <lisanna.dettwyler@intel.com>
# RUN curl \
#         -H 'X-JFrog-Art-Api: AKCp5dKE5fAFNtDsVe3jgV7D2X8dEvy4Ho5QNj9xPBEwjYqZEkNQGmBa5f8Qa1XVr1SN7YzX6' \
#         https://gfx-assets.fm.intel.com/artifactory/gta-repos/tools/gta-asset/gta-asset-linux64/1.0.423/artifacts/qb-asset \
#         -o /usr/local/bin/qb-asset && \
#     chmod +x /usr/local/bin/qb-asset

# Install level-zero
# https://gitlab.devtools.intel.com/one-api/level_zero/tags/loki-0.1.6
# --force-all is used to avoid having to install NEO
# Jenkins artifact URLs are used because Jenkins supports authentication tokens
ARG JENKINS_USER
ENV JENKINS_USER=$JENKINS_USER
ARG JENKINS_TOKEN
ENV JENKINS_TOKEN=$JENKINS_TOKEN
RUN mkdir /tmp/loki-packages && \
    cd /tmp/loki-packages && \
    curl -u "$JENKINS_USER:$JENKINS_TOKEN" -O http://opencl.igk.intel.com/jenkins/view/Loki/job/ocl-loki-package-master/390/artifact/ubuntu18/intel-loki-core_0.1.6-382%7Ebionic_release_amd64.deb && \
    curl -u "$JENKINS_USER:$JENKINS_TOKEN" -O http://opencl.igk.intel.com/jenkins/view/Loki/job/ocl-loki-package-master/390/artifact/ubuntu18/intel-loki-devel_0.1.6-382%7Ebionic_release_amd64.deb && \
    dpkg --force-all -i ./*.deb && \
    cd - && \
    rm -rf /tmp/loki-packages

CMD [ "bash" ]
