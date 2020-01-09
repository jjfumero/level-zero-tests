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
TEST(zetSysmanPowerGetTests,
     GivenSamePowerHandleWhenRequestingPowerPropertiesThenExpectValidMaxLimit) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t pCount = 0;
    auto pPowerHandles = lzt::get_power_handles(device, pCount);
    for (auto pPowerHandle : pPowerHandles) {
      EXPECT_NE(nullptr, pPowerHandle);
      auto properties = lzt::get_power_properties(pPowerHandle);
      EXPECT_GT(properties.maxLimit, 0);
    }
  }
}

TEST(
    zetSysmanPowerGetTests,
    GivenSamePowerHandleWhenRequestingPowerPropertiesThenExpectSamePropertiesTwice) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t pCount = 0;
    auto pPowerHandles = lzt::get_power_handles(device, pCount);
    for (auto pPowerHandle : pPowerHandles) {
      EXPECT_NE(nullptr, pPowerHandle);
      auto propertiesInitial = lzt::get_power_properties(pPowerHandle);
      auto propertiesLater = lzt::get_power_properties(pPowerHandle);
      EXPECT_EQ(propertiesInitial.onSubdevice, propertiesLater.onSubdevice);
      EXPECT_EQ(propertiesInitial.subdeviceId, propertiesLater.subdeviceId);
      EXPECT_EQ(propertiesInitial.canControl, propertiesLater.canControl);
      EXPECT_EQ(propertiesInitial.isEnergyThresholdSupported,
                propertiesLater.isEnergyThresholdSupported);
      EXPECT_EQ(propertiesInitial.maxLimit, propertiesLater.maxLimit);
    }
  }
}

TEST(
    zetSysmanPowerGetLimitsTests,
    GivenValidPowerHandleWhenRequestingPowerLimitsThenExpectzetSysmanPowerGetLimitsToReturnValidPowerLimits) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t pCount = 0;
    auto pPowerHandles = lzt::get_power_handles(device, pCount);
    for (auto pPowerHandle : pPowerHandles) {
      EXPECT_NE(nullptr, pPowerHandle);
      zet_power_sustained_limit_t power_sustained_limit;
      zet_power_burst_limit_t power_burst_limit;
      zet_power_peak_limit_t power_peak_limit;
      lzt::get_power_limits(pPowerHandle, &power_sustained_limit,
                            &power_burst_limit, &power_peak_limit);
      auto properties = lzt::get_power_properties(pPowerHandle);
      EXPECT_LE(power_sustained_limit.power, power_burst_limit.power);
      EXPECT_LE(power_burst_limit.power, power_peak_limit.powerAC);
      EXPECT_LE(power_peak_limit.powerAC, properties.maxLimit);
    }
  }
}
TEST(
    zetSysmanPowerGetLimitsTests,
    GivenValidPowerHandleWhenRequestingPowerLimitsThenExpectzetSysmanPowerGetLimitsToReturnSameValuesTwice) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t pCount = 0;
    auto pPowerHandles = lzt::get_power_handles(device, pCount);
    for (auto pPowerHandle : pPowerHandles) {
      EXPECT_NE(nullptr, pPowerHandle);
      zet_power_sustained_limit_t power_sustained_limit_Initial;
      zet_power_burst_limit_t power_burst_limit_Initial;
      zet_power_peak_limit_t power_peak_limit_Initial;
      lzt::get_power_limits(pPowerHandle, &power_sustained_limit_Initial,
                            &power_burst_limit_Initial,
                            &power_peak_limit_Initial);
      zet_power_sustained_limit_t power_sustained_limit_Later;
      zet_power_burst_limit_t power_burst_limit_Later;
      zet_power_peak_limit_t power_peak_limit_Later;
      lzt::get_power_limits(pPowerHandle, &power_sustained_limit_Later,
                            &power_burst_limit_Later, &power_peak_limit_Later);

      EXPECT_EQ(power_sustained_limit_Initial.enabled,
                power_sustained_limit_Later.enabled);
      EXPECT_EQ(power_sustained_limit_Initial.power,
                power_sustained_limit_Later.power);
      EXPECT_EQ(power_sustained_limit_Initial.interval,
                power_sustained_limit_Later.interval);
      EXPECT_EQ(power_burst_limit_Initial.enabled,
                power_burst_limit_Later.enabled);
      EXPECT_EQ(power_burst_limit_Initial.power, power_burst_limit_Later.power);
      EXPECT_EQ(power_peak_limit_Initial.powerAC,
                power_peak_limit_Later.powerAC);
      EXPECT_EQ(power_peak_limit_Initial.powerDC,
                power_peak_limit_Later.powerDC);
    }
  }
}
TEST(
    zetSysmanPowerSetLimitsTests,
    GivenValidPowerHandleWhenSettingPowerValuesThenExpectzetSysmanPowerSetLimitsFollowedByzetSysmanPowerGetLimitsToMatch) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t pCount = 0;
    auto pPowerHandles = lzt::get_power_handles(device, pCount);
    for (auto pPowerHandle : pPowerHandles) {
      EXPECT_NE(nullptr, pPowerHandle);
      zet_power_sustained_limit_t power_sustained_limit_Initial;
      zet_power_burst_limit_t power_burst_limit_Initial;
      zet_power_peak_limit_t power_peak_limit_Initial;
      lzt::get_power_limits(
          pPowerHandle, &power_sustained_limit_Initial,
          &power_burst_limit_Initial,
          &power_peak_limit_Initial); // get default power values
      auto properties = lzt::get_power_properties(pPowerHandle);
      zet_power_sustained_limit_t power_sustained_limit_set;
      zet_power_burst_limit_t power_burst_limit_set;
      zet_power_peak_limit_t power_peak_limit_set;
      if (power_sustained_limit_Initial.enabled == true)
        power_sustained_limit_set.enabled = false;
      else
        power_sustained_limit_set.enabled = true;
      power_sustained_limit_set.power = properties.maxLimit;
      power_sustained_limit_set.interval =
          power_sustained_limit_Initial.interval;
      if (power_burst_limit_Initial.enabled == true)
        power_burst_limit_set.enabled = false;
      else
        power_burst_limit_set.enabled = true;
      power_burst_limit_set.power = properties.maxLimit;
      power_peak_limit_set.powerAC = properties.maxLimit;
      power_peak_limit_set.powerDC = power_peak_limit_Initial.powerDC;
      lzt::set_power_limits(pPowerHandle, &power_sustained_limit_set,
                            &power_burst_limit_set,
                            &power_peak_limit_set); // Set power values
      zet_power_sustained_limit_t power_sustained_limit_get;
      zet_power_burst_limit_t power_burst_limit_get;
      zet_power_peak_limit_t power_peak_limit_get;
      lzt::get_power_limits(pPowerHandle, &power_sustained_limit_get,
                            &power_burst_limit_get,
                            &power_peak_limit_get); // Get power values
      EXPECT_EQ(power_sustained_limit_get.enabled,
                power_sustained_limit_set.enabled);
      EXPECT_EQ(power_sustained_limit_get.power,
                power_sustained_limit_set.power);
      EXPECT_EQ(power_sustained_limit_get.interval,
                power_sustained_limit_set.interval);
      EXPECT_EQ(power_burst_limit_get.enabled, power_burst_limit_set.enabled);
      EXPECT_EQ(power_burst_limit_get.power, power_burst_limit_set.power);
      EXPECT_EQ(power_peak_limit_get.powerAC, power_peak_limit_set.powerAC);
      EXPECT_EQ(
          power_peak_limit_get.powerDC,
          power_peak_limit_set.powerDC); // Verify whether values match or not
      lzt::set_power_limits(pPowerHandle, &power_sustained_limit_Initial,
                            &power_burst_limit_Initial,
                            &power_peak_limit_Initial); // Set values to default
    }
  }
}
} // namespace
