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
    GivenDefaultDeviceAndEventPoolWhenCreatingAnEventAndQueryingItsStatusThenSuccessIsReturned) {
  xe_event_handle_t event = nullptr;

  ep.create_event(event);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventQueryStatus(event));
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
