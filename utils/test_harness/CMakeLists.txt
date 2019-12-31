#
# INTEL CONFIDENTIAL
#
# Copyright 2019 Intel Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code (Material) are owned by Intel Corporation or its
# suppliers or licensors.
#
# Title to the Material remains with Intel Corporation or its suppliers and
# licensors. The Material contains trade secrets and proprietary and
# confidential information of Intel or its suppliers and licensors.
# The Material is protected by worldwide copyright and trade secret laws and
# treaty provisions. No part of the Material may be used, copied, reproduced,
# modified, published, uploaded, posted, transmitted, distributed,
# or disclosed in any way without Intel's prior express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure
# or delivery of the Materials, either expressly, by implication, inducement,
# estoppel or otherwise. Any license under such intellectual property rights
# must be express and approved by Intel in writing.
#

add_core_library(test_harness
    SOURCE
    # "include/test_harness/test_harness.hpp"
    # "include/test_harness/test_harness_device.hpp"
    # "include/test_harness/test_harness_driver.hpp"
    # "include/test_harness/test_harness_cmdlist.hpp"
    # "include/test_harness/test_harness_cmdqueue.hpp"
    # "include/test_harness/test_harness_event.hpp"
    # "include/test_harness/test_harness_memory.hpp"
    # "include/test_harness/test_harness_image.hpp"
    # "include/test_harness/test_harness_fence.hpp"
    # "include/test_harness/test_harness_module.hpp"
    # "tools/include/test_harness_api_tracing.hpp"
    # "tools/sysman/include/test_harness_sysman.h"
    "src/test_harness_device.cpp"
    "src/test_harness_driver.cpp"
    "src/test_harness_cmdlist.cpp"
    "src/test_harness_cmdqueue.cpp"
    "src/test_harness_event.cpp"
    "src/test_harness_memory.cpp"
    "src/test_harness_image.cpp"
    "src/test_harness_fence.cpp"
    "src/test_harness_module.cpp"
    "src/test_harness_sampler.cpp"
    "src/test_harness_ocl_interop.cpp"
    "tools/src/test_harness_api_tracing.cpp"
    "tools/sysman/src/test_harness_sysman_frequency.cpp"
    "tools/sysman/src/test_harness_sysman_standby.cpp"
    "tools/sysman/src/test_harness_sysman_init.cpp"
    "tools/sysman/src/test_harness_sysman_pci.cpp"
)
target_link_libraries(test_harness
    PUBLIC
    GTest::GTest
    level_zero_tests::image
    level_zero_tests::logging
    level_zero_tests::utils
    LevelZero::LevelZero
)
target_include_directories(test_harness
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
