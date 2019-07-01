# Level-Zero Performance Tests

Performance Benchmarks implemented using the Level-Zero Runtime Library.

## Getting Started

**Prerequisites:**
 * Intel® Graphics 5XX (Skylake) or newer
 * Level-Zero GPU driver
 * Compiler with C++11 support
 * GCC 5.4 or newer
 * Clang 3.8 or newer
 * CMake 3.8 or newer

## Build

Build instructions in [BUILD](BUILD.md) file.

## Running

You must execute each benchmark with your current working directory set to the
one containing that benchmark's `.spv` files (by default your build directory).
Example:

```
$ ls
  xe_nano xe_nano_benchmarks.spv ...
$ ./xe_nano
```

## Maintainers

Instructions on how to configure the git repo & generate source tarballs is
contained in the [MAINTAINERS](MAINTAINERS.md) file.
