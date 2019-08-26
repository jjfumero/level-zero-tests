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
namespace lzt = level_zero_tests;
#include "xe_cmdqueue.h"
#include "xe_cmdlist.h"
#include "xe_device.h"
#include "xe_driver.h"
#include "xe_memory.h"
#include "xe_copy.h"
#include "xe_fence.h"
#include "xe_barrier.h"

namespace {

void print_cmdqueue_descriptor(const xe_command_queue_desc_t descriptor) {
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

class xeCommandQueueCreateTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<
          std::tuple<xe_command_queue_flag_t, xe_command_queue_mode_t,
                     xe_command_queue_priority_t>> {};

TEST_P(xeCommandQueueCreateTests,
       GivenValidDescriptorWhenCreatingCommandQueueThenSuccessIsReturned) {

  xe_command_queue_desc_t descriptor = {
      XE_COMMAND_QUEUE_DESC_VERSION_CURRENT, // version
      std::get<0>(GetParam()),               // flags
      std::get<1>(GetParam()),               // mode
      std::get<2>(GetParam())                // priority
  };
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  const xe_device_group_handle_t device_group = lzt::get_default_device_group();

  xe_device_properties_t properties;
  properties.version = XE_DEVICE_PROPERTIES_VERSION_CURRENT;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetDeviceProperties(device_group, &properties));

  if ((descriptor.flags == XE_COMMAND_QUEUE_FLAG_NONE) ||
      (descriptor.flags == XE_COMMAND_QUEUE_FLAG_SINGLE_SLICE_ONLY)) {
    EXPECT_GT(static_cast<uint32_t>(properties.numAsyncComputeEngines), 0);
    descriptor.ordinal =
        static_cast<uint32_t>(properties.numAsyncComputeEngines - 1);
  } else if (descriptor.flags == XE_COMMAND_QUEUE_FLAG_COPY_ONLY) {
    EXPECT_GT(static_cast<uint32_t>(properties.numAsyncCopyEngines), 0);
    descriptor.ordinal =
        static_cast<uint32_t>(properties.numAsyncCopyEngines - 1);
  }
  print_cmdqueue_descriptor(descriptor);

  xe_command_queue_handle_t command_queue = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandQueueCreate(device, &descriptor, &command_queue));
  EXPECT_NE(nullptr, command_queue);

  lzt::destroy_command_queue(command_queue);
}

INSTANTIATE_TEST_CASE_P(
    TestAllInputPermuations, xeCommandQueueCreateTests,
    ::testing::Combine(
        ::testing::Values(XE_COMMAND_QUEUE_FLAG_NONE,
                          XE_COMMAND_QUEUE_FLAG_COPY_ONLY,
                          XE_COMMAND_QUEUE_FLAG_LOGICAL_ONLY,
                          XE_COMMAND_QUEUE_FLAG_SINGLE_SLICE_ONLY),
        ::testing::Values(XE_COMMAND_QUEUE_MODE_DEFAULT,
                          XE_COMMAND_QUEUE_MODE_SYNCHRONOUS,
                          XE_COMMAND_QUEUE_MODE_ASYNCHRONOUS),
        ::testing::Values(XE_COMMAND_QUEUE_PRIORITY_NORMAL,
                          XE_COMMAND_QUEUE_PRIORITY_LOW,
                          XE_COMMAND_QUEUE_PRIORITY_HIGH)));

class xeCommandQueueDestroyTests : public ::testing::Test {};

TEST_F(
    xeCommandQueueDestroyTests,
    GivenValidDeviceAndNonNullCommandQueueWhenDestroyingCommandQueueThenSuccessIsReturned) {
  xe_command_queue_desc_t descriptor;
  descriptor.version = XE_COMMAND_QUEUE_DESC_VERSION_CURRENT;

  xe_command_queue_handle_t command_queue = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandQueueCreate(lzt::xeDevice::get_instance()->get_device(),
                                 &descriptor, &command_queue));
  EXPECT_NE(nullptr, command_queue);

  lzt::destroy_command_queue(command_queue);
}

struct CustomExecuteParams {
  uint32_t num_command_lists;
  uint32_t sync_timeout;
};

class xeCommandQueueExecuteCommandListTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<CustomExecuteParams> {
protected:
  void SetUp() override {
    const xe_device_handle_t device =
        lzt::xeDevice::get_instance()->get_device();
    const xe_device_group_handle_t device_group =
        lzt::get_default_device_group();
    EXPECT_GT(params.num_command_lists, 0);

    print_cmdqueue_exec(params.num_command_lists, params.sync_timeout);
    xe_command_list_desc_t list_descriptor;
    list_descriptor.version = XE_COMMAND_LIST_DESC_VERSION_CURRENT;

    for (uint32_t i = 0; i < params.num_command_lists; i++) {
      void *host_shared = nullptr;
      EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupAllocSharedMem(
                                       device_group, device,
                                       XE_DEVICE_MEM_ALLOC_FLAG_BIAS_UNCACHED,
                                       1, XE_HOST_MEM_ALLOC_FLAG_BIAS_UNCACHED,
                                       buff_size_bytes, 1, &host_shared));

      EXPECT_NE(nullptr, host_shared);
      host_buffer.push_back(static_cast<uint8_t *>(host_shared));
      void *device_shared = nullptr;
      EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupAllocSharedMem(
                                       device_group, device,
                                       XE_DEVICE_MEM_ALLOC_FLAG_BIAS_UNCACHED,
                                       1, XE_HOST_MEM_ALLOC_FLAG_BIAS_UNCACHED,
                                       buff_size_bytes, 1, &device_shared));

      EXPECT_NE(nullptr, device_shared);
      device_buffer.push_back(static_cast<uint8_t *>(device_shared));
      xe_command_list_handle_t command_list;
      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeCommandListCreate(device, &list_descriptor, &command_list));
      EXPECT_NE(nullptr, command_list);

      uint8_t *char_input = static_cast<uint8_t *>(host_shared);
      for (uint32_t j = 0; j < buff_size_bytes; j++) {
        char_input[j] = lzt::generate_value<uint8_t>(0, 255, 0);
      }
      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeCommandListAppendMemoryCopy(
                    command_list, device_buffer.at(i), host_buffer.at(i),
                    buff_size_bytes, nullptr, 0, nullptr));
      // Current synchronization tests randomly fail on Gen9HW (JIRA LOKI-301).
      // Only way to guarantee copy has occurred is to use barrier
      //         EXPECT_EQ(XE_RESULT_SUCCESS,
      //            xeCommandListAppendBarrier(command_list, nullptr, 0,
      //            nullptr));
      EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListClose(command_list));
      list_of_command_lists.push_back(command_list);
    }

    xe_command_queue_desc_t queue_descriptor;
    queue_descriptor.version = XE_COMMAND_QUEUE_DESC_VERSION_CURRENT;
    queue_descriptor.flags = XE_COMMAND_QUEUE_FLAG_COPY_ONLY;
    queue_descriptor.mode = XE_COMMAND_QUEUE_MODE_DEFAULT;
    queue_descriptor.priority = XE_COMMAND_QUEUE_PRIORITY_NORMAL;

    xe_device_properties_t properties;
    properties.version = XE_DEVICE_PROPERTIES_VERSION_CURRENT;
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeDeviceGroupGetDeviceProperties(device_group, &properties));
    EXPECT_GT((uint32_t)properties.numAsyncCopyEngines, 0);
    queue_descriptor.ordinal = (uint32_t)properties.numAsyncCopyEngines - 1;
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandQueueCreate(device, &queue_descriptor, &command_queue));
    EXPECT_NE(nullptr, command_queue);
  }

  void TearDown() override {
    for (uint32_t i = 0; i < params.num_command_lists; i++) {
      EXPECT_EQ(
          0, memcmp(host_buffer.at(i), device_buffer.at(i), buff_size_bytes));
      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeCommandListDestroy(list_of_command_lists.at(i)));
      lzt::free_memory(host_buffer.at(i));
      lzt::free_memory(device_buffer.at(i));
    }
    lzt::destroy_command_queue(command_queue);
  }

  const uint32_t buff_size_bytes = 12;
  CustomExecuteParams params = GetParam();

  xe_command_queue_handle_t command_queue = nullptr;
  std::vector<xe_command_list_handle_t> list_of_command_lists;
  std::vector<uint8_t *> host_buffer;
  std::vector<uint8_t *> device_buffer;
};

class xeCommandQueueExecuteCommandListTestsSynchronize
    : public xeCommandQueueExecuteCommandListTests {};

TEST_P(
    xeCommandQueueExecuteCommandListTestsSynchronize,
    GivenCommandQueueSynchronizationWhenExecutingCommandListsThenSuccessIsReturned) {

  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandQueueExecuteCommandLists(
                                   command_queue, params.num_command_lists,
                                   list_of_command_lists.data(), nullptr));
  xe_result_t sync_status = XE_RESULT_NOT_READY;
  auto start = std::chrono::high_resolution_clock::now();
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> realtime = end - start;
  double test_timeout = 10.0; // seconds
  while ((sync_status != XE_RESULT_SUCCESS) &&
         (realtime.count() < test_timeout)) {
    EXPECT_EQ(sync_status, XE_RESULT_NOT_READY);
    sync_status = xeCommandQueueSynchronize(command_queue, params.sync_timeout);
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
                        xeCommandQueueExecuteCommandListTestsSynchronize,
                        testing::ValuesIn(synchronize_test_input));

class xeCommandQueueExecuteCommandListTestsFence
    : public xeCommandQueueExecuteCommandListTests {};

TEST_P(
    xeCommandQueueExecuteCommandListTestsFence,
    GivenFenceSynchronizationWhenExecutingCommandListsThenSuccessIsReturned) {

  xe_fence_desc_t fence_descriptor;
  fence_descriptor.version = XE_FENCE_DESC_VERSION_CURRENT;
  xe_fence_handle_t hFence = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFenceCreate(command_queue, &fence_descriptor, &hFence));
  EXPECT_NE(nullptr, hFence);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandQueueExecuteCommandLists(
                                   command_queue, params.num_command_lists,
                                   list_of_command_lists.data(), hFence));
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFenceHostSynchronize(hFence, params.sync_timeout));
  EXPECT_EQ(XE_RESULT_SUCCESS, xeFenceDestroy(hFence));
}

CustomExecuteParams fence_test_input[] = {
    {1, UINT32_MAX},  {2, UINT32_MAX},  {3, UINT32_MAX},  {4, UINT32_MAX},
    {5, UINT32_MAX},  {6, UINT32_MAX},  {7, UINT32_MAX},  {8, UINT32_MAX},
    {9, UINT32_MAX},  {10, UINT32_MAX}, {20, UINT32_MAX}, {30, UINT32_MAX},
    {50, UINT32_MAX}, {100, UINT32_MAX}};

INSTANTIATE_TEST_CASE_P(TestIncreasingNumberCommandListsWithCommandQueueFence,
                        xeCommandQueueExecuteCommandListTestsFence,
                        testing::ValuesIn(fence_test_input));

} // namespace
