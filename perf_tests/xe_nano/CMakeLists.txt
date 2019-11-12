# INTEL CONFIDENTIAL
# Copyright (c) 2019 Intel Corporation. All Rights Reserved.
#
# The source code contained or described herein and all documents related to the
# source code ("Material") are owned by Intel Corporation or its suppliers
# or licensors. Title to the Material remains with Intel Corporation or its
# suppliers and licensors. The Material contains trade secrets and proprietary
# and confidential information of Intel or its suppliers and licensors. The
# Material is protected by worldwide copyright and trade secret laws and
# treaty provisions. No part of the Material may be used, copied, reproduced,
# modified, published, uploaded, posted, transmitted, distributed, or
# disclosed in any way without Intel's prior express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or delivery
# of the Materials, either expressly, by implication, inducement, estoppel or
# otherwise. Any license under such intellectual property rights must be
# express and approved by Intel in writing.

set(OS_SPECIFIC_LIBS "")
set(XE_NANO_HWCOUNTER_SRC src/hardware_counter/hardware_counter_stub.cpp)
if(UNIX)
    set(PAPI_LIB papi)
    find_library(PAPI_LIB_PATH ${PAPI_LIB})
    if(PAPI_LIB_PATH)
      set(XE_NANO_HWCOUNTER_SRC src/hardware_counter/hardware_counter_support.cpp)
      set(OS_SPECIFIC_LIBS ${OS_SPECIFIC_LIBS} ${PAPI_LIB})
      add_definitions(-DWITH_PAPI)
      message(STATUS "Found PAPI library: ${PAPI_LIB}")
    else()
      message(STATUS "PAPI library not found, hardware counter support disabled")
    endif()
else()
endif()

add_lzt_test(
  NAME xe_nano
  GROUP "/perf_tests"
  SOURCES
    ../common/src/xe_app.cpp
    src/api_static_probe.cpp
    ${XE_NANO_HWCOUNTER_SRC}
    src/xe_nano.cpp
    src/benchmark.cpp
  LINK_LIBRARIES
    ${OS_SPECIFIC_LIBS}
    gmock
  KERNELS xe_nano_benchmarks
)
