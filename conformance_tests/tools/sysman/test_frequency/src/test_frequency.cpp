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

} // namespace
