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

#ifndef level_zero_tests_XE_TEST_HARNESS_FENCE_HPP
#define level_zero_tests_XE_TEST_HARNESS_FENCE_HPP

#include "xe_fence.h"

namespace level_zero_tests {

xe_fence_handle_t create_fence(xe_command_queue_handle_t cmd_queue);
void destroy_fence(xe_fence_handle_t fence);
void reset_fence(xe_fence_handle_t fence);
xe_result_t query_fence(xe_fence_handle_t fence);
xe_result_t sync_fence(xe_fence_handle_t fence, uint32_t timeout);

}; // namespace level_zero_tests

#endif
