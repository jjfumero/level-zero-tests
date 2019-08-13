# Level-Zero Conformance Tests

Conformance test content for validating all features of the Level Zero Runtime Library.

## Getting Started

**Prerequisites:**
 * Intel® Graphics 5XX (Skylake) or newer
 * Level-Zero GPU driver
 * Compiler with C++11 support
 * GCC 5.4 or newer
 * Clang 3.8 or newer
 * CMake 3.8 or newer

## Build

Build instructions in [BUILD](BUILD.md) file.

## Running

**Executing the conformance tests on HW:**
 * Execute each test individually
    * Set export LD_LIBRARY_PATH= "path to Level_zero.so"
    * sudo ./test_xe_<filename>
 * Execute all the tests using "ctest"
    * Set export LD_LIBRARY_PATH= "path to Level_zero.so"
    * Copy your folder and all sub folders: "build/compute_samples/tests/embargo"
    * inside "embargo" run ./ctest
    * This will run all the Level Zero conformance tests

**Executing the conformance tests on fulsim:**
 * Execute each test individually
    * Set LD_LIBRARY_PATH= "path to Level_zero.so"
        * GEN9 fulsim SKL:
            * export SetCommandStreamReceiver=2;export ProductFamilyOverride=skl
        * ATS Fulsim:
            * export SetCommandStreamReceiver=2;export ProductFamilyOverride=ats;export EnableLocalMemory=1
        * ./test_xe<filename>
    * NOTE: Execution of all the test using "ctest" does not work due to TBX failing to reset fast enough.