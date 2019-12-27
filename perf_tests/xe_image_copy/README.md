# Description
xe_image_copy is a performance microbenchmark for measuring the image transfer bandwidth between Host memory and GPU Device memory.

xe_image_copy  measures image (size is configurable via commandline)  transfer bandwidth for the following:
* From Host to Device  in GigaBytes Per Second 
* From Device to Host in GigaBytes Per Second
* From Host to Device to Host in GigaBytes per Second

# Features
* Configurable image width,height,depth,xoffset,yoffset,zoffset
* Configurable number of iterations per image transfer

# Prerequisite
  Requires L0 UMD 
  
# How to Build it
Built as performance test for level_zero_test library

# How to Run it
To run all benchmarks using the default settings: 
```
    $ LD_LIBRARY_PATH ={your path to liblevel_zero.so} ./xe_image_copy
	
Default Settings:
* Both Host->Device and Device->Host image transfer bandwidth measurments performed 
* Host->Device->Host image tranfer bandwdith measurment performed
* Default image size is 2048x2048x1
* Default iterations per image transfer = 50

To use command line option features:
 xe_image_copy [OPTIONS]

 OPTIONS:
   --help                     produce help message
  -w [ --width ]              set image width (by default it is 2048)
  -h [ --height ]             set image height (by default it is 2048)
  -d [ --depth ]              set image depth (by default it is 1
  --offx                      set image xoffset (by default it is 0)
  --offy                      set image yoffset (by default it is 0)
  --offz                      set image zoffset (by default it is 0)
  --warmup                    set number of warmup operations (by default it is 10)
  --iter                      set number of iterations (by default it is 50)

For example to run a xe_image_copy with width 1024 height 1024:

 ./xe_image_copy -w 1024 -h 1024

