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

#include "gtest/gtest.h"

#include "logging/logging.hpp"
#include "ze_api.h"
#include "test_harness/test_harness.hpp"

namespace lzt = level_zero_tests;

namespace {

TEST(
    zeInitTests,
    GivenDriverWasAlreadyInitializedWhenInitializingDriverThenSuccessIsReturned) {
  for (int i = 0; i < 5; ++i) {
    lzt::ze_init();
  }
}

TEST(zeDriverGetDriverVersionTests,
     GivenZeroVersionWhenGettingDriverVersionThenNonZeroVersionIsReturned) {

  lzt::ze_init();

  auto drivers = lzt::get_all_driver_handles();
  for (auto driver : drivers) {
    uint32_t version = lzt::get_driver_version(driver);
    LOG_INFO << "Driver version: " << version;
  }
}

TEST(zeDriverGetApiVersionTests,
     GivenValidDriverWhenRetrievingApiVersionThenValidApiVersionIsReturned) {
  lzt::ze_init();

  auto drivers = lzt::get_all_driver_handles();
  for (auto driver : drivers) {
    ze_api_version_t api_version = lzt::get_api_version(driver);
    LOG_INFO << "API version: " << api_version;
  }
}

TEST(zeDriverGetIPCPropertiesTests,
     GivenValidDriverWhenRetrievingIPCPropertiesThenValidPropertiesAreRetured) {

  auto drivers = lzt::get_all_driver_handles();
  ASSERT_GT(drivers.size(), 0);
  for (auto driver : drivers) {
    lzt::get_ipc_properties(driver);
  }
}

} // namespace
