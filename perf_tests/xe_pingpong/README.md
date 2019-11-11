# Description
xe_pingpong is a performance benchmark suite ported from clpingpong which measures round-trip delay times for a light-weight kernel execution of 32-bit addition on a 32-bit integer followed by 32-bit subtraction of 32-bit integer on host CPU.  Transfers or mappings of the kernel 32-bit integer to the host CPU add to the delay.

It is based off the clpingpong here licensed with the MIT License:  https://github.com/ekondis/clpingpong

xe_pingpong measures the following:
* Kernel execution time for integer argument in Device Memory
* Kernel execution time for integer argument in Host Memory
* Kernel execution time for integer argument in Shared Memory
* Round-trip (ping-pong) time for kernel integer argument in Device Memory and transfer to Host for decrement
* Round-trip time for kernel integer argument in Host Memory and decrement in Host
* Round-trip time for kernel integer argument in Shared Memory and memcpy to Host for decrement (Note:  this is intended to resemeble the OpenCL mapping operation)
* Host overhead for transfer/mapping operations

# Prerequisite
  Requires L0 UMD 
  
# How to Build it
Built as performance test for level_zero_test library

# How to Run it
To run all benchmarks, use the following command. 
```
    $ LD_LIBRARY_PATH ={your path to liblevel_zero.so} ./xe_pingpong
```

