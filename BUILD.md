# Level-Zero Tests Build Guide

## Dependencies

Dockerfiles are available in the [docker](./docker) directory which can be used
as build environments. These are the environments used by the automation, and
are therefore guaranteed to work. This is the recommended method for building on
Windows and Linux. Refer to the Dockerfile corresponding with your platform for
the exact packages and any other configuration steps required for configuring
your own environment manually.

For building against level-zero, you can either build against the version you
have installed on your system (automatic, Linux only), or specify an install
prefix with the `L0_ROOT` cmake flag during configuration.

Some tests depend on level-zero's OpenCL interop functionality in order to work.
If OpenCL is available on the system (specify a non-standard path with
`OPENCL_ROOT`), then the interop support will be enabled automatically. To
require the interop support, set the `REQUIRE_LEVELZERO_OPENCL_INTEROP` cmake
flag to `YES`.

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
