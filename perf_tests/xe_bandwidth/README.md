# Description
xe_bandwidth is a performance micro benchmark suite for measuring bandwidth and latency 
for transfers between Host memory and GPU Device memory.

xe_bandwidth measures the following:
* Host->Device Memory transfer bandwidth in GigaBytes Per Second 
* Host->Device Memory transfer latency in microseconds
* Device->Host Memory transfer bandwidth in GigaBytes Per Second
* Device->Host Memory transfer latency in microseconds

# Features
* Configurable range of transfer size measurements
* Configurable number of iterations per transfer size
* Optional user flag enables verification of first and last byte of every transfer

# Prerequisite
  Requires L0 UMD 
  
# How to Build it
Built as performance test for level_zero_test library

# How to Run it
To run all benchmarks using the default settings: 
```
    $ LD_LIBRARY_PATH ={your path to liblevel_zero.so} ./xe_bandwidth
	
Default Settings:
* Both Host->Device and Device->Host memory transfer measurments performed
* Verification option disabled
* iterations per transfer size = 500
* transfer size range:  1 byte up to 2^30 bytes, with doubling in size per test case

To use command line option features:
 xe_bandwidth [OPTIONS]

 OPTIONS:
  -t, string               selectively run a particular test:
      h2d or H2D                       run only Host-to-Device tests
      d2h or D2H                       run only Device-to-Host tests 
                            [default:  both]
  -v                       enable verificaton
                            [default:  disabled]
  -i                       set number of iterations per transfer
                            [default:  500]
  -s                       select only one transfer size (bytes) 
  -sb                      select beginning transfer size (bytes)
                            [default:  1]
  -se                      select ending transfer size (bytes)
                            [default: 2^30]
  -h, --help               display help message

For example to run a single Host->Device test for transfer_size = 300 bytes, 100 iterations, verification enabled:

 ./xe_bandwidth -t h2d -s 300 -i 100 -v

