# Description
xe_peak is a performance benchmark suite ported from clpeak which profiles Xe devices to find their peak cpacities for supported functionality.

It is based off the clpeak here licensed with the "unlicense": https://github.com/krrishnarraj/clpeak

xe_peak measures the following:
* Global Memory Bandwidth in GigaBytes Per Second
* Half Precision Compute in GigaFlops
* Single Precision Compute in GigaFlops
* Double Precision Compute in GigaFlops
* Integer Compute in GigaInteger Flops
* Memory Transfer Bandwidth in GigaBytes Per Second
* Kernel Launch Latency in micro seconds
* Kernel Latency in micro seconds

# Prerequisites
* Requires:
  * L0 Packages: intel-loki-core

# How to Build it
Built during the main L0 build command:

Example:

    ...
        mkdir build; cd build
        cmake -DCMAKE_BUILD_TYPE=Debug -DHAVE_TBX_SERVER=ON .
        make -j
    ```

# How to Run it
To run all benchmarks, use the following command. Additional Options and filtering benchmarks are described in the next section.
```
    cd bin
    ./xe_peak
```

# Additional Options
* To look up options available:
```
      $ ./xe_peak -h
        -p, --platform num          choose platform (num starts with 0)
        -d, --device num            choose device   (num starts with 0)
        -e                          time using xe events instead of std chrono timer
                                    hide driver latencies [default: No]
        -t, string                  selectively run a particular test
            global_bw               selectively run global bandwidth test
            hp_compute              selectively run half precision compute test
            sp_compute              selectively run single precision compute test
            dp_compute              selectively run double precision compute test
            int_compute             selectively run integer compute test
            transfer_bw             selectively run transfer bandwidth test
            kernel_lat              selectively run kernel latency test
        -a                          run all above tests [default]
        -v                          enable verbose prints
        -i                          set number of iterations to run[default: 50]
        -w                          set number of warmup iterations to run[default: 10]
        -h, --help                  display help message

```

* Example: Run only the global_bw benchmark:
```
      $ ./xe_peak -global_bw
```
