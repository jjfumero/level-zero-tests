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

#include "xe_test_harness/xe_test_harness_driver.hpp"
#include "xe_driver.h"

#include "gtest/gtest.h"

namespace level_zero_tests {

void xe_init() { xe_init(XE_INIT_FLAG_NONE); }

void xe_init(xe_init_flag_t init_flag) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeInit(init_flag));
}

uint32_t get_driver_version(xe_device_group_handle_t device_group) {

  uint32_t version = 0;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetDriverVersion(device_group, &version));
  EXPECT_NE(version, 0);

  return version;
}

}; // namespace level_zero_tests
