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

CMD ["/bin/bash"]
