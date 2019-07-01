# Level-Zero Performance Tests Build Guide

## Dependencies

Requires the `intel-loki-core`, `intel-loki-devel`, & `opencl-headers` packages
to be installed.

`xe_nano` optionally requires `libpapi-dev`.

### Building Level-Zero Performance Tests

```
mkdir build
cd build
cmake ..
make -j`nproc`
make install
```

Executables are installed to your CMAKE build directory.
