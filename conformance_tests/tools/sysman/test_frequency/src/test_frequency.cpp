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

namespace lzt = level_zero_tests;

#include "ze_api.h"
#include "zet_api.h"

namespace {

TEST(
    zetSysmanFrequencyGetTests,
    GivenComponentCountZeroWhenRetrievingSysmanHandlesThenNonZeroCountIsReturned) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    lzt::get_freq_handle_count(device);
  }
}

TEST(
    zetSysmanFrequencyGetTests,
    GivenComponentCountZeroWhenRetrievingSysmanHandlesThenNotNullFrequencyHandlesAreReturned) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandles = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandles) {
      EXPECT_NE(nullptr, pFreqHandle);
    }
  }
}

TEST(
    zetSysmanFrequencyGetTests,
    GivenComponentCountWhenRetrievingSysmanHandlesThenActualComponentCountIsUpdated) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    uint32_t pCount = lzt::get_freq_handle_count(device);
    uint32_t testCount = pCount + 1;
    testCount = lzt::get_freq_handle_count(device, testCount);
    EXPECT_EQ(testCount, pCount);
  }
}

TEST(
    zetSysmanFrequencyGetTests,
    GivenValidComponentCountWhenCallingApiTwiceThenSimilarFrequencyHandlesReturned) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandlesInitial = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandlesInitial) {
      EXPECT_NE(nullptr, pFreqHandle);
    }

    auto pFreqHandlesLater = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandlesLater) {
      EXPECT_NE(nullptr, pFreqHandle);
    }

    EXPECT_EQ(pFreqHandlesInitial.size(), pFreqHandlesLater.size());
    EXPECT_TRUE(std::equal(pFreqHandlesInitial.begin(),
                           pFreqHandlesInitial.end(),
                           pFreqHandlesLater.begin()));
  }
}

TEST(zetSysmanFrequencyGetStateTests,
     GivenValidDeviceWhenRetrievingFreqStateThenValidFreqStatesAreReturned) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandles = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandles) {
      EXPECT_NE(nullptr, pFreqHandle);
      zet_freq_state_t pState = lzt::get_freq_state(pFreqHandle);
      lzt::validate_freq_state(pFreqHandle, pState);
    }
  }
}

TEST(zetSysmanFrequencyGetStateTests,
     GivenValidFreqRangeWhenRetrievingFreqStateThenValidFreqStatesAreReturned) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandles = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandles) {
      EXPECT_NE(nullptr, pFreqHandle);
      zet_freq_range_t limits;
      uint32_t count = 0;
      auto frequency = lzt::get_available_clocks(pFreqHandle, count);
      ASSERT_GT(frequency.size(), 0);
      limits.min = frequency[0];
      limits.max = frequency[1];
      lzt::set_freq_range(pFreqHandle, limits);
      lzt::idle_check(pFreqHandle);
      zet_freq_state_t state = lzt::get_freq_state(pFreqHandle);
      lzt::validate_freq_state(pFreqHandle, state);
      EXPECT_LE(state.actual, limits.max);
      EXPECT_GE(state.actual, limits.min);
      EXPECT_LE(state.request, limits.max);
      EXPECT_GE(state.request, limits.min);
    }
  }
}
TEST(
    zetSysmanFrequencyGetAvailableClocksTests,
    GivenValidFrequencyHandleWhenRetrievingAvailableClocksThenSuccessAndSameValuesAreReturnedTwice) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandles = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandles) {
      EXPECT_NE(nullptr, pFreqHandle);
      uint32_t icount = 0;
      uint32_t lcount = 0;
      auto pFrequencyInitial = lzt::get_available_clocks(pFreqHandle, icount);
      auto pFrequencyLater = lzt::get_available_clocks(pFreqHandle, lcount);

      EXPECT_EQ(pFrequencyInitial.size(), pFrequencyLater.size());
      EXPECT_TRUE(std::equal(pFrequencyInitial.begin(), pFrequencyInitial.end(),
                             pFrequencyLater.begin()));
    }
  }
}

TEST(
    zetSysmanFrequencyGetAvailableClocksTests,
    GivenValidFrequencyHandleWhenRetrievingAvailableClocksThenPositiveAndValidValuesAreReturned) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandles = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandles) {
      EXPECT_NE(nullptr, pFreqHandle);
      uint32_t count = 0;
      auto pFrequency = lzt::get_available_clocks(pFreqHandle, count);

      for (uint32_t i = 0; i < pFrequency.size(); i++) {
        EXPECT_GT(pFrequency[i], 0);
        if (i > 0)
          EXPECT_GE(
              pFrequency[i],
              pFrequency[i - 1]); // Each entry in array of pFrequency, should
                                  // be less than or equal to next entry
      }
    }
  }
}
TEST(zetSysmanFrequencyGetAvailableClocksTests,
     GivenClocksCountWhenRetrievingAvailableClocksThenActualCountIsUpdated) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandles = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandles) {
      EXPECT_NE(nullptr, pFreqHandle);
      uint32_t pCount = 0;
      pCount = lzt::get_available_clock_count(pFreqHandle);
      uint32_t tCount = pCount + 1;
      lzt::get_available_clocks(pFreqHandle, tCount);
      EXPECT_EQ(pCount, tCount);
    }
  }
}

TEST(
    zetSysmanFrequencyGetPropertiesTests,
    GivenValidFrequencyHandleWhenRequestingDeviceGPUTypeThenExpectCanControlPropertyToBeTrue) {
  ze_result_t result;
  zet_freq_properties_t properties;
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandles = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandles) {
      EXPECT_NE(nullptr, pFreqHandle);
      properties = lzt::get_freq_properties(pFreqHandle);
      if (properties.type == ZET_FREQ_DOMAIN_GPU)
        EXPECT_TRUE(properties.canControl);
      else if (properties.type == ZET_FREQ_DOMAIN_MEMORY)
        EXPECT_FALSE(properties.canControl);
      else
        FAIL();
    }
  }
}

TEST(
    zetSysmanFrequencyGetPropertiesTests,
    GivenValidFrequencyHandleWhenRequestingFrequencyPropertiesThenExpectPositiveFrequencyRangeAndSteps) {
  ze_result_t result;
  zet_freq_properties_t properties;
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandles = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandles) {
      EXPECT_NE(nullptr, pFreqHandle);
      properties = lzt::get_freq_properties(pFreqHandle);
      EXPECT_GT(properties.max, 0);
      EXPECT_GT(properties.min, 0);
      EXPECT_GT(properties.step, 0);
    }
  }
}

TEST(
    zetSysmanFrequencyGetPropertiesTests,
    GivenSameFrequencyHandleWhenRequestingFrequencyPropertiesThenExpectSamePropertiesOnMultipleCalls) {
  ze_result_t result;
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandles = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandles) {
      EXPECT_NE(nullptr, pFreqHandle);
    }
    std::vector<zet_freq_properties_t> properties(3);

    for (uint32_t i = 0; i < 3; i++)
      properties[i] = lzt::get_freq_properties(pFreqHandles[0]);

    ASSERT_GT(properties.size(), 1);
    for (uint32_t i = 1; i < properties.size(); i++) {
      EXPECT_EQ(properties[0].type, properties[i].type);
      EXPECT_EQ(properties[0].onSubdevice, properties[i].onSubdevice);
      if (properties[0].onSubdevice == true &&
          properties[i].onSubdevice == true)
        EXPECT_EQ(properties[0].subdeviceId, properties[i].subdeviceId);
      EXPECT_EQ(properties[0].canControl, properties[i].canControl);
      EXPECT_EQ(properties[0].isThrottleEventSupported,
                properties[i].isThrottleEventSupported);
      EXPECT_EQ(properties[0].max, properties[i].max);
      EXPECT_EQ(properties[0].min, properties[i].min);
      EXPECT_EQ(properties[0].step, properties[i].step);
    }
  }
}

TEST(
    zetSysmanFrequencyGetRangeTests,
    GivenValidFrequencyCountWhenRequestingFrequencyHandleThenExpectzetSysmanFrequencyGetRangeToReturnSuccessOnMultipleCalls) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandles = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandles) {
      EXPECT_NE(nullptr, pFreqHandle);
      zet_freq_range_t freqRange;
      for (uint32_t i = 0; i < 3; i++)
        freqRange = lzt::get_freq_range(pFreqHandle);
    }
  }
}

TEST(
    zetSysmanFrequencyGetRangeTests,
    GivenSameFrequencyHandleWhenRequestingFrequencyRangeThenExpectSameRangeOnMultipleCalls) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandles = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandles) {
      EXPECT_NE(nullptr, pFreqHandle);
      std::vector<zet_freq_range_t> freqRangeToCompare;
      for (uint32_t i = 0; i < 3; i++)
        freqRangeToCompare.push_back(lzt::get_freq_range(pFreqHandle));

      for (uint32_t i = 1; i < freqRangeToCompare.size(); i++) {
        EXPECT_EQ(freqRangeToCompare[0].max, freqRangeToCompare[i].max);
        EXPECT_EQ(freqRangeToCompare[0].min, freqRangeToCompare[i].min);
      }
    }
  }
}

TEST(
    zetSysmanFrequencyGetRangeTests,
    GivenValidFrequencyCountWhenRequestingFrequencyHandleThenExpectzetSysmanFrequencyGetRangeToReturnValidFrequencyRanges) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandles = lzt::get_freq_handles(device);
    zet_freq_range_t freqRange;
    for (auto pFreqHandle : pFreqHandles)
      freqRange = lzt::get_and_validate_freq_range(pFreqHandle);
  }
}

TEST(
    zetSysmanFrequencySetRangeTests,
    GivenValidFrequencyRangeWhenRequestingSetFrequencyThenExpectUpdatedFrequencyInGetFrequencyCall) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandles = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandles) {
      EXPECT_NE(nullptr, pFreqHandle);

      zet_freq_range_t freqRange, freqRangeReset;
      uint32_t count = 0;
      auto frequency = lzt::get_available_clocks(*pFreqHandles.data(), count);
      ASSERT_GT(frequency.size(), 0);
      if (count == 1) {
        freqRange.min = frequency[0];
        freqRange.max = frequency[0];
        lzt::set_freq_range(pFreqHandle, freqRange);
        freqRangeReset = lzt::get_and_validate_freq_range(pFreqHandle);
        EXPECT_EQ(freqRange.max, freqRangeReset.max);
        EXPECT_EQ(freqRange.min, freqRangeReset.min);
      } else {
        for (uint32_t i = 1; i < count; i++) {
          freqRange.min = frequency[i - 1];
          freqRange.max = frequency[i];
          lzt::set_freq_range(pFreqHandle, freqRange);
          freqRangeReset = lzt::get_and_validate_freq_range(pFreqHandle);
          EXPECT_EQ(freqRange.max, freqRangeReset.max);
          EXPECT_EQ(freqRange.min, freqRangeReset.min);
        }
      }
    }
  }
}
void load_for_gpu() {
  int m, k, n;
  m = k = n = 5000;
  std::vector<float> a(m * k, 1);
  std::vector<float> b(k * n, 1);
  std::vector<float> c(m * n, 0);
  const ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  void *a_buffer = lzt::allocate_host_memory(m * k * sizeof(float));
  void *b_buffer = lzt::allocate_host_memory(k * n * sizeof(float));
  void *c_buffer = lzt::allocate_host_memory(m * n * sizeof(float));
  ze_module_handle_t module =
      lzt::create_module(device, "ze_matrix_multiplication.spv",
                         ZE_MODULE_FORMAT_IL_SPIRV, nullptr, nullptr);
  ze_kernel_handle_t function =
      lzt::create_function(module, "ze_matrix_multiplication");
  lzt::set_group_size(function, 16, 16, 1);
  lzt::set_argument_value(function, 0, sizeof(a_buffer), &a_buffer);
  lzt::set_argument_value(function, 1, sizeof(b_buffer), &b_buffer);
  lzt::set_argument_value(function, 2, sizeof(m), &m);
  lzt::set_argument_value(function, 3, sizeof(k), &k);
  lzt::set_argument_value(function, 4, sizeof(n), &n);
  lzt::set_argument_value(function, 5, sizeof(c_buffer), &c_buffer);
  ze_command_list_handle_t cmd_list = lzt::create_command_list(device);
  std::memcpy(a_buffer, a.data(), a.size() * sizeof(float));
  std::memcpy(b_buffer, b.data(), b.size() * sizeof(float));
  lzt::append_barrier(cmd_list, nullptr, 0, nullptr);
  const int group_count_x = m / 16;
  const int group_count_y = n / 16;
  ze_thread_group_dimensions_t tg;
  tg.groupCountX = group_count_x;
  tg.groupCountY = group_count_y;
  tg.groupCountZ = 1;
  zeCommandListAppendLaunchKernel(cmd_list, function, &tg, nullptr, 0, nullptr);
  lzt::append_barrier(cmd_list, nullptr, 0, nullptr);
  lzt::close_command_list(cmd_list);
  ze_command_queue_handle_t cmd_q = lzt::create_command_queue(device);
  lzt::execute_command_lists(cmd_q, 1, &cmd_list, nullptr);
  lzt::synchronize(cmd_q, UINT32_MAX);
  std::memcpy(c.data(), c_buffer, c.size() * sizeof(float));
  lzt::destroy_command_queue(cmd_q);
  lzt::destroy_command_list(cmd_list);
  lzt::destroy_function(function);
  lzt::free_memory(a_buffer);
  lzt::free_memory(b_buffer);
  lzt::free_memory(c_buffer);
  lzt::destroy_module(module);
}
void get_throttle_time_init(zet_sysman_freq_handle_t pFreqHandle,
                            zet_freq_throttle_time_t &throttletime) {
  EXPECT_EQ(lzt::check_for_throttling(pFreqHandle), true);
  throttletime = lzt::get_throttle_time(pFreqHandle);
  EXPECT_GT(throttletime.throttleTime, 0);
  EXPECT_GT(throttletime.timestamp, 0);
}

TEST(zetSysmanFrequencyGetThrottleTimeTests,
     GivenValidFrequencyHandleThenCheckForThrottling) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto pFreqHandles = lzt::get_freq_handles(device);
    for (auto pFreqHandle : pFreqHandles) {
      EXPECT_NE(nullptr, pFreqHandle);
      {
        uint32_t pcount = 0;
        auto pPowerHandles = lzt::get_power_handles(device, pcount);
        for (auto pPowerHandle : pPowerHandles) {
          EXPECT_NE(nullptr, pPowerHandle);
          zet_power_sustained_limit_t power_sustained_limit_Initial;
          lzt::get_power_limits(pPowerHandle, &power_sustained_limit_Initial,
                                nullptr, nullptr);
          // TODO: will fix this once API gets implemented
          zet_power_sustained_limit_t power_sustained_limit_set;
          power_sustained_limit_set.power = 0;
          lzt::set_power_limits(pPowerHandle, &power_sustained_limit_set,
                                nullptr, nullptr);
          auto before_throttletime = lzt::get_throttle_time(pFreqHandle);
          zet_freq_throttle_time_t throttletime;
          std::thread first(load_for_gpu);
          std::thread second(get_throttle_time_init, pFreqHandle,
                             std::ref(throttletime));
          first.join();
          second.join();
          auto after_throttletime = lzt::get_throttle_time(pFreqHandle);
          EXPECT_LT(before_throttletime.throttleTime,
                    after_throttletime.throttleTime);
          EXPECT_NE(before_throttletime.timestamp,
                    after_throttletime.timestamp);
          lzt::set_power_limits(pPowerHandle, &power_sustained_limit_Initial,
                                nullptr, nullptr);
        }
      }
    }
  }
}

} // namespace
