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

#include "xe_driver.h"
#include "xe_event.h"
#include "xe_copy.h"
#include "xe_barrier.h"
#include <thread>

namespace {

class xeDeviceCreateEventPoolTests : public lzt::xeEventPoolTests {};

TEST_F(
    xeDeviceCreateEventPoolTests,
    GivenDefaultDeviceWhenCreatingEventPoolWithDefaultFlagsThenNotNullEventPoolIsReturned) {
  ep.InitEventPool();
}

TEST_F(
    xeDeviceCreateEventPoolTests,
    GivenDefaultDeviceWhenCreatingEventPoolWithHostVisibleFlagsThenNotNullEventPoolIsReturned) {
  ep.InitEventPool(32, XE_EVENT_POOL_FLAG_HOST_VISIBLE);
}

TEST_F(
    xeDeviceCreateEventPoolTests,
    GivenDefaultDeviceWhenCreatingEventPoolWithIPCFlagsThenNotNullEventPoolIsReturned) {
  ep.InitEventPool(32, XE_EVENT_POOL_FLAG_IPC);
}

class xeDeviceCreateEventPermuteEventsTests
    : public lzt::xeEventPoolTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_event_scope_flag_t, xe_event_scope_flag_t>> {};

TEST_P(
    xeDeviceCreateEventPermuteEventsTests,
    GivenDefaultDeviceAndEventPoolWhenCreatingEventsWithSignalAndWaitEventsThenNotNullEventIsReturned) {

  xe_event_handle_t event = nullptr;

  ep.create_event(event, std::get<0>(GetParam()), std::get<1>(GetParam()));
  ep.destroy_event(event);
}

INSTANTIATE_TEST_CASE_P(
    ImplictEventCreateParameterizedTest, xeDeviceCreateEventPermuteEventsTests,
    ::testing::Combine(::testing::Values(XE_EVENT_SCOPE_FLAG_NONE,
                                         XE_EVENT_SCOPE_FLAG_SUBDEVICE,
                                         XE_EVENT_SCOPE_FLAG_DEVICE,
                                         XE_EVENT_SCOPE_FLAG_HOST),
                       ::testing::Values(XE_EVENT_SCOPE_FLAG_NONE,
                                         XE_EVENT_SCOPE_FLAG_SUBDEVICE,
                                         XE_EVENT_SCOPE_FLAG_DEVICE,
                                         XE_EVENT_SCOPE_FLAG_HOST)));

TEST_F(xeDeviceCreateEventPoolTests,
       GivenDefaultDeviceWhenGettingIpcHandleThenNotNullisReturned) {
  xe_ipc_event_pool_handle_t hIpc;
  ep.InitEventPool();
  // As of July 10, 2019, xeEventPoolGetIpcHandle() returns UNSUPPORTED
  // and thus the following test fails:
  ASSERT_EQ(XE_RESULT_SUCCESS, xeEventPoolGetIpcHandle(ep.event_pool_, &hIpc));
}

TEST_F(
    xeDeviceCreateEventPoolTests,
    GivenDefaultDeviceWhenGettingIpcHandleAndOpeningAndClosingThenSuccessIsReturned) {
  xe_ipc_event_pool_handle_t hIpc;
  ep.InitEventPool();
  // As of July 10, 2019, xeEventPoolGetIpcHandle() returns UNSUPPORTED
  ASSERT_EQ(XE_RESULT_SUCCESS, xeEventPoolGetIpcHandle(ep.event_pool_, &hIpc));

  // Due to the above failures, the following tests obviously also fail.
  xe_event_pool_handle_t eventPool;
  ep.open_ipc_handle(hIpc, &eventPool);
  EXPECT_NE(nullptr, eventPool);
  ep.close_ipc_handle_pool(eventPool);
}

class xeDeviceCreateEventAndCommandListTests : public lzt::xeCommandList,
                                               public lzt::xeEventPoolTests {};

TEST_F(
    xeDeviceCreateEventAndCommandListTests,
    GivenDefaultDeviceWhenAppendingSignalEventToComandListThenSuccessIsReturned) {
  xe_event_handle_t event = nullptr;

  ep.create_event(event);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendSignalEvent(command_list_, event));
  ep.destroy_event(event);
}

TEST_F(
    xeDeviceCreateEventAndCommandListTests,
    GivenDefaultDeviceWhenAppendingWaitEventsToComandListThenSuccessIsReturned) {
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> events(event_count, nullptr);

  ep.create_events(events, event_count);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendWaitOnEvents(
                                   command_list_, event_count, events.data()));
  ep.destroy_events(events);
}

class xeDeviceCreateEventTests : public lzt::xeEventPoolTests {};

TEST_F(
    xeDeviceCreateEventTests,
    GivenDefaultDeviceAndEventPoolWhenSignalingHostEventThenSuccessIsReturned) {
  xe_event_handle_t event = nullptr;

  ep.create_event(event);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSignal(event));
  ep.destroy_event(event);
}

class xeHostEventSyncPermuteTimeoutTests
    : public lzt::xeEventPoolTests,
      public ::testing::WithParamInterface<uint32_t> {};

void child_thread_function(xe_event_handle_t event, uint32_t timeout) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSynchronize(event, timeout));
}

TEST_P(xeHostEventSyncPermuteTimeoutTests,
       GivenDefaultDeviceAndEventPoolWhenSyncingEventThenSuccessIsReturned) {
  xe_event_handle_t event = nullptr;

  ep.create_event(event, XE_EVENT_SCOPE_FLAG_HOST, XE_EVENT_SCOPE_FLAG_HOST);

  std::thread child_thread(child_thread_function, event, GetParam());
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSignal(event));
  child_thread.join();
  ep.destroy_event(event);
}

INSTANTIATE_TEST_CASE_P(ImplictHostSynchronizeEventParameterizedTest,
                        xeHostEventSyncPermuteTimeoutTests,
                        ::testing::Values(0, 10000000, UINT32_MAX));

TEST_F(
    xeDeviceCreateEventTests,
    GivenDefaultDeviceAndEventPoolWhenCreatingAnEventAndQueryingItsStatusThenNotReadyIsReturned) {
  xe_event_handle_t event = nullptr;

  ep.create_event(event);
  EXPECT_EQ(XE_RESULT_NOT_READY, xeEventQueryStatus(event));
  ep.destroy_event(event);
}

TEST_F(
    xeDeviceCreateEventAndCommandListTests,
    GivenDefaultDeviceAndEventPoolWhenAppendingEventResetThenSuccessIsReturned) {
  xe_event_handle_t event = nullptr;

  ep.create_event(event);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendEventReset(command_list_, event));
  ep.destroy_event(event);
}

TEST_F(xeDeviceCreateEventTests,
       GivenDefaultDeviceAndEventPoolWhenResettingEventThenSuccessIsReturned) {
  xe_event_handle_t event = nullptr;

  ep.create_event(event);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventReset(event));
  ep.destroy_event(event);
}

class xeEventSignalingTests : public lzt::xeEventPoolTests {};

TEST_F(
    xeEventSignalingTests,
    GivenOneEventSignaledbyHostWhenQueryStatusThenVerifyOnlyOneEventDetected) {
  size_t num_event = 10;
  std::vector<xe_event_handle_t> host_event(num_event, nullptr);

  ep.InitEventPool(num_event);
  ep.create_events(host_event, num_event);
  for (uint32_t i = 0; i < num_event; i++) {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSignal(host_event[i]));
    for (uint32_t j = 0; j < num_event; j++) {
      if (j == i) {
        EXPECT_EQ(XE_RESULT_SUCCESS, xeEventQueryStatus(host_event[j]));
      } else {
        EXPECT_EQ(XE_RESULT_NOT_READY, xeEventQueryStatus(host_event[j]));
      }
    }
    EXPECT_EQ(XE_RESULT_SUCCESS, xeEventReset(host_event[i]));
  }
  ep.destroy_events(host_event);
}

TEST_F(
    xeEventSignalingTests,
    GivenOneEventSignaledbyCommandListWhenQueryStatusOnHostThenVerifyOnlyOneEventDetected) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  size_t num_event = 10;
  ASSERT_GE(num_event, 3);
  std::vector<xe_event_handle_t> device_event(num_event, nullptr);
  xe_command_list_handle_t cmd_list = lzt::create_command_list(device);
  xe_command_queue_handle_t cmd_q = lzt::create_command_queue(device);
  size_t copy_size = 4096;
  void *src_buff = lzt::allocate_host_memory(copy_size);
  void *dst_buff = lzt::allocate_shared_memory(copy_size);
  ep.InitEventPool(num_event);
  ep.create_events(device_event, num_event);

  lzt::write_data_pattern(src_buff, copy_size, 1);
  lzt::write_data_pattern(dst_buff, copy_size, 0);
  // Add in Wait on Event to check issue identified in LOKI-537
  for (uint32_t i = 0; i < num_event - 1; i++) {
    lzt::append_signal_event(cmd_list, device_event[i]);
    lzt::append_barrier(cmd_list, nullptr, 0, nullptr);
    lzt::append_wait_on_events(cmd_list, 1, &device_event[num_event - 1]);
    lzt::append_memory_copy(cmd_list, dst_buff, src_buff, copy_size, nullptr, 0,
                            nullptr);
    lzt::close_command_list(cmd_list);
    lzt::execute_command_lists(cmd_q, 1, &cmd_list, nullptr);
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeEventHostSynchronize(device_event[i], UINT32_MAX - 1));
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeEventHostSignal(device_event[num_event - 1]));
    lzt::synchronize(cmd_q, UINT32_MAX);

    for (uint32_t j = 0; j < num_event - 1; j++) {
      if (j == i) {
        EXPECT_EQ(XE_RESULT_SUCCESS, xeEventQueryStatus(device_event[j]));
      } else {
        EXPECT_EQ(XE_RESULT_NOT_READY, xeEventQueryStatus(device_event[j]));
      }
    }
    lzt::validate_data_pattern(dst_buff, copy_size, 1);
    EXPECT_EQ(XE_RESULT_SUCCESS, xeEventReset(device_event[i]));
    EXPECT_EQ(XE_RESULT_SUCCESS, xeEventReset(device_event[num_event - 1]));
    lzt::reset_command_list(cmd_list);
  }

  lzt::destroy_command_queue(cmd_q);
  lzt::destroy_command_list(cmd_list);
  lzt::free_memory(src_buff);
  lzt::free_memory(dst_buff);
  ep.destroy_events(device_event);
}

TEST_F(
    xeEventSignalingTests,
    GivenCommandListWaitsForEventsWhenHostAndCommandListSendsSignalsThenCommandListExecutesSuccessfully) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  size_t num_event = 10;
  ASSERT_GE(num_event, 3);
  std::vector<xe_event_handle_t> device_event(num_event, nullptr);
  xe_command_list_handle_t cmd_list = lzt::create_command_list(device);
  xe_command_queue_handle_t cmd_q = lzt::create_command_queue(device);
  size_t copy_size = 4096;
  void *src_buff = lzt::allocate_host_memory(copy_size);
  void *dev_buff = lzt::allocate_device_memory(copy_size);
  void *dst_buff = lzt::allocate_shared_memory(copy_size);
  ep.InitEventPool(num_event);
  ep.create_events(device_event, num_event);

  lzt::write_data_pattern(src_buff, copy_size, 1);
  lzt::write_data_pattern(dst_buff, copy_size, 0);

  lzt::append_signal_event(cmd_list, device_event[0]);
  lzt::append_memory_copy(cmd_list, dev_buff, src_buff, copy_size, nullptr, 0,
                          nullptr);
  lzt::append_signal_event(cmd_list, device_event[1]);
  lzt::append_barrier(cmd_list, nullptr, 0, nullptr);
  lzt::append_wait_on_events(cmd_list, num_event, device_event.data());
  lzt::append_memory_copy(cmd_list, dst_buff, dev_buff, copy_size, nullptr, 0,
                          nullptr);
  lzt::close_command_list(cmd_list);
  lzt::execute_command_lists(cmd_q, 1, &cmd_list, nullptr);
  for (uint32_t i = 2; i < num_event; i++) {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSignal(device_event[i]));
  }
  lzt::synchronize(cmd_q, UINT32_MAX);

  for (uint32_t i = 0; i < num_event; i++) {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeEventQueryStatus(device_event[i]));
  }

  lzt::validate_data_pattern(dst_buff, copy_size, 1);

  lzt::destroy_command_queue(cmd_q);
  lzt::destroy_command_list(cmd_list);
  lzt::free_memory(src_buff);
  lzt::free_memory(dev_buff);
  lzt::free_memory(dst_buff);
  ep.destroy_events(device_event);
}

TEST_F(xeEventSignalingTests,
       GivenEventsSignaledWhenResetThenQueryStatusReturnsNotReady) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  size_t num_event = 10;
  ASSERT_GE(num_event, 4);
  size_t num_loop = 4;
  ASSERT_GE(num_loop, 4);

  std::vector<xe_event_handle_t> device_event(num_event, nullptr);
  xe_command_list_handle_t cmd_list = lzt::create_command_list(device);
  xe_command_queue_handle_t cmd_q = lzt::create_command_queue(device);
  size_t loop_data_size = 300; // do not make this N*256
  ASSERT_TRUE(loop_data_size % 256);
  size_t copy_size = num_loop * loop_data_size;
  void *src_buff = lzt::allocate_host_memory(copy_size);
  void *dst_buff = lzt::allocate_shared_memory(copy_size);
  uint8_t *src_char = static_cast<uint8_t *>(src_buff);
  uint8_t *dst_char = static_cast<uint8_t *>(dst_buff);
  ep.InitEventPool(num_event);
  ep.create_events(device_event, num_event);

  lzt::write_data_pattern(src_buff, copy_size, 1);
  lzt::write_data_pattern(dst_buff, copy_size, 0);
  for (size_t i = 0; i < num_loop; i++) {
    for (size_t j = 0; j < num_event; j++) {
      EXPECT_EQ(XE_RESULT_NOT_READY, xeEventQueryStatus(device_event[i]));
    }
    lzt::append_signal_event(cmd_list, device_event[i]);
    lzt::append_wait_on_events(cmd_list, num_event, device_event.data());
    lzt::append_memory_copy(cmd_list, static_cast<void *>(dst_char),
                            static_cast<void *>(src_char), loop_data_size,
                            nullptr, 0, nullptr);
    lzt::append_reset_event(cmd_list, device_event[i]);
    lzt::close_command_list(cmd_list);
    lzt::execute_command_lists(cmd_q, 1, &cmd_list, nullptr);
    for (size_t j = 0; j < num_event; j++) {
      if (i != j) {
        EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSignal(device_event[j]));
      }
    }
    lzt::synchronize(cmd_q, UINT32_MAX);

    for (size_t j = 0; j < num_event; j++) {
      if (i == j) {
        EXPECT_EQ(XE_RESULT_NOT_READY, xeEventQueryStatus(device_event[j]));
      } else {
        EXPECT_EQ(XE_RESULT_SUCCESS, xeEventQueryStatus(device_event[j]));
        EXPECT_EQ(XE_RESULT_SUCCESS, xeEventReset(device_event[j]));
        EXPECT_EQ(XE_RESULT_NOT_READY, xeEventQueryStatus(device_event[j]));
      }
    }
    src_char += loop_data_size;
    dst_char += loop_data_size;
    lzt::reset_command_list(cmd_list);
  }
  lzt::validate_data_pattern(dst_buff, copy_size, 1);

  lzt::destroy_command_queue(cmd_q);
  lzt::destroy_command_list(cmd_list);
  lzt::free_memory(src_buff);
  lzt::free_memory(dst_buff);
  ep.destroy_events(device_event);
}

} // namespace

// TODO: Test different event pool flags
// TODO: Test different event pool counts
// TODO: Test event creation from event pool
// TODO: Test event signals
// TODO: Test event synchronization
// TODO: Test event queries
// TODO: Test event in a command list
// TODO: Test event reset
// TODO: Test events on a device using OpenCL C kernels
// TODO: Test IPC
