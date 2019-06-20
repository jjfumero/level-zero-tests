# Level-Zero Performance Tests

## Preparing Source Repository for Building

```
irepo select linux.yml
irepo sync
```

## Building

Requires the `intel-loki-core` and `intel-loki-devel` packages to be installed.

`xe_nano` optionally requires `libpapi-dev`.

```
mkdir build
cd build
cmake ..
make -j`nproc`
make install
```

Executables are installed to your CMAKE build directory.

## Running

You must execute each benchmark with your current working directory set to the
one containing that benchmark's `.spv` files (by default your build directory).
