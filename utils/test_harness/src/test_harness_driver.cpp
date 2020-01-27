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

#include "test_harness/test_harness_driver.hpp"
#include "ze_api.h"

#include "gtest/gtest.h"

namespace level_zero_tests {

void ze_init() { ze_init(ZE_INIT_FLAG_NONE); }

void ze_init(ze_init_flag_t init_flag) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInit(init_flag));
}

uint32_t get_driver_version(ze_driver_handle_t driver) {

  uint32_t version = 0;
  ze_driver_properties_t properties;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDriverGetProperties(driver, &properties));
  version = properties.version;
  EXPECT_NE(version, 0);

  return version;
}

ze_api_version_t get_api_version(ze_driver_handle_t driver) {
  ze_api_version_t api_version;

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDriverGetApiVersion(driver, &api_version));
  return api_version;
}

ze_driver_ipc_properties_t get_ipc_properties(ze_driver_handle_t driver) {
  ze_driver_ipc_properties_t properties = {
      ZE_DRIVER_IPC_PROPERTIES_VERSION_CURRENT};

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDriverGetIPCProperties(driver, &properties));

  return properties;
}

}; // namespace level_zero_tests
