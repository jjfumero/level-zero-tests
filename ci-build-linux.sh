#! /usr/bin/env bash

set -eu
set -o pipefail

# irepo authentication
echo "machine gerrit-gfx.intel.com" >> ~/.netrc
echo "login $GERRITGFX_HTTP_USER" >> ~/.netrc
echo "password $GERRITGFX_HTTP_PASSWORD" >> ~/.netrc

# Synchronize third-party assets with irepo
~/.irepo/irepo select ./dependencies.yml
~/.irepo/irepo sync -c googletest
~/.irepo/irepo sync -c level_zero_linux
~/.irepo/irepo sync -c yuv_samples

mkdir build
cd build
cmake \
  -G Ninja \
  -DCMAKE_C_COMPILER_LAUNCHER=ccache \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
  -DCMAKE_BUILD_TYPE=Release \
  ..
cmake --build . --config Release --target clang-format-check
cmake --build . --config Release --target package_source
cmake --build . --config Release --target package
mv intel-level-zero-* ..
