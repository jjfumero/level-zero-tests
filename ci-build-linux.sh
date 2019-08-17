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
cmake --build . --target clang-format-check
cmake --build . --config Release --target install
# cmake --build . --config Release --target package
# cmake --build . --config Release --target package_source

# Special archives to support legacy compute-samples GTA plugin

mkdir -p out/archived
cp -r out/perf_tests out/archived/perf-tests-Linux
pushd out/archived
tar -czvf perf-tests-Linux.tar.gz perf-tests-Linux
tar -ztvf perf-tests-Linux.tar.gz
popd

cp -r out/conformance_tests out/archived/conformance-tests-Linux
pushd out/archived
tar -czvf conformance-tests-Linux.tar.gz conformance-tests-Linux
tar -ztvf conformance-tests-Linux.tar.gz
popd
