# Description
xe_peer is a performance benchmark suite for measuing peer-to-peer bandwidth
and latency.

# How to Build it
It builds with the L0 repo; Therefore, the steps are the same:
```
    mkdir build; cd build
    cmake -DCMAKE_BUILD_TYPE=Debug -DHAVE_TBX_SERVER=ON .
    make -j
```

# How to Run it
To run, use the following command.
```
    cd bin
    ./xe_peer
```
