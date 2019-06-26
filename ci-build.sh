#! /usr/bin/env bash

set -eu
set -o pipefail

# Disable SSH host key verification for irepo
echo "Host *" >> /etc/ssh/ssh_config && \
echo "   StrictHostKeyChecking no" >> /etc/ssh/ssh_config  && \
echo "    UserKnownHostsFile=/dev/null" >> /etc/ssh/ssh_config

# irepo authentication
echo "machine gerrit-gfx.intel.com" >> ~/.netrc
echo "login $GERRITGFX_HTTP_USER" >> ~/.netrc
echo "password $GERRITGFX_HTTP_PASSWORD" >> ~/.netrc

# Synchronize third-party assets with irepo
~/.irepo/irepo select ./linux.yml
~/.irepo/irepo sync

mkdir build
cd build
cmake ..
make -j`nproc` package_source package
