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
```

## Running

You must execute each benchmark with your current working directory set to the
one containing that benchmark's `.spv` files.

No useful means of installation are supported yet, so just execute the
benchmarks in-place from their build directories (which contains the `.spv`
files and the executable itself), e.g.:

```
cd build/perf_tests/xe_nano
./xe_nano
```