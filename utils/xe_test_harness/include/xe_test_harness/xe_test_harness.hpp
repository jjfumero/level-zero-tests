/*
 * INTEL CONFIDENTIAL
 *
 * Copyright 2019 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code (Material) are owned by Intel Corporation or its
 * suppliers or licensors.
 *
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors. The Material contains trade secrets and proprietary and
 * confidential information of Intel or its suppliers and licensors.
 * The Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure
 * or delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 */

#ifndef level_zero_tests_ZE_TEST_HARNESS_HPP
#define level_zero_tests_ZE_TEST_HARNESS_HPP
#include "gtest/gtest.h"

#include "xe_test_harness_driver.hpp"
#include "xe_test_harness_device.hpp"
#include "xe_test_harness_cmdqueue.hpp"
#include "xe_test_harness_cmdlist.hpp"
#include "xe_test_harness_event.hpp"
#include "xe_test_harness_memory.hpp"
#include "xe_test_harness_image.hpp"
#include "xe_test_harness_module.hpp"
#include "xe_test_harness_sampler.hpp"

class zeEventPoolCommandListTests : public ::testing::Test {
protected:
  level_zero_tests::zeEventPool ep;
  level_zero_tests::zeCommandList cl;
};

#endif
