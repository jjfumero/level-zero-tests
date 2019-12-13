# Description
xe_image_copy is a performance microbenchmark suite for measuring the bandwidth for copying the image from host to device

# How to Build it
It builds with the L0 repo; Therefore, the steps are the same:
```
    mkdir build; cd build
    cmake -DCMAKE_BUILD_TYPE=Debug
    make -j
```

# How to Run it
To run, use the following command.
```
    cd bin
    ./xe_image_copy --help    [help will provide options to run ]
    ./xe_image_copy  [will run with default parameters]

