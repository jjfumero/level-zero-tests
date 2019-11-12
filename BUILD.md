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

This project uses cmake to configure the build. By default, all the tests are
built.

The `install` target will by default create an `out` directory in your cmake
build directory containing the built test executables and their data files.
Nothing will get installed to any system paths. You can override the default
install location by setting `CMAKE_INSTALL_PREFIX`.

```
mkdir build
cd build
cmake -D CMAKE_INSTALL_PREFIX=$PWD/../out ..
cmake --build . --config Release --target install
```

### Building a subset of the test executables

Test executables are divided into a group hierarchy, and it is possible to
select a specific grouping of test executables for build using the `GROUP`
cmake flag. The following group specifiers are available:

  - `/`: All tests.
  - `/perf_tests`: All the performance tests.
  - `/conformance_tests`: All the conformance tests.
  - `/conformance_tests/core`: All of the conformance tests for the core API.
  - `/conformance_tests/tools`: All of the conformance tests for the tools API.
  - `/conformance_tests/tools/tracing`: All of the tools API conformance tests
    related to tracing.
  - `/conformance_tests/tools/sysman`: ALl of the tools API conformance tests
    relating to system management.

```
cmake
  -D GROUP=/perf_tests
  -D CMAKE_INSTALL_PREFIX=$PWD/../out/perf_tests
  ..
cmake --build . --config Release --target install
```

The group that any particular test executable belongs to can be seen by looking
in its `CMakeLists.txt` file at the `add_lzt_test()` call.
