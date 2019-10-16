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
#include "xe_test_harness/xe_test_harness_fence.hpp"
#include "logging/logging.hpp"
#include <chrono>
#include <thread>

namespace lzt = level_zero_tests;

#include "ze_api.h"

namespace {

class zeCommandQueueCreateFenceTests : public lzt::zeCommandQueueTests {};

TEST_F(
    zeCommandQueueCreateFenceTests,
    GivenDefaultFenceDescriptorWhenCreatingFenceThenNotNullPointerIsReturned) {
  ze_fence_desc_t descriptor;
  descriptor.version = ZE_FENCE_DESC_VERSION_CURRENT;

  ze_fence_handle_t fence = nullptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeFenceCreate(cq.command_queue_, &descriptor, &fence));
  EXPECT_NE(nullptr, fence);

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeFenceDestroy(fence));
}

class zeFenceTests : public ::testing::Test {
public:
  zeFenceTests() : cq(), cl() {
    ze_fence_desc_t descriptor;
    descriptor.version = ZE_FENCE_DESC_VERSION_CURRENT;

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeFenceCreate(cq.command_queue_, &descriptor, &fence_));
    EXPECT_NE(nullptr, fence_);
  }
  ~zeFenceTests() { EXPECT_EQ(ZE_RESULT_SUCCESS, zeFenceDestroy(fence_)); }

protected:
  ze_fence_handle_t fence_ = nullptr;
  lzt::zeCommandQueue cq;
  lzt::zeCommandList cl;
};

class zeFenceSynchronizeTests : public zeFenceTests,
                                public ::testing::WithParamInterface<uint32_t> {
};

TEST_P(zeFenceSynchronizeTests,
       GivenSignaledFenceWhenSynchronizingThenSuccessIsReturned) {

  const std::vector<int8_t> input = {72, 101, 108, 108, 111, 32,
                                     87, 111, 114, 108, 100, 33};

  void *output_buffer = lzt::allocate_device_memory(input.size());

  lzt::append_memory_copy(cl.command_list_, output_buffer, input.data(),
                          input.size(), nullptr);

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(cl.command_list_));

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandQueueExecuteCommandLists(cq.command_queue_, 1,
                                              &cl.command_list_, fence_));

  // sleep for a bit to give execution a chance to complete
  std::chrono::milliseconds timespan(100);
  std::this_thread::sleep_for(timespan);

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeFenceHostSynchronize(fence_, GetParam()));

  lzt::free_memory(output_buffer);
}

INSTANTIATE_TEST_CASE_P(FenceSyncParameterizedTest, zeFenceSynchronizeTests,
                        ::testing::Values(0, 3, UINT32_MAX));

TEST_F(zeFenceSynchronizeTests,
       GivenSignaledFenceWhenQueryingThenSuccessIsReturned) {

  const std::vector<int8_t> input = {72, 101, 108, 108, 111, 32,
                                     87, 111, 114, 108, 100, 33};

  void *output_buffer = lzt::allocate_device_memory(input.size());

  lzt::append_memory_copy(cl.command_list_, output_buffer, input.data(),
                          input.size(), nullptr);

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(cl.command_list_));

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandQueueExecuteCommandLists(cq.command_queue_, 1,
                                              &cl.command_list_, fence_));

  // sleep for a bit to give execution a chance to complete
  std::chrono::milliseconds timespan(100);
  std::this_thread::sleep_for(timespan);

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeFenceQueryStatus(fence_));
  lzt::free_memory(output_buffer);
}

TEST_F(zeFenceSynchronizeTests,
       GivenSignaledFenceWhenResettingThenSuccessIsReturned) {

  const std::vector<int8_t> input = {72, 101, 108, 108, 111, 32,
                                     87, 111, 114, 108, 100, 33};

  void *output_buffer = lzt::allocate_device_memory(input.size());

  lzt::append_memory_copy(cl.command_list_, output_buffer, input.data(),
                          input.size(), nullptr);

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(cl.command_list_));

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandQueueExecuteCommandLists(cq.command_queue_, 1,
                                              &cl.command_list_, fence_));

  lzt::sync_fence(fence_, UINT32_MAX);

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeFenceReset(fence_));

  lzt::free_memory(output_buffer);
}

TEST_F(zeFenceSynchronizeTests,
       GivenSignaledFenceWhenResettingThenFenceBecomesInvalidWhenQueried) {

  const std::vector<int8_t> input = {72, 101, 108, 108, 111, 32,
                                     87, 111, 114, 108, 100, 33};

  void *output_buffer = lzt::allocate_device_memory(input.size());

  lzt::append_memory_copy(cl.command_list_, output_buffer, input.data(),
                          input.size(), nullptr);

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(cl.command_list_));

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandQueueExecuteCommandLists(cq.command_queue_, 1,
                                              &cl.command_list_, fence_));

  lzt::sync_fence(fence_, UINT32_MAX);
  lzt::reset_fence(fence_);

  EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT, zeFenceQueryStatus(fence_));
  lzt::free_memory(output_buffer);
}

class zeMultipleFenceTests : public lzt::zeEventPoolTests,
                             public ::testing::WithParamInterface<bool> {};

TEST_P(
    zeMultipleFenceTests,
    GivenMultipleCommandQueuesWhenFenceAndEventSetThenVerifyAllSignaledSuccessful) {
  ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  ze_device_properties_t properties;
  properties.version = ZE_DEVICE_PROPERTIES_VERSION_CURRENT;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceGetProperties(device, &properties));
  // maximum simultaneous command queues
  const size_t num_async_compute =
      static_cast<size_t>(properties.numAsyncComputeEngines);
  const size_t num_async_copy =
      static_cast<size_t>(properties.numAsyncCopyEngines);
  const size_t num_cmdq = num_async_compute + num_async_copy;
  bool use_event = GetParam();

  const size_t size = 16;
  std::vector<ze_event_handle_t> host_to_dev_event(num_cmdq, nullptr);
  if (use_event) {
    FAIL(); // Must fail until event signaling issues resolved
            // Multiple issues noted (LOKI-537, LOKI-551)
    ep.InitEventPool(num_cmdq);
  }
  std::vector<ze_fence_handle_t> fence(num_cmdq, nullptr);
  std::vector<ze_command_queue_handle_t> cmdq(num_cmdq, nullptr);
  std::vector<ze_command_list_handle_t> cmdlist(num_cmdq, nullptr);
  std::vector<void *> buffer(num_cmdq, nullptr);
  std::vector<uint8_t> val(num_cmdq, 0);
  ze_command_list_flag_t cmdlist_flag = ZE_COMMAND_LIST_FLAG_NONE;
  ze_command_queue_flag_t cmdq_flag = ZE_COMMAND_QUEUE_FLAG_NONE;
  uint32_t ordinal = 0;

  for (size_t i = 0; i < num_cmdq; i++) {
    if (i == num_async_compute) {
      cmdlist_flag = ZE_COMMAND_LIST_FLAG_COPY_ONLY;
      cmdq_flag = ZE_COMMAND_QUEUE_FLAG_COPY_ONLY;
      ordinal = 0;
    }
    cmdq[i] = lzt::create_command_queue(
        device, cmdq_flag, ZE_COMMAND_QUEUE_MODE_DEFAULT,
        ZE_COMMAND_QUEUE_PRIORITY_NORMAL, ordinal);
    cmdlist[i] = lzt::create_command_list(device, cmdlist_flag);
    fence[i] = lzt::create_fence(cmdq[i]);
    buffer[i] = lzt::allocate_shared_memory(size);
    val[i] = static_cast<uint8_t>(i + 1);

    if (use_event) {
      ep.create_event(host_to_dev_event[i], ZE_EVENT_SCOPE_FLAG_HOST,
                      ZE_EVENT_SCOPE_FLAG_HOST);
      lzt::append_wait_on_events(cmdlist[i], 1, &host_to_dev_event[i]);
    }
    lzt::append_memory_set(cmdlist[i], buffer[i], val[i], size);
    lzt::append_barrier(cmdlist[i], nullptr, 0, nullptr);
    lzt::close_command_list(cmdlist[i]);
    ordinal++;
  }

  // attempt to execute command queues simutanesously
  for (size_t i = 0; i < num_cmdq; i++) {
    lzt::execute_command_lists(cmdq[i], 1, &cmdlist[i], fence[i]);
  }

  if (use_event) {
    for (size_t i = 0; i < num_cmdq; i++) {
      EXPECT_EQ(ZE_RESULT_NOT_READY, zeFenceQueryStatus(fence[i]));
      EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventHostSignal(host_to_dev_event[i]));
    }
  }

  for (size_t i = 0; i < num_cmdq; i++) {
    lzt::sync_fence(fence[i], UINT32_MAX);
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeFenceQueryStatus(fence[i]));
    for (size_t j = 0; j < size; j++) {
      EXPECT_EQ(static_cast<uint8_t *>(buffer[i])[j], val[i]);
    }
  }

  for (size_t i = 0; i < num_cmdq; i++) {
    if (use_event) {
      ep.destroy_event(host_to_dev_event[i]);
    }
    lzt::destroy_command_queue(cmdq[i]);
    lzt::destroy_command_list(cmdlist[i]);
    lzt::destroy_fence(fence[i]);
    lzt::free_memory(buffer[i]);
  }
}

INSTANTIATE_TEST_CASE_P(TestMultipleFenceWithAndWithoutEvent,
                        zeMultipleFenceTests, testing::Values(false, true));

} // namespace

// TODO: Test different fence flags
// TODO: Test fences on different types of command queues
// TODO: Test fences between command lists
// TODO: Test fence on a device using OpenCL C kernels
