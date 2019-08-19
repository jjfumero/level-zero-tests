# Level-Zero Tests Build Guide

## Dependencies

Dockerfiles are available in the [docker](./docker) directory which can be used
as build environments. These are the environments used by the automation, and
are therefore guaranteed to work. This is the recommended method for building on
Windows and Linux. Refer to the Dockerfile corresponding with your platform if
you want to configure your own environment manually.

For building against level-zero, you can either build against the version you
have installed on your system (automatic, Linux only), or specify an install
prefix with the `L0_ROOT` cmake flag during configuration.

## Building

This project uses cmake to configure the build. The `install` target will create
an `out` directory in your cmake build directory containing the built
performance test and the conformance test binaries (e.g.,
`build/out/perf_tests/` and `build/out/conformance_tests/`). Nothing will get
installed to your system.

```
mkdir build
cd build
cmake ..
cmake --build . --config Release --target install
```
