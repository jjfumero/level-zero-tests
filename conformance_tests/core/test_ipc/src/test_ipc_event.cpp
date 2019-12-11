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
#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"
#include "logging/logging.hpp"
#include "test_ipc_event.hpp"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/process.hpp>

#include "ze_api.h"

namespace lzt = level_zero_tests;
namespace bipc = boost::interprocess;
namespace {

static const ze_event_desc_t defaultEventDesc = {
    ZE_EVENT_DESC_VERSION_CURRENT, 5, ZE_EVENT_SCOPE_FLAG_NONE,
    ZE_EVENT_SCOPE_FLAG_HOST, // ensure memory coherency across device and Host
                              // after event signalled
};

ze_event_pool_desc_t defaultEventPoolDesc = {
    ZE_EVENT_POOL_DESC_VERSION_CURRENT,
    (ze_event_pool_flag_t)(ZE_EVENT_POOL_FLAG_HOST_VISIBLE |
                           ZE_EVENT_POOL_FLAG_IPC),
    10};

static lzt::zeEventPool get_event_pool(bool multi_device) {
  lzt::zeEventPool ep;
  if (multi_device) {
    auto devices = lzt::get_devices(lzt::get_default_driver());
    ep.InitEventPool(defaultEventPoolDesc, devices);
  } else {
    ze_device_handle_t device = lzt::get_default_device();
    ep.InitEventPool(defaultEventPoolDesc);
  }
  return ep;
}

static void parent_host_signals(ze_event_handle_t hEvent) {
  lzt::signal_event_from_host(hEvent);
}

static void parent_device_signals(ze_event_handle_t hEvent) {
  auto cmdlist = lzt::create_command_list();
  auto cmdqueue = lzt::create_command_queue();
  lzt::append_signal_event(cmdlist, hEvent);
  lzt::execute_command_lists(cmdqueue, 1, &cmdlist, nullptr);
  lzt::synchronize(cmdqueue, UINT32_MAX);

  // cleanup
  lzt::destroy_command_list(cmdlist);
  lzt::destroy_command_queue(cmdqueue);
}

static void run_ipc_event_test(parent_test_t parent_test,
                               child_test_t child_test, bool multi_device) {
  auto ep = get_event_pool(multi_device);
  ze_ipc_event_pool_handle_t hIpcEventPool;
  ep.get_ipc_handle(&hIpcEventPool);
  if (testing::Test::HasFatalFailure())
    return; // Abort test if IPC Event handle failed

  ze_event_handle_t hEvent;
  ep.create_event(hEvent, defaultEventDesc);
  shared_data_t test_data = {parent_test, child_test, hIpcEventPool,
                             multi_device};
  bipc::shared_memory_object::remove("ipc_event_test");
  bipc::shared_memory_object shm(bipc::create_only, "ipc_event_test",
                                 bipc::read_write);
  shm.truncate(sizeof(shared_data_t));
  bipc::mapped_region region(shm, bipc::read_write);
  std::memcpy(region.get_address(), &test_data, sizeof(shared_data_t));

  // launch child
  boost::process::child c("./ipc/test_ipc_event_helper");

  switch (parent_test) {
  case PARENT_TEST_HOST_SIGNALS:
    parent_host_signals(hEvent);
    break;
  case PARENT_TEST_DEVICE_SIGNALS:
    parent_device_signals(hEvent);
    break;
  default:
    FAIL() << "Fatal test error";
  }

  c.wait(); // wait for the process to exit
  ASSERT_EQ(c.exit_code(), 0);

  // cleanup
  bipc::shared_memory_object::remove("ipc_event_test");
  ep.destroy_event(hEvent);
}

TEST(
    zeIPCEventTests,
    GivenTwoProcessesWhenEventSignaledByHostInParentThenEventSetinChildFromHostPerspective) {
  run_ipc_event_test(PARENT_TEST_HOST_SIGNALS, CHILD_TEST_HOST_READS, false);
}

TEST(
    zeIPCEventTests,
    GivenTwoProcessesWhenEventSignaledByDeviceInParentThenEventSetinChildFromHostPerspective) {
  run_ipc_event_test(PARENT_TEST_DEVICE_SIGNALS, CHILD_TEST_HOST_READS, false);
}

TEST(
    zeIPCEventTests,
    GivenTwoProcessesWhenEventSignaledByDeviceInParentThenEventSetinChildFromDevicePerspective) {
  run_ipc_event_test(PARENT_TEST_DEVICE_SIGNALS, CHILD_TEST_DEVICE_READS,
                     false);
}

TEST(
    zeIPCEventTests,
    GivenTwoProcessesWhenEventSignaledByHostInParentThenEventSetinChildFromDevicePerspective) {
  run_ipc_event_test(PARENT_TEST_HOST_SIGNALS, CHILD_TEST_DEVICE_READS, false);
}

TEST(
    zeIPCEventMultiDeviceTests,
    GivenTwoProcessesWhenEventSignaledByDeviceInParentThenEventSetinChildFromSecondDevicePerspective) {
  ASSERT_GT(lzt::get_ze_device_count(), 1);
  run_ipc_event_test(PARENT_TEST_DEVICE_SIGNALS, CHILD_TEST_DEVICE2_READS,
                     true);
}

TEST(
    zeIPCEventMultiDeviceTests,
    GivenTwoProcessesWhenEventSignaledByHostInParentThenEventSetinChildFromMultipleDevicePerspective) {
  ASSERT_GT(lzt::get_ze_device_count(), 1);
  run_ipc_event_test(PARENT_TEST_HOST_SIGNALS, CHILD_TEST_MULTI_DEVICE_READS,
                     true);
}

} // namespace
