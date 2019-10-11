#! /usr/bin/env bash

set -eu
set -o pipefail

dt workspace select ./dependencies.yml
dt workspace sync -c googletest
dt workspace sync -c level_zero_linux
dt workspace sync -c yuv_samples

mkdir build
cd build
cmake \
  -G Ninja \
  -DL0_ROOT=$PWD/../third_party/level_zero_linux \
  -DREQUIRE_LEVELZERO_OPENCL_INTEROP=YES \
  -DCMAKE_C_COMPILER_LAUNCHER=ccache \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
  -DCMAKE_BUILD_TYPE=Release \
  ..
cmake --build . --target clang-format-check
cmake --build . --config Release --target install
# cmake --build . --config Release --target package
# cmake --build . --config Release --target package_source
