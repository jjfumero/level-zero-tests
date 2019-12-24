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
#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"

namespace lzt = level_zero_tests;

#include "ze_api.h"
#include "zet_api.h"

namespace {

TEST(
    zetSysmanStandbyGetTests,
    GivenValidDeviceWhenRetrievingStandbyHandlesThenNonZeroCountAndValidStandbyHandlesAreReturned) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pStandbyHandles = lzt::get_standby_handles(device);
    for (auto pStandbyHandle : pStandbyHandles) {
      EXPECT_NE(nullptr, pStandbyHandle);
    }
  }
}
TEST(
    zetSysmanStandbyGetTests,
    GivenValidDeviceWhenRetrievingStandbyHandlesThenSimilarHandlesAreReturnedTwice) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pStandbyHandlesInitial = lzt::get_standby_handles(device);
    auto pStandbyHandlesLater = lzt::get_standby_handles(device);
    EXPECT_EQ(pStandbyHandlesInitial.size(), pStandbyHandlesLater.size());
    EXPECT_TRUE(std::equal(pStandbyHandlesInitial.begin(),
                           pStandbyHandlesInitial.end(),
                           pStandbyHandlesLater.begin()));
  }
}
TEST(
    zetSysmanStandbyGetTests,
    GivenValidDeviceWhenRetrievingStandbyHandlesThenActualHandleCountIsUpdated) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t pCount = lzt::get_standby_handle_count(device);
    uint32_t tCount = pCount + 1;
    tCount = lzt::get_standby_handle_count(device, tCount);
    EXPECT_EQ(tCount, pCount);
  }
}
TEST(
    zetSysmanStandyGetModeTests,
    GivenValidDeviceWhenRequestingModeThenExpectzetSysmanStandbyGetModeToReturnValidStandbyMode) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pHandles = lzt::get_standby_handles(device);
    for (auto pHandle : pHandles) {
      EXPECT_NE(nullptr, pHandle);
      zet_standby_promo_mode_t standByMode;
      standByMode = lzt::get_standby_mode(pHandle);
      switch (standByMode) {
      case ZET_STANDBY_PROMO_MODE_DEFAULT:
        SUCCEED();
        break;
      case ZET_STANDBY_PROMO_MODE_NEVER:
        SUCCEED();
        break;
      default:
        FAIL();
      }
    }
  }
}
} // namespace
