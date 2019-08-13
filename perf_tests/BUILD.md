# Level-Zero Performance Tests Build Guide

## Dependencies

Requires the `intel-loki-core`, `intel-loki-devel`, & `opencl-headers` packages
to be installed.

NOTE: to use a local version of intel-loki-core & intel-loki-devel set the
following environment variable:
`L0_ROOT` = `Level_zero_install_directory`

`xe_nano` optionally requires `libpapi-dev`.

### Building Level-Zero Performance Tests

```
cd level_zero_tests/
mkdir build
cd build
cmake ..
cd perf_tests
make -j`nproc`
make install
```

Executables are installed to your CMAKE build directory.
