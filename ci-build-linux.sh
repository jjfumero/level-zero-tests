#! /usr/bin/env bash

set -eu
set -o pipefail

# Synchronize third-party assets with irepo
~/.irepo/irepo select ./dependencies.yml
~/.irepo/irepo sync -c googletest
~/.irepo/irepo sync -c level_zero_linux
~/.irepo/irepo sync -c yuv_samples

mkdir build
cd build
cmake \
  -G Ninja \
  -DL0_ROOT=$PWD/../third_party/level_zero_linux \
  -DCMAKE_C_COMPILER_LAUNCHER=ccache \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
  -DCMAKE_BUILD_TYPE=Release \
  ..
cmake --build . --target clang-format-check
cmake --build . --config Release --target install
# cmake --build . --config Release --target package
# cmake --build . --config Release --target package_source
