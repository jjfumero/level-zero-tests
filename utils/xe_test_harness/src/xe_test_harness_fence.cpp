/*
 * Copyright(c) 2019 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "gtest/gtest.h"
#include "xe_test_harness/xe_test_harness_fence.hpp"

namespace level_zero_tests {

ze_fence_handle_t create_fence(ze_command_queue_handle_t cmd_queue) {
  ze_fence_handle_t fence;
  ze_fence_desc_t desc;
  desc.version = ZE_FENCE_DESC_VERSION_CURRENT;
  desc.flags = ZE_FENCE_FLAG_NONE;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeFenceCreate(cmd_queue, &desc, &fence));
  return fence;
}

void destroy_fence(ze_fence_handle_t fence) {

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeFenceDestroy(fence));
}

void reset_fence(ze_fence_handle_t fence) {

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeFenceReset(fence));
}

ze_result_t query_fence(ze_fence_handle_t fence) {
  ze_result_t result;
  EXPECT_EQ(ZE_RESULT_SUCCESS, result = zeFenceQueryStatus(fence));
  return result;
}

ze_result_t sync_fence(ze_fence_handle_t fence, uint32_t timeout) {

  ze_result_t result;
  EXPECT_EQ(ZE_RESULT_SUCCESS, result = zeFenceHostSynchronize(fence, timeout));
  return result;
}

}; // namespace level_zero_tests
