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

call cmake ^
  -G Ninja ^
  -D CMAKE_BUILD_TYPE=Release ^
  -D L0_ROOT=%cd%/../third_party/level_zero_windows ^
  -D REQUIRE_LEVELZERO_OPENCL_INTEROP=YES ^
  -D GROUP=/conformance_tests ^
  -D CMAKE_INSTALL_PREFIX=%cd%/../out/windows/conformance_tests ^
  .. || exit /b
call cmake --build . --config Release --target install || exit /b

call cmake ^
  -G Ninja ^
  -D CMAKE_BUILD_TYPE=Release ^
  -D L0_ROOT=%cd%/../third_party/level_zero_windows ^
  -D REQUIRE_LEVELZERO_OPENCL_INTEROP=YES ^
  -D GROUP=/perf_tests ^
  -D CMAKE_INSTALL_PREFIX=%cd%/../out/windows/perf_tests ^
  .. || exit /b
call cmake --build . --config Release --target install || exit /b

cd..
copy /y nul windows_build_succeeded
