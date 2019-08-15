#! /usr/bin/env bash

set -eu
set -o pipefail

for path in $(find perf_tests conformance_tests -name "*.cl"); do
    dir=$(dirname "$path")
    basename=$(basename -- "$path")
    filename="${basename%.*}"
    echo "Compiling $path -> $dir/$filename.spv"
    clang \
      -cc1 \
      -emit-spirv \
      -triple spir64-unknown-unknown \
      -cl-std=CL2.0 \
      -include opencl.h \
      -x cl \
      -o $dir/$filename.spv \
      $path &
done

wait
