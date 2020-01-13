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
#include <thread>
#include "logging/logging.hpp"
#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"
#include <chrono>
namespace lzt = level_zero_tests;

#include "ze_api.h"
#include "zet_api.h"

namespace {

TEST(
    zetSysmanPowerGetTests,
    GivenValidDeviceWhenRetrievingPowerHandlesThenNonZeroCountAndValidPowerHandlesAreReturned) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t count = 0;
    auto pPowerHandles = lzt::get_power_handles(device, count);
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

    uint32_t pcount = lzt::get_power_handle_count(device);
    uint32_t tcount = pcount + 1;
    lzt::get_power_handles(device, tcount);
    EXPECT_EQ(tcount, pcount);
    if (pcount > 1) {
      tcount = pcount - 1;
      auto pPowerHandles = lzt::get_power_handles(device, tcount);
      EXPECT_EQ(static_cast<uint32_t>(pPowerHandles.size()), tcount);
    }
  }
}
TEST(zetSysmanPowerGetTests,
     GivenSamePowerHandleWhenRequestingPowerPropertiesThenExpectValidMaxLimit) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t count = 0;
    auto pPowerHandles = lzt::get_power_handles(device, count);
    for (auto pPowerHandle : pPowerHandles) {
      EXPECT_NE(nullptr, pPowerHandle);
      auto pProperties = lzt::get_power_properties(pPowerHandle);
      EXPECT_GT(pProperties.maxLimit, 0);
    }
  }
}

TEST(
    zetSysmanPowerGetTests,
    GivenSamePowerHandleWhenRequestingPowerPropertiesThenExpectSamePropertiesTwice) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t count = 0;
    auto pPowerHandles = lzt::get_power_handles(device, count);
    for (auto pPowerHandle : pPowerHandles) {
      EXPECT_NE(nullptr, pPowerHandle);
      auto pPropertiesInitial = lzt::get_power_properties(pPowerHandle);
      auto pPropertiesLater = lzt::get_power_properties(pPowerHandle);
      EXPECT_EQ(pPropertiesInitial.onSubdevice, pPropertiesLater.onSubdevice);
      EXPECT_EQ(pPropertiesInitial.subdeviceId, pPropertiesLater.subdeviceId);
      EXPECT_EQ(pPropertiesInitial.canControl, pPropertiesLater.canControl);
      EXPECT_EQ(pPropertiesInitial.isEnergyThresholdSupported,
                pPropertiesLater.isEnergyThresholdSupported);
      EXPECT_EQ(pPropertiesInitial.maxLimit, pPropertiesLater.maxLimit);
    }
  }
}

TEST(
    zetSysmanPowerGetLimitsTests,
    GivenValidPowerHandleWhenRequestingPowerLimitsThenExpectzetSysmanPowerGetLimitsToReturnValidPowerLimits) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t count = 0;
    auto pPowerHandles = lzt::get_power_handles(device, count);
    for (auto pPowerHandle : pPowerHandles) {
      EXPECT_NE(nullptr, pPowerHandle);
      zet_power_sustained_limit_t pSustained;
      zet_power_burst_limit_t pBurst;
      zet_power_peak_limit_t pPeak;
      lzt::get_power_limits(pPowerHandle, &pSustained, &pBurst, &pPeak);
      auto pProperties = lzt::get_power_properties(pPowerHandle);
      EXPECT_LE(pSustained.power, pBurst.power);
      EXPECT_LE(pBurst.power, pPeak.powerAC);
      EXPECT_LE(pPeak.powerAC, pProperties.maxLimit);
    }
  }
}
TEST(
    zetSysmanPowerGetLimitsTests,
    GivenValidPowerHandleWhenRequestingPowerLimitsThenExpectzetSysmanPowerGetLimitsToReturnSameValuesTwice) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t count = 0;
    auto pPowerHandles = lzt::get_power_handles(device, count);
    for (auto pPowerHandle : pPowerHandles) {
      EXPECT_NE(nullptr, pPowerHandle);
      zet_power_sustained_limit_t pSustainedInitial;
      zet_power_burst_limit_t pBurstInitial;
      zet_power_peak_limit_t pPeakInitial;
      lzt::get_power_limits(pPowerHandle, &pSustainedInitial, &pBurstInitial,
                            &pPeakInitial);
      zet_power_sustained_limit_t pSustainedLater;
      zet_power_burst_limit_t pBurstLater;
      zet_power_peak_limit_t pPeakLater;
      lzt::get_power_limits(pPowerHandle, &pSustainedLater, &pBurstLater,
                            &pPeakLater);

      EXPECT_EQ(pSustainedInitial.enabled, pSustainedLater.enabled);
      EXPECT_EQ(pSustainedInitial.power, pSustainedLater.power);
      EXPECT_EQ(pSustainedInitial.interval, pSustainedLater.interval);
      EXPECT_EQ(pBurstInitial.enabled, pBurstLater.enabled);
      EXPECT_EQ(pBurstInitial.power, pBurstLater.power);
      EXPECT_EQ(pPeakInitial.powerAC, pPeakLater.powerAC);
      EXPECT_EQ(pPeakInitial.powerDC, pPeakLater.powerDC);
    }
  }
}
TEST(
    zetSysmanPowerSetLimitsTests,
    GivenValidPowerHandleWhenSettingPowerValuesThenExpectzetSysmanPowerSetLimitsFollowedByzetSysmanPowerGetLimitsToMatch) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t count = 0;
    auto pPowerHandles = lzt::get_power_handles(device, count);
    for (auto pPowerHandle : pPowerHandles) {
      EXPECT_NE(nullptr, pPowerHandle);
      zet_power_sustained_limit_t pSustainedInitial;
      zet_power_burst_limit_t pBurstInitial;
      zet_power_peak_limit_t pPeakInitial;
      lzt::get_power_limits(pPowerHandle, &pSustainedInitial, &pBurstInitial,
                            &pPeakInitial); // get default power values
      auto pProperties = lzt::get_power_properties(pPowerHandle);
      zet_power_sustained_limit_t pSustainedSet;
      zet_power_burst_limit_t pBurstSet;
      zet_power_peak_limit_t pPeakSet;
      if (pSustainedInitial.enabled == true)
        pSustainedSet.enabled = false;
      else
        pSustainedSet.enabled = true;
      pSustainedSet.power = pProperties.maxLimit;
      pSustainedSet.interval = pSustainedInitial.interval;
      if (pBurstInitial.enabled == true)
        pBurstSet.enabled = false;
      else
        pBurstSet.enabled = true;
      pBurstSet.power = pProperties.maxLimit;
      pPeakSet.powerAC = pProperties.maxLimit;
      pPeakSet.powerDC = pPeakInitial.powerDC;
      lzt::set_power_limits(pPowerHandle, &pSustainedSet, &pBurstSet,
                            &pPeakSet); // Set power values
      zet_power_sustained_limit_t pSustainedGet;
      zet_power_burst_limit_t pBurstGet;
      zet_power_peak_limit_t pPeakGet;
      lzt::get_power_limits(pPowerHandle, &pSustainedGet, &pBurstGet,
                            &pPeakGet); // Get power values
      EXPECT_EQ(pSustainedGet.enabled, pSustainedSet.enabled);
      EXPECT_EQ(pSustainedGet.power, pSustainedSet.power);
      EXPECT_EQ(pSustainedGet.interval, pSustainedSet.interval);
      EXPECT_EQ(pBurstGet.enabled, pBurstSet.enabled);
      EXPECT_EQ(pBurstGet.power, pBurstSet.power);
      EXPECT_EQ(pPeakGet.powerAC, pPeakSet.powerAC);
      EXPECT_EQ(pPeakGet.powerDC,
                pPeakSet.powerDC); // Verify whether values match or not
      lzt::set_power_limits(pPowerHandle, &pSustainedInitial, &pBurstInitial,
                            &pPeakInitial); // Set values to default
    }
  }
}
TEST(
    zetSysmanPowerGetEnergyCounterTests,
    GivenValidPowerHandleThenExpectzetSysmanPowerGetEnergyCounterToReturnSuccess) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t count = 0;
    auto pPowerHandles = lzt::get_power_handles(device, count);
    for (auto pPowerHandle : pPowerHandles) {
      EXPECT_NE(nullptr, pPowerHandle);
      zet_power_energy_counter_t pEnergyCounter;
      lzt::get_power_energy_counter(pPowerHandle, &pEnergyCounter);
      uint64_t energy_initial = pEnergyCounter.energy;
      uint64_t timestamp_initial = pEnergyCounter.timestamp;
      std::this_thread::sleep_for(std::chrono::milliseconds(2));
      lzt::get_power_energy_counter(pPowerHandle, &pEnergyCounter);
      uint64_t energy_later = pEnergyCounter.energy;
      uint64_t timestamp_later = pEnergyCounter.timestamp;
      EXPECT_GE(energy_later, energy_initial);
      EXPECT_NE(timestamp_later, timestamp_initial);
    }
  }
}
TEST(
    zetSysmanPowerGetEnergyThresholdTests,
    GivenValidPowerHandleWhenGettingEnergyThresholdThenSuccessIsReturnedAndParameterValuesAreValid) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t count = 0;
    auto pPowerHandles = lzt::get_power_handles(device, count);
    for (auto pPowerHandle : pPowerHandles) {
      EXPECT_NE(nullptr, pPowerHandle);
      auto pThreshold = lzt::get_power_energy_threshold(pPowerHandle);
      ASSERT_GE(pThreshold.threshold, 0);
      if (pThreshold.threshold > 0)
        EXPECT_LT(pThreshold.processId, UINT32_MAX);
      else
        EXPECT_EQ(pThreshold.processId, UINT32_MAX);
    }
  }
}
TEST(
    zetSysmanPowerGetEnergyThresholdTests,
    GivenValidPowerHandleWhenGettingEnergyThresholdTwiceThenSameValueReturned) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t count = 0;
    auto pPowerHandles = lzt::get_power_handles(device, count);
    for (auto pPowerHandle : pPowerHandles) {
      EXPECT_NE(nullptr, pPowerHandle);
      auto pThresholdInitial = lzt::get_power_energy_threshold(pPowerHandle);
      auto pThresholdLater = lzt::get_power_energy_threshold(pPowerHandle);
      EXPECT_EQ(pThresholdInitial.enable, pThresholdLater.enable);
      EXPECT_EQ(pThresholdInitial.threshold, pThresholdLater.threshold);
      EXPECT_EQ(pThresholdInitial.processId, pThresholdLater.processId);
    }
  }
}
} // namespace
