# Description
xe_nano is a performance benchmark suite for individual function calls. Some of the measurements are latency, instruction count, cycle count, function calls per second. In addition, it's integrated with gtest to allow easy test filtering.

# Prerequisites
* libpapi library on Linux systems is required. Metrics that use hardware counters such as cycle count and instruction count are only supported on Linux systems as the libpapi library is used. If libpapi is not installed in the system, xe_nano will omit hardware counter metrics.
* For xe_nano to access hardware counters, they have to be enabled via a sysfs variable on Linux systems by:
```
    sudo sh -c 'echo -1 >/proc/sys/kernel/perf_event_paranoid'
```

# How to Build it
It builds with the L0 repo; Therefore, the steps are the same:
```
    mkdir build; cd build
    cmake -DCMAKE_BUILD_TYPE=Debug -DHAVE_TBX_SERVER=ON .
    make -j
```

# How to Run it
To run all tests, use the following command. Options for filtering tests are below.
```
    cd bin
    ./xe_nano
```

# Additional Options
* To look up tests available:
```
      $ ./xe_nano --gtest_list_tests
        xeFunctionSetArgumentValue_Buffer
        xeFunctionSetArgumentValue_Immediate
        xeFunctionSetArgumentValue_Image
        xeCommandListAppendLaunchFunction
        xeCommandQueueExecuteCommandLists
```

* To filter tests available:
```
      $ ./xe_nano --gtest_filter=*xeFunctionSetArgumentValue*
```
