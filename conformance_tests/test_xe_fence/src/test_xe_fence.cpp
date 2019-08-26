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

#include "xe_driver.h"
#include "xe_fence.h"
#include "xe_memory.h"
#include "xe_copy.h"

namespace {

class xeCommandQueueCreateFenceTests : public lzt::xeCommandQueueTests {};

TEST_F(
    xeCommandQueueCreateFenceTests,
    GivenDefaultFenceDescriptorWhenCreatingFenceThenNotNullPointerIsReturned) {
  xe_fence_desc_t descriptor;
  descriptor.version = XE_FENCE_DESC_VERSION_CURRENT;

  xe_fence_handle_t fence = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFenceCreate(cq.command_queue_, &descriptor, &fence));
  EXPECT_NE(nullptr, fence);

  EXPECT_EQ(XE_RESULT_SUCCESS, xeFenceDestroy(fence));
}

class xeFenceTests : public ::testing::Test {
public:
  xeFenceTests() : cq(), cl() {
    xe_fence_desc_t descriptor;
    descriptor.version = XE_FENCE_DESC_VERSION_CURRENT;

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFenceCreate(cq.command_queue_, &descriptor, &fence_));
    EXPECT_NE(nullptr, fence_);
  }
  ~xeFenceTests() { EXPECT_EQ(XE_RESULT_SUCCESS, xeFenceDestroy(fence_)); }

protected:
  xe_fence_handle_t fence_ = nullptr;
  lzt::xeCommandQueue cq;
  lzt::xeCommandList cl;
};

class xeFenceSynchronizeTests : public xeFenceTests,
                                public ::testing::WithParamInterface<uint32_t> {
};

TEST_P(xeFenceSynchronizeTests,
       GivenSignaledFenceWhenSynchronizingThenSuccessIsReturned) {

  const std::vector<int8_t> input = {72, 101, 108, 108, 111, 32,
                                     87, 111, 114, 108, 100, 33};

  void *output_buffer = lzt::allocate_device_memory(input.size());

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemoryCopy(cl.command_list_, output_buffer,
                                          input.data(), input.size(), nullptr,
                                          0, nullptr));

  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListClose(cl.command_list_));

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandQueueExecuteCommandLists(cq.command_queue_, 1,
                                              &cl.command_list_, fence_));

  // sleep for a bit to give execution a chance to complete
  std::chrono::milliseconds timespan(100);
  std::this_thread::sleep_for(timespan);

  EXPECT_EQ(XE_RESULT_SUCCESS, xeFenceHostSynchronize(fence_, GetParam()));

  lzt::free_memory(output_buffer);
}

INSTANTIATE_TEST_CASE_P(FenceSyncParameterizedTest, xeFenceSynchronizeTests,
                        ::testing::Values(0, 3, UINT32_MAX));

TEST_F(xeFenceSynchronizeTests,
       GivenSignaledFenceWhenQueryingThenSuccessIsReturned) {

  const std::vector<int8_t> input = {72, 101, 108, 108, 111, 32,
                                     87, 111, 114, 108, 100, 33};

  void *output_buffer = lzt::allocate_device_memory(input.size());

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemoryCopy(cl.command_list_, output_buffer,
                                          input.data(), input.size(), nullptr,
                                          0, nullptr));

  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListClose(cl.command_list_));

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandQueueExecuteCommandLists(cq.command_queue_, 1,
                                              &cl.command_list_, fence_));

  // sleep for a bit to give execution a chance to complete
  std::chrono::milliseconds timespan(100);
  std::this_thread::sleep_for(timespan);

  EXPECT_EQ(XE_RESULT_SUCCESS, xeFenceQueryStatus(fence_));
  lzt::free_memory(output_buffer);
}

TEST_F(xeFenceSynchronizeTests,
       GivenSignaledFenceWhenResettingThenSuccessIsReturned) {

  const std::vector<int8_t> input = {72, 101, 108, 108, 111, 32,
                                     87, 111, 114, 108, 100, 33};

  void *output_buffer = lzt::allocate_device_memory(input.size());

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemoryCopy(cl.command_list_, output_buffer,
                                          input.data(), input.size(), nullptr,
                                          0, nullptr));

  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListClose(cl.command_list_));

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandQueueExecuteCommandLists(cq.command_queue_, 1,
                                              &cl.command_list_, fence_));

  lzt::sync_fence(fence_, UINT32_MAX);

  EXPECT_EQ(XE_RESULT_SUCCESS, xeFenceReset(fence_));

  lzt::free_memory(output_buffer);
}

TEST_F(xeFenceSynchronizeTests,
       GivenSignaledFenceWhenResettingThenFenceBecomesInvalidWhenQueried) {

  const std::vector<int8_t> input = {72, 101, 108, 108, 111, 32,
                                     87, 111, 114, 108, 100, 33};

  void *output_buffer = lzt::allocate_device_memory(input.size());

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemoryCopy(cl.command_list_, output_buffer,
                                          input.data(), input.size(), nullptr,
                                          0, nullptr));

  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListClose(cl.command_list_));

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandQueueExecuteCommandLists(cq.command_queue_, 1,
                                              &cl.command_list_, fence_));

  lzt::sync_fence(fence_, UINT32_MAX);
  lzt::reset_fence(fence_);

  EXPECT_EQ(XE_RESULT_ERROR_INVALID_PARAMETER, xeFenceQueryStatus(fence_));
  lzt::free_memory(output_buffer);
}

} // namespace

// TODO: Test different fence flags
// TODO: Test fences on different types of command queues
// TODO: Test fences between command lists
// TODO: Test fence on a device using OpenCL C kernels
