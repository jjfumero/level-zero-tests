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

namespace compute_samples {

xe_fence_handle_t create_fence(xe_command_queue_handle_t cmd_queue) {
  xe_fence_handle_t fence;
  xe_fence_desc_t desc;
  desc.version = XE_FENCE_DESC_VERSION_CURRENT;
  desc.flags = XE_FENCE_FLAG_NONE;
  EXPECT_EQ(XE_RESULT_SUCCESS, xeFenceCreate(cmd_queue, &desc, &fence));
  return fence;
}

void destroy_fence(xe_fence_handle_t fence) {

  EXPECT_EQ(XE_RESULT_SUCCESS, xeFenceDestroy(fence));
}

void reset_fence(xe_fence_handle_t fence) {

  EXPECT_EQ(XE_RESULT_SUCCESS, xeFenceReset(fence));
}

xe_result_t query_fence(xe_fence_handle_t fence) {
  xe_result_t result;
  EXPECT_EQ(XE_RESULT_SUCCESS, result = xeFenceQueryStatus(fence));
  return result;
}

xe_result_t sync_fence(xe_fence_handle_t fence, uint32_t timeout) {

  xe_result_t result;
  EXPECT_EQ(XE_RESULT_SUCCESS, result = xeFenceHostSynchronize(fence, timeout));
  return result;
}

}; // namespace compute_samples
