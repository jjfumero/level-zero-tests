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
#include "random/random.hpp"
#include "logging/logging.hpp"
#include <chrono>
#include <thread>
namespace lzt = level_zero_tests;
#include "ze_api.h"

namespace {

void print_cmdqueue_descriptor(const ze_command_queue_desc_t descriptor) {
  LOG_INFO << "VERSION = " << descriptor.version
           << "   FLAG = " << descriptor.flags
           << "   MODE = " << descriptor.mode
           << "   PRIORITY = " << descriptor.priority
           << "   ORDINAL = " << descriptor.ordinal;
}

void print_cmdqueue_exec(const uint32_t num_command_lists,
                         const uint32_t sync_timeout) {
  LOG_INFO << " num_command_lists = " << num_command_lists
           << " sync_timeout = " << sync_timeout;
}

class zeCommandQueueCreateTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<
          std::tuple<ze_command_queue_flag_t, ze_command_queue_mode_t,
                     ze_command_queue_priority_t>> {};

TEST_P(zeCommandQueueCreateTests,
       GivenValidDescriptorWhenCreatingCommandQueueThenSuccessIsReturned) {

  ze_command_queue_desc_t descriptor = {
      ZE_COMMAND_QUEUE_DESC_VERSION_CURRENT, // version
      std::get<0>(GetParam()),               // flags
      std::get<1>(GetParam()),               // mode
      std::get<2>(GetParam())                // priority
  };
  const ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  const ze_driver_handle_t driver = lzt::get_default_driver();

  ze_device_properties_t properties;
  properties.version = ZE_DEVICE_PROPERTIES_VERSION_CURRENT;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceGetProperties(device, &properties));

  if ((descriptor.flags == ZE_COMMAND_QUEUE_FLAG_NONE) ||
      (descriptor.flags == ZE_COMMAND_QUEUE_FLAG_SINGLE_SLICE_ONLY)) {
    EXPECT_GT(static_cast<uint32_t>(properties.numAsyncComputeEngines), 0);
    descriptor.ordinal =
        static_cast<uint32_t>(properties.numAsyncComputeEngines - 1);
  } else if (descriptor.flags == ZE_COMMAND_QUEUE_FLAG_COPY_ONLY) {
    EXPECT_GT(static_cast<uint32_t>(properties.numAsyncCopyEngines), 0);
    descriptor.ordinal =
        static_cast<uint32_t>(properties.numAsyncCopyEngines - 1);
  }
  print_cmdqueue_descriptor(descriptor);

  ze_command_queue_handle_t command_queue = nullptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandQueueCreate(device, &descriptor, &command_queue));
  EXPECT_NE(nullptr, command_queue);

  lzt::destroy_command_queue(command_queue);
}

INSTANTIATE_TEST_CASE_P(
    TestAllInputPermuations, zeCommandQueueCreateTests,
    ::testing::Combine(
        ::testing::Values(ZE_COMMAND_QUEUE_FLAG_NONE,
                          ZE_COMMAND_QUEUE_FLAG_COPY_ONLY,
                          ZE_COMMAND_QUEUE_FLAG_LOGICAL_ONLY,
                          ZE_COMMAND_QUEUE_FLAG_SINGLE_SLICE_ONLY),
        ::testing::Values(ZE_COMMAND_QUEUE_MODE_DEFAULT,
                          ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS,
                          ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS),
        ::testing::Values(ZE_COMMAND_QUEUE_PRIORITY_NORMAL,
                          ZE_COMMAND_QUEUE_PRIORITY_LOW,
                          ZE_COMMAND_QUEUE_PRIORITY_HIGH)));

class zeCommandQueueDestroyTests : public ::testing::Test {};

TEST_F(
    zeCommandQueueDestroyTests,
    GivenValidDeviceAndNonNullCommandQueueWhenDestroyingCommandQueueThenSuccessIsReturned) {
  ze_command_queue_desc_t descriptor;
  descriptor.version = ZE_COMMAND_QUEUE_DESC_VERSION_CURRENT;

  ze_command_queue_handle_t command_queue = nullptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandQueueCreate(lzt::zeDevice::get_instance()->get_device(),
                                 &descriptor, &command_queue));
  EXPECT_NE(nullptr, command_queue);

  lzt::destroy_command_queue(command_queue);
}

struct CustomExecuteParams {
  uint32_t num_command_lists;
  uint32_t sync_timeout;
};

class zeCommandQueueExecuteCommandListTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<CustomExecuteParams> {
protected:
  void SetUp() override {
    const ze_device_handle_t device =
        lzt::zeDevice::get_instance()->get_device();
    const ze_driver_handle_t driver = lzt::get_default_driver();
    EXPECT_GT(params.num_command_lists, 0);

    print_cmdqueue_exec(params.num_command_lists, params.sync_timeout);
    ze_command_list_desc_t list_descriptor;
    list_descriptor.version = ZE_COMMAND_LIST_DESC_VERSION_CURRENT;

    for (uint32_t i = 0; i < params.num_command_lists; i++) {
      void *host_shared = nullptr;
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeDriverAllocSharedMem(driver, device,
                                       ZE_DEVICE_MEM_ALLOC_FLAG_BIAS_UNCACHED,
                                       1, ZE_HOST_MEM_ALLOC_FLAG_BIAS_UNCACHED,
                                       buff_size_bytes, 1, &host_shared));

      EXPECT_NE(nullptr, host_shared);
      host_buffer.push_back(static_cast<uint8_t *>(host_shared));
      void *device_shared = nullptr;
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeDriverAllocSharedMem(driver, device,
                                       ZE_DEVICE_MEM_ALLOC_FLAG_BIAS_UNCACHED,
                                       1, ZE_HOST_MEM_ALLOC_FLAG_BIAS_UNCACHED,
                                       buff_size_bytes, 1, &device_shared));

      EXPECT_NE(nullptr, device_shared);
      device_buffer.push_back(static_cast<uint8_t *>(device_shared));
      ze_command_list_handle_t command_list;
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeCommandListCreate(device, &list_descriptor, &command_list));
      EXPECT_NE(nullptr, command_list);

      uint8_t *char_input = static_cast<uint8_t *>(host_shared);
      for (uint32_t j = 0; j < buff_size_bytes; j++) {
        char_input[j] = lzt::generate_value<uint8_t>(0, 255, 0);
      }
      lzt::append_memory_copy(command_list, device_buffer.at(i),
                              host_buffer.at(i), buff_size_bytes, nullptr);
      // Current synchronization tests randomly fail on Gen9HW (JIRA LOKI-301).
      // Only way to guarantee copy has occurred is to use barrier
      //         EXPECT_EQ(ZE_RESULT_SUCCESS,
      //            zeCommandListAppendBarrier(command_list, nullptr, 0,
      //            nullptr));
      EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(command_list));
      list_of_command_lists.push_back(command_list);
    }

    ze_command_queue_desc_t queue_descriptor;
    queue_descriptor.version = ZE_COMMAND_QUEUE_DESC_VERSION_CURRENT;
    queue_descriptor.flags = ZE_COMMAND_QUEUE_FLAG_COPY_ONLY;
    queue_descriptor.mode = ZE_COMMAND_QUEUE_MODE_DEFAULT;
    queue_descriptor.priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL;

    ze_device_properties_t properties;
    properties.version = ZE_DEVICE_PROPERTIES_VERSION_CURRENT;
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceGetProperties(device, &properties));
    EXPECT_GT((uint32_t)properties.numAsyncCopyEngines, 0);
    queue_descriptor.ordinal = (uint32_t)properties.numAsyncCopyEngines - 1;
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandQueueCreate(device, &queue_descriptor, &command_queue));
    EXPECT_NE(nullptr, command_queue);
  }

  void TearDown() override {
    for (uint32_t i = 0; i < params.num_command_lists; i++) {
      EXPECT_EQ(
          0, memcmp(host_buffer.at(i), device_buffer.at(i), buff_size_bytes));
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeCommandListDestroy(list_of_command_lists.at(i)));
      lzt::free_memory(host_buffer.at(i));
      lzt::free_memory(device_buffer.at(i));
    }
    lzt::destroy_command_queue(command_queue);
  }

  const uint32_t buff_size_bytes = 12;
  CustomExecuteParams params = GetParam();

  ze_command_queue_handle_t command_queue = nullptr;
  std::vector<ze_command_list_handle_t> list_of_command_lists;
  std::vector<uint8_t *> host_buffer;
  std::vector<uint8_t *> device_buffer;
};

class zeCommandQueueExecuteCommandListTestsSynchronize
    : public zeCommandQueueExecuteCommandListTests {};

TEST_P(
    zeCommandQueueExecuteCommandListTestsSynchronize,
    GivenCommandQueueSynchronizationWhenExecutingCommandListsThenSuccessIsReturned) {

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandQueueExecuteCommandLists(
                                   command_queue, params.num_command_lists,
                                   list_of_command_lists.data(), nullptr));
  ze_result_t sync_status = ZE_RESULT_NOT_READY;
  auto start = std::chrono::high_resolution_clock::now();
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> realtime = end - start;
  double test_timeout = 10.0; // seconds
  while ((sync_status != ZE_RESULT_SUCCESS) &&
         (realtime.count() < test_timeout)) {
    EXPECT_EQ(sync_status, ZE_RESULT_NOT_READY);
    sync_status = zeCommandQueueSynchronize(command_queue, params.sync_timeout);
    end = std::chrono::high_resolution_clock::now();
    realtime = end - start;
  }
  EXPECT_LT(realtime.count(), test_timeout); // timeout after 10 seconds
}

CustomExecuteParams synchronize_test_input[] = {{1, 0},
                                                {2, UINT32_MAX >> 30},
                                                {3, UINT32_MAX >> 28},
                                                {4, UINT32_MAX >> 26},
                                                {5, UINT32_MAX >> 24},
                                                {6, UINT32_MAX >> 22},
                                                {7, UINT32_MAX >> 20},
                                                {8, UINT32_MAX >> 18},
                                                {9, UINT32_MAX >> 16},
                                                {10, UINT32_MAX >> 8},
                                                {20, UINT32_MAX >> 4},
                                                {30, UINT32_MAX >> 2},
                                                {50, UINT32_MAX >> 1},
                                                {100, UINT32_MAX}};

INSTANTIATE_TEST_CASE_P(TestIncreasingNumberCommandListsWithSynchronize,
                        zeCommandQueueExecuteCommandListTestsSynchronize,
                        testing::ValuesIn(synchronize_test_input));

class zeCommandQueueExecuteCommandListTestsFence
    : public zeCommandQueueExecuteCommandListTests {};

TEST_P(
    zeCommandQueueExecuteCommandListTestsFence,
    GivenFenceSynchronizationWhenExecutingCommandListsThenSuccessIsReturned) {

  ze_fence_desc_t fence_descriptor;
  fence_descriptor.version = ZE_FENCE_DESC_VERSION_CURRENT;
  ze_fence_handle_t hFence = nullptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeFenceCreate(command_queue, &fence_descriptor, &hFence));
  EXPECT_NE(nullptr, hFence);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandQueueExecuteCommandLists(
                                   command_queue, params.num_command_lists,
                                   list_of_command_lists.data(), hFence));
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeFenceHostSynchronize(hFence, params.sync_timeout));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeFenceDestroy(hFence));
}

CustomExecuteParams fence_test_input[] = {
    {1, UINT32_MAX},  {2, UINT32_MAX},  {3, UINT32_MAX},  {4, UINT32_MAX},
    {5, UINT32_MAX},  {6, UINT32_MAX},  {7, UINT32_MAX},  {8, UINT32_MAX},
    {9, UINT32_MAX},  {10, UINT32_MAX}, {20, UINT32_MAX}, {30, UINT32_MAX},
    {50, UINT32_MAX}, {100, UINT32_MAX}};

INSTANTIATE_TEST_CASE_P(TestIncreasingNumberCommandListsWithCommandQueueFence,
                        zeCommandQueueExecuteCommandListTestsFence,
                        testing::ValuesIn(fence_test_input));

static void ExecuteCommandQueue(ze_command_queue_handle_t &cq,
                                ze_command_list_handle_t &cl,
                                volatile bool &exited) {
  exited = false;
  lzt::execute_command_lists(cq, 1, &cl, nullptr);
  exited = true;
}

class zeCommandQueueSynchronousTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<ze_command_queue_mode_t> {};

TEST_P(
    zeCommandQueueSynchronousTests,
    GivenModeWhenCreatingCommandQueueThenSynchronousOrAsynchronousBehaviorIsConfirmed) {
  ze_command_queue_mode_t mode = GetParam();
  ze_command_queue_handle_t cq = lzt::create_command_queue(mode);
  ze_command_list_handle_t cl = lzt::create_command_list();
  lzt::zeEventPool ep;
  ze_event_handle_t hEvent;

  ep.create_event(hEvent, ZE_EVENT_SCOPE_FLAG_HOST, ZE_EVENT_SCOPE_FLAG_NONE);
  // Verify Host Reads Event as unset
  EXPECT_EQ(ZE_RESULT_NOT_READY, zeEventHostSynchronize(hEvent, 0));

  lzt::append_wait_on_events(cl, 1, &hEvent);
  lzt::close_command_list(cl);
  volatile bool exited = false;
  std::thread child_thread(&ExecuteCommandQueue, std::ref(cq), std::ref(cl),
                           std::ref(exited));

  // sleep for 5 seconds to give execution a chance to complete
  std::chrono::seconds timespan(5);
  std::this_thread::sleep_for(timespan);

  // We expect the child thread to exit if we are in async mode:
  if (exited)
    EXPECT_EQ(ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS, mode);
  else
    // We expect if we are in synchronous mode that the child thread will never
    // exit:

    // This test failse, and the corresponding LOKI ticket is: LOKI-588

    EXPECT_EQ(ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS, mode);

  // Note: if the command queue has a mode of: ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS
  // It won't ever finish unless we signal the event that is waiting on:
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventHostSignal(hEvent));

  child_thread.join();

  ep.destroy_event(hEvent);
  lzt::destroy_command_list(cl);
  lzt::destroy_command_queue(cq);
}

INSTANTIATE_TEST_CASE_P(SynchronousAndAsynchronousCommandQueueTests,
                        zeCommandQueueSynchronousTests,
                        testing::Values(ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS,
                                        ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS));

} // namespace
