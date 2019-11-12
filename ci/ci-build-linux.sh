#! /usr/bin/env bash

set -eu
set -o pipefail

dt workspace select ./dependencies.yml
dt workspace sync -c googletest
dt workspace sync -c level_zero_linux
dt workspace sync -c yuv_samples

mkdir build
cd build

for group in "/" "/conformance_tests" "/perf_tests"; do
    cmake \
      -G Ninja \
      -D L0_ROOT=$PWD/../third_party/level_zero_linux \
      -D REQUIRE_LEVELZERO_OPENCL_INTEROP=YES \
      -D CMAKE_C_COMPILER_LAUNCHER=ccache \
      -D CMAKE_CXX_COMPILER_LAUNCHER=ccache \
      -D CMAKE_BUILD_TYPE=Release \
      -D GROUP=$group \
      -D CMAKE_INSTALL_PREFIX=$1/$group \
      ..
    cmake --build . --target clang-format-check
    cmake --build . --config Release --target install
done
