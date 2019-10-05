@echo on

call dt workspace select .\dependencies.yml || exit /b
call dt workspace sync -c googletest || exit /b
call dt workspace sync -c level_zero_windows || exit /b
call dt workspace sync -c yuv_samples || exit /b

mkdir build
cd build
call "%VCInstallDir%\Auxiliary\Build\vcvars64.bat"
set CC=clcache
set CXX=clcache
call cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DL0_ROOT=%cd%/../third_party/level_zero_windows -DREQUIRE_LEVELZERO_OPENCL_INTEROP=YES -DREQUIRE_LEVELZERO_OPENCL_INTEROP=NO -DBOOST_ROOT=C:\boost -DBoost_COMPILER="-vc" -DBOOST_LIB_TOOLSET="vc" -DZLIB_ROOT=C:\zlib -DPNG_ROOT=C:\libpng -DOPENCL_ROOT=C:\opencl .. || exit /b
call cmake --build . --config Release --target install || exit /b

cd..
copy /y nul windows_build_succeeded
