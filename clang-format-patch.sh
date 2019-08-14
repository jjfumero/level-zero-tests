#! /usr/bin/env bash

set -eu
set -o pipefail

TMP_DIR=$(mktemp -d)
TMP_SRC=$TMP_DIR/src
cp -r $1 $TMP_SRC
FILES=$( \
    find \
    $TMP_SRC/conformance_tests \
    $TMP_SRC/perf_tests \
    $TMP_SRC/utils \
    -name '*.cpp' -or -name '*.hpp' \
)
clang-format-7 -style=file -i $FILES >/dev/null
git -C $TMP_SRC diff
rm -rf $TMP_DIR
