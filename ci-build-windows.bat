@echo off
echo machine gerrit-gfx.intel.com >C:\Users\%USERNAME%\_netrc
echo login %GERRITGFX_HTTP_USER% >>C:\Users\%USERNAME%\_netrc
echo password %GERRITGFX_HTTP_PASSWORD% >>C:\Users\%USERNAME%\_netrc
@echo on

call irepo select .\dependencies.yml || exit /b
call irepo sync -c googletest || exit /b
call irepo sync -c level_zero_windows || exit /b
call irepo sync -c yuv_samples || exit /b

mkdir build
cd build
call "%VCInstallDir%\Auxiliary\Build\vcvars64.bat"
set CC=clcache
set CXX=clcache
call cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT=C:\boost -DBoost_COMPILER="-vc142" -DZLIB_ROOT=C:\zlib -DPNG_ROOT=C:\libpng -DOPENCL_ROOT=C:\opencl .. || exit /b
call cmake --build . --config Release --target package || exit /b

cd..
copy /y nul windows_build_succeeded
