# Level-Zero Tests Build Guide

## Dependencies

Requires the `intel-loki-core`, `intel-loki-devel`, `opencl-headers`, 
`libpng-dev`, `ocl-icd-opencl-dev`, `libboost-all-dev`, & `libva-dev` packages
to be installed.

NOTE: to use a local version of intel-loki-core & intel-loki-devel set the
following environment variable:
`L0_ROOT` = `Level_zero_install_directory`

`xe_nano` optionally requires `libpapi-dev`.


### Building Level-Zero Tests

```
mkdir build
cd build
cmake ..
make -j`nproc`
make install
```

Executables are installed to your CMAKE build directory.
