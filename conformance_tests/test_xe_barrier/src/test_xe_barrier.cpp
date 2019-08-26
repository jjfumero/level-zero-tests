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

#include "xe_utils/xe_utils.hpp"
#include "xe_test_harness/xe_test_harness.hpp"
#include "logging/logging.hpp"
#include "xe_barrier.h"

namespace lzt = level_zero_tests;

namespace {

class xeCommandListAppendBarrierTests : public lzt::xeCommandListTests {};

TEST_F(xeCommandListAppendBarrierTests,
       GivenEmptyCommandListWhenAppendingBarrierThenSuccessIsReturned) {
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendBarrier(cl.command_list_, nullptr, 0, nullptr));
}

class xeEventPoolCommandListAppendBarrierTests
    : public xeEventPoolCommandListTests {};

TEST_F(
    xeEventPoolCommandListAppendBarrierTests,
    GivenEmptyCommandListWhenAppendingBarrierWithEventThenSuccessIsReturned) {
  xe_event_handle_t event = nullptr;

  ep.create_event(event);

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendBarrier(cl.command_list_, event, 0, nullptr));
  ep.destroy_event(event);
}

TEST_F(
    xeEventPoolCommandListAppendBarrierTests,
    GivenEmptyCommandListWhenAppendingBarrierWithEventsThenSuccessIsReturned) {
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> events(event_count, nullptr);

  ep.create_events(events, event_count);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendBarrier(cl.command_list_, nullptr, events.size(),
                                       events.data()));
  ep.destroy_events(events);
}

TEST_F(
    xeEventPoolCommandListAppendBarrierTests,
    GivenEmptyCommandListWhenAppendingBarrierWithSignalEventAndWaitEventsThenSuccessIsReturned) {
  xe_event_handle_t event = nullptr;
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> events(event_count, nullptr);

  ep.create_event(event);
  ep.create_events(events, event_count);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendBarrier(cl.command_list_, event, events.size(),
                                       events.data()));
  ep.destroy_events(events);
  ep.destroy_event(event);
}

void AppendMemoryRangesBarrierTest(
    xe_command_list_handle_t command_list, xe_event_handle_t signaling_event,
    std::vector<xe_event_handle_t> &waiting_events) {
  const std::vector<size_t> range_sizes{4096, 8192};
  std::vector<const void *> ranges{
      lzt::allocate_device_memory(range_sizes[0] * 2),
      lzt::allocate_device_memory(range_sizes[1] * 2)};

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemoryRangesBarrier(
                command_list, ranges.size(), range_sizes.data(), ranges.data(),
                signaling_event, waiting_events.size(), waiting_events.data()));

  for (auto &range : ranges)
    lzt::free_memory(range);
}

// The following test fails and the failure is recorded in:
// LOKI-303 xeCommandListAppendMemoryRangesBarrier() returns:
// XE_RESULT_ERROR_UNSUPPORTED
class xeCommandListAppendMemoryRangesBarrierTests
    : public lzt::xeCommandListTests {};

TEST_F(
    xeCommandListAppendMemoryRangesBarrierTests,
    GivenEmptyCommandListWhenAppendingMemoryRangesBarrierThenSuccessIsReturned) {
  std::vector<xe_event_handle_t> waiting_events;

  AppendMemoryRangesBarrierTest(cl.command_list_, nullptr, waiting_events);
}

class xeEventPoolCommandListAppendMemoryRangesBarrierTests
    : public xeEventPoolCommandListTests {};

TEST_F(
    xeEventPoolCommandListAppendMemoryRangesBarrierTests,
    GivenEmptyCommandListEventPoolWhenAppendingMemoryRangesBarrierSignalEventThenSuccessIsReturned) {
  xe_event_handle_t signaling_event = nullptr;
  std::vector<xe_event_handle_t> waiting_events;

  ep.create_event(signaling_event);
  AppendMemoryRangesBarrierTest(cl.command_list_, signaling_event,
                                waiting_events);
  ep.destroy_event(signaling_event);
}

TEST_F(
    xeEventPoolCommandListAppendMemoryRangesBarrierTests,
    GivenEmptyCommandListEventPoolWhenAppendingMemoryRangesBarrierWaitEventsThenSuccessIsReturned) {
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> waiting_events(event_count, nullptr);

  ep.create_events(waiting_events, event_count);
  AppendMemoryRangesBarrierTest(cl.command_list_, nullptr, waiting_events);
  ep.destroy_events(waiting_events);
}

TEST_F(
    xeEventPoolCommandListAppendMemoryRangesBarrierTests,
    GivenEmptyCommandListEventPoolWhenAppendingMemoryRangesBarrierSignalEventAndWaitEventsThenSuccessIsReturned) {
  xe_event_handle_t signaling_event = nullptr;
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> waiting_events(event_count, nullptr);

  ep.create_event(signaling_event);
  ep.create_events(waiting_events, event_count);
  AppendMemoryRangesBarrierTest(cl.command_list_, signaling_event,
                                waiting_events);
  ep.destroy_event(signaling_event);
  ep.destroy_events(waiting_events);
}

// The following test fails and the failure is recorded in:
// LOKI-303 xeCommandListAppendMemoryRangesBarrier() returns:
// XE_RESULT_ERROR_UNSUPPORTED
class xeDeviceSystemBarrierTests : public ::testing::Test {};

TEST_F(xeDeviceSystemBarrierTests,
       GivenDeviceWhenAddingSystemBarrierThenSuccessIsReturned) {

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceSystemBarrier(lzt::xeDevice::get_instance()->get_device()));
}

} // namespace

// TODO: Test barriers in non-empty command lists
// TODO: Test barriers on a device using OpenCL C kernels
