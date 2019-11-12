FROM python:3.8.0-alpine3.10
LABEL role="ci-run"

ENV http_proxy http://proxy-chain.intel.com:911
ENV https_proxy http://proxy-chain.intel.com:911

RUN pip3 install \
      docker \
      junitparser \
      PyYaml
