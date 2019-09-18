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
#include "xe_copy.h"
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

class xeBarrierEventSignalingTests : public lzt::xeEventPoolTests {};

TEST_F(
    xeBarrierEventSignalingTests,
    GivenEventSignaledWhenCommandListAppendingBarrierThenHostDetectsEventSuccessfully) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  xe_event_handle_t event_barrier_to_host = nullptr;
  std::vector<int> inpa = {0, 1, 2,  3,  4,  5,  6,  7,
                           8, 9, 10, 11, 12, 13, 14, 15};
  size_t xfer_size = inpa.size() * sizeof(int);
  void *dev_mem = lzt::allocate_device_memory(xfer_size);
  void *host_mem = lzt::allocate_host_memory(xfer_size);
  xe_command_list_handle_t cmd_list = lzt::create_command_list(device);
  xe_command_queue_handle_t cmd_q = lzt::create_command_queue(device);
  ep.create_event(event_barrier_to_host, XE_EVENT_SCOPE_FLAG_HOST,
                  XE_EVENT_SCOPE_FLAG_HOST);
  lzt::append_memory_copy(cmd_list, dev_mem, inpa.data(), xfer_size, nullptr, 0,
                          nullptr);
  lzt::append_barrier(cmd_list, event_barrier_to_host, 0, nullptr);
  lzt::append_memory_copy(cmd_list, host_mem, dev_mem, xfer_size, nullptr, 0,
                          nullptr);
  lzt::close_command_list(cmd_list);
  lzt::execute_command_lists(cmd_q, 1, &cmd_list, nullptr);
  lzt::synchronize(cmd_q, UINT32_MAX);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeEventHostSynchronize(event_barrier_to_host, UINT32_MAX - 1));
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventQueryStatus(event_barrier_to_host));

  lzt::destroy_command_queue(cmd_q);
  lzt::destroy_command_list(cmd_list);
  lzt::free_memory(host_mem);
  lzt::free_memory(dev_mem);
  ep.destroy_event(event_barrier_to_host);
}

TEST_F(
    xeBarrierEventSignalingTests,
    GivenCommandListAppendingBarrierWaitsForEventsWhenHostAndCommandListSendSignalsThenCommandListExecutesSuccessfully) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  uint32_t num_events = 6;
  ASSERT_GE(num_events, 3);
  std::vector<xe_event_handle_t> events_to_barrier(num_events, nullptr);
  std::vector<int> inpa = {0, 1, 2,  3,  4,  5,  6,  7,
                           8, 9, 10, 11, 12, 13, 14, 15};
  size_t xfer_size = inpa.size() * sizeof(int);
  void *dev_mem = lzt::allocate_device_memory(xfer_size);
  void *host_mem = lzt::allocate_host_memory(xfer_size);
  xe_command_list_handle_t cmd_list = lzt::create_command_list(device);
  xe_command_queue_handle_t cmd_q = lzt::create_command_queue(device);
  ep.create_events(events_to_barrier, num_events, XE_EVENT_SCOPE_FLAG_HOST,
                   XE_EVENT_SCOPE_FLAG_HOST);
  lzt::append_signal_event(cmd_list, events_to_barrier[0]);
  lzt::append_memory_copy(cmd_list, dev_mem, inpa.data(), xfer_size, nullptr, 0,
                          nullptr);
  lzt::append_signal_event(cmd_list, events_to_barrier[1]);
  lzt::append_barrier(cmd_list, nullptr, num_events, events_to_barrier.data());
  lzt::append_memory_copy(cmd_list, host_mem, dev_mem, xfer_size, nullptr, 0,
                          nullptr);
  lzt::close_command_list(cmd_list);
  lzt::execute_command_lists(cmd_q, 1, &cmd_list, nullptr);
  // Note: Current test for Wait Events will hang on Fulsim and Cobalt (ref
  // LOKI-457)
  for (uint32_t i = 2; i < num_events; i++) {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSignal(events_to_barrier[i]));
  }
  lzt::synchronize(cmd_q, UINT32_MAX);
  for (uint32_t i = 0; i < num_events; i++) {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeEventQueryStatus(events_to_barrier[i]));
  }

  lzt::destroy_command_queue(cmd_q);
  lzt::destroy_command_list(cmd_list);
  lzt::free_memory(host_mem);
  lzt::free_memory(dev_mem);
  ep.destroy_events(events_to_barrier);
}

TEST_F(
    xeBarrierEventSignalingTests,
    GivenEventSignaledWhenCommandListAppendingMemoryRangesBarrierThenHostDetectsEventSuccessfully) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  xe_event_handle_t event_barrier_to_host = nullptr;
  std::vector<int> inpa = {0, 1, 2,  3,  4,  5,  6,  7,
                           8, 9, 10, 11, 12, 13, 14, 15};
  size_t xfer_size = inpa.size() * sizeof(int);
  const std::vector<size_t> range_sizes{inpa.size(), inpa.size()};
  void *dev_mem = lzt::allocate_device_memory(xfer_size);
  void *host_mem = lzt::allocate_host_memory(xfer_size);
  std::vector<const void *> ranges{dev_mem, host_mem};
  xe_command_list_handle_t cmd_list = lzt::create_command_list(device);
  xe_command_queue_handle_t cmd_q = lzt::create_command_queue(device);
  ep.create_event(event_barrier_to_host, XE_EVENT_SCOPE_FLAG_HOST,
                  XE_EVENT_SCOPE_FLAG_HOST);
  lzt::append_memory_copy(cmd_list, dev_mem, inpa.data(), xfer_size, nullptr, 0,
                          nullptr);
  lzt::append_memory_ranges_barrier(cmd_list, ranges.size(), range_sizes.data(),
                                    ranges.data(), event_barrier_to_host, 0,
                                    nullptr);
  lzt::append_memory_copy(cmd_list, host_mem, dev_mem, xfer_size, nullptr, 0,
                          nullptr);
  lzt::close_command_list(cmd_list);
  lzt::execute_command_lists(cmd_q, 1, &cmd_list, nullptr);
  lzt::synchronize(cmd_q, UINT32_MAX);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeEventHostSynchronize(event_barrier_to_host, UINT32_MAX - 1));
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventQueryStatus(event_barrier_to_host));

  lzt::destroy_command_queue(cmd_q);
  lzt::destroy_command_list(cmd_list);
  lzt::free_memory(host_mem);
  lzt::free_memory(dev_mem);
  ep.destroy_event(event_barrier_to_host);
}

TEST_F(
    xeBarrierEventSignalingTests,
    GivenCommandListAppendingMemoryRangesBarrierWaitsForEventsWhenHostAndCommandListSendSignalsThenCommandListExecutesSuccessfully) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  uint32_t num_events = 6;
  ASSERT_GE(num_events, 3);
  std::vector<xe_event_handle_t> events_to_barrier(num_events, nullptr);
  std::vector<int> inpa = {0, 1, 2,  3,  4,  5,  6,  7,
                           8, 9, 10, 11, 12, 13, 14, 15};
  size_t xfer_size = inpa.size() * sizeof(int);
  const std::vector<size_t> range_sizes{inpa.size(), inpa.size()};
  void *dev_mem = lzt::allocate_device_memory(xfer_size);
  void *host_mem = lzt::allocate_host_memory(xfer_size);
  std::vector<const void *> ranges{dev_mem, host_mem};
  xe_command_list_handle_t cmd_list = lzt::create_command_list(device);
  xe_command_queue_handle_t cmd_q = lzt::create_command_queue(device);
  ep.create_events(events_to_barrier, num_events, XE_EVENT_SCOPE_FLAG_HOST,
                   XE_EVENT_SCOPE_FLAG_HOST);

  lzt::append_signal_event(cmd_list, events_to_barrier[0]);
  lzt::append_memory_copy(cmd_list, dev_mem, inpa.data(), xfer_size, nullptr, 0,
                          nullptr);
  lzt::append_signal_event(cmd_list, events_to_barrier[1]);
  lzt::append_memory_ranges_barrier(cmd_list, ranges.size(), range_sizes.data(),
                                    ranges.data(), nullptr, num_events,
                                    events_to_barrier.data());
  lzt::append_memory_copy(cmd_list, host_mem, dev_mem, xfer_size, nullptr, 0,
                          nullptr);
  lzt::close_command_list(cmd_list);
  lzt::execute_command_lists(cmd_q, 1, &cmd_list, nullptr);
  // Note: Current test for Wait Events will hang on Fulsim and Cobalt (ref
  // LOKI-457)
  for (uint32_t i = 2; i < num_events; i++) {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSignal(events_to_barrier[i]));
  }
  lzt::synchronize(cmd_q, UINT32_MAX);
  for (uint32_t i = 0; i < num_events; i++) {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeEventQueryStatus(events_to_barrier[i]));
  }

  lzt::destroy_command_queue(cmd_q);
  lzt::destroy_command_list(cmd_list);
  lzt::free_memory(host_mem);
  lzt::free_memory(dev_mem);
  ep.destroy_events(events_to_barrier);
}

enum BarrierType { BARRIER, MEMORY_RANGES_BARRIER };

class xeBarrierKernelTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<enum BarrierType> {};

TEST_P(
    xeBarrierKernelTests,
    GivenKernelFunctionsAndMemoryCopyWhenBarrierInsertedThenExecutionCompletesSuccesfully) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  xe_command_list_handle_t cmd_list = lzt::create_command_list(device);
  xe_command_queue_handle_t cmd_q = lzt::create_command_queue(device);
  size_t num_int = 1000;
  void *dev_buff = lzt::allocate_device_memory(num_int * sizeof(int));
  void *host_buff = lzt::allocate_host_memory(num_int * sizeof(int));

  int *p_host = static_cast<int *>(host_buff);
  int addval_1 = -100;
  int val_1 = 50000;
  int val = val_1;
  for (size_t i = 0; i < num_int; i++) {
    p_host[i] = val;
    val += addval_1;
  }
  p_host = static_cast<int *>(host_buff);

  enum BarrierType barrier_type = GetParam();
  int addval_2 = 50;
  xe_module_handle_t module =
      lzt::create_module(device, "xe_barrier_add.spv",
                         XE_MODULE_FORMAT_IL_SPIRV, nullptr, nullptr);
  xe_function_handle_t function_1 =
      lzt::create_function(module, "xe_barrier_add_constant");
  lzt::set_group_size(function_1, 1, 1, 1);

  int *p_dev = static_cast<int *>(dev_buff);
  lzt::set_argument_value(function_1, 0, sizeof(p_dev), &p_dev);
  lzt::set_argument_value(function_1, 1, sizeof(addval_2), &addval_2);
  xe_thread_group_dimensions_t tg;
  tg.groupCountX = num_int;
  tg.groupCountY = 1;
  tg.groupCountZ = 1;
  lzt::append_memory_copy(cmd_list, dev_buff, host_buff, num_int * sizeof(int),
                          nullptr, 0, nullptr);
  // Memory barrier to ensure memory coherency after copy to device memory
  if (barrier_type == MEMORY_RANGES_BARRIER) {
    const std::vector<size_t> range_sizes{num_int * sizeof(int),
                                          num_int * sizeof(int)};
    std::vector<const void *> ranges{dev_buff, host_buff};
    lzt::append_memory_ranges_barrier(cmd_list, ranges.size(),
                                      range_sizes.data(), ranges.data(),
                                      nullptr, 0, nullptr);
  } else {
    lzt::append_barrier(cmd_list, nullptr, 0, nullptr);
  }
  lzt::append_launch_function(cmd_list, function_1, &tg, nullptr, 0, nullptr);
  // Execution barrier to ensure function_1 completes before function_2 starts
  lzt::append_barrier(cmd_list, nullptr, 0, nullptr);
  xe_function_handle_t function_2 =
      lzt::create_function(module, "xe_barrier_add_two_arrays");
  lzt::set_group_size(function_2, 1, 1, 1);

  lzt::set_argument_value(function_2, 0, sizeof(p_host), &p_host);
  lzt::set_argument_value(function_2, 1, sizeof(p_dev), &p_dev);
  lzt::append_launch_function(cmd_list, function_2, &tg, nullptr, 0, nullptr);

  lzt::close_command_list(cmd_list);
  lzt::execute_command_lists(cmd_q, 1, &cmd_list, nullptr);
  lzt::synchronize(cmd_q, UINT32_MAX);
  val = (2 * val_1) + addval_2;
  for (size_t i = 0; i < num_int; i++) {
    EXPECT_EQ(p_host[i], val);
    val += (2 * addval_1);
  }

  lzt::destroy_function(function_2);
  lzt::destroy_function(function_1);
  lzt::destroy_module(module);
  lzt::destroy_command_queue(cmd_q);
  lzt::destroy_command_list(cmd_list);
  lzt::free_memory(host_buff);
  lzt::free_memory(dev_buff);
}

INSTANTIATE_TEST_CASE_P(TestKernelWithBarrierAndMemoryRangesBarrier,
                        xeBarrierKernelTests,
                        testing::Values(BARRIER, MEMORY_RANGES_BARRIER));

} // namespace

// TODO: Test barriers in non-empty command lists
// TODO: Test barriers on a device using OpenCL C kernels
