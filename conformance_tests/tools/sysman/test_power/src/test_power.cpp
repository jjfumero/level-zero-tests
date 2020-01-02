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
    zetSysmanPowerGetTests,
    GivenValidDeviceWhenRetrievingPowerHandlesThenNonZeroCountAndValidPowerHandlesAreReturned) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t pCount = 0;
    auto pPowerHandles = lzt::get_power_handles(device, pCount);
    for (auto pPowerHandle : pPowerHandles) {
      EXPECT_NE(nullptr, pPowerHandle);
    }
  }
}
TEST(
    zetSysmanPowerGetTests,
    GivenValidDeviceWhenRetrievingPowerHandlesThenSimilarHandlesAreReturnedTwice) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t icount = 0;
    uint32_t lcount = 0;
    auto pPowerHandlesInitial = lzt::get_power_handles(device, icount);
    auto pPowerHandlesLater = lzt::get_power_handles(device, lcount);
    EXPECT_EQ(pPowerHandlesInitial.size(), pPowerHandlesLater.size());
    EXPECT_TRUE(std::equal(pPowerHandlesInitial.begin(),
                           pPowerHandlesInitial.end(),
                           pPowerHandlesLater.begin()));
  }
}
TEST(
    zetSysmanPowerGetTests,
    GivenValidDeviceWhenRetrievingPowerHandlesThenActualHandleCountIsUpdatedAndIfRequestedHandlesAreLessThanActualHandleCountThenDesiredNumberOfHandlesAreReturned) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {

    uint32_t pCount = lzt::get_power_handle_count(device);
    uint32_t tCount = pCount + 1;
    lzt::get_power_handles(device, tCount);
    EXPECT_EQ(tCount, pCount);
    if (pCount > 1) {
      tCount = pCount - 1;
      auto pPowerHandles = lzt::get_power_handles(device, tCount);
      EXPECT_EQ(static_cast<uint32_t>(pPowerHandles.size()), tCount);
    }
  }
}
} // namespace
