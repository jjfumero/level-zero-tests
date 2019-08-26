/*
 * Copyright(c) 2018 Intel Corporation
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

#ifndef level_zero_tests_OCL_UTILS_EMBARGO_HPP
#define level_zero_tests_OCL_UTILS_EMBARGO_HPP

#include <vector>

#include <boost/compute/core.hpp>
#include <boost/compute/intel/command_queue.hpp>
namespace compute = boost::compute;

namespace level_zero_tests {
compute::context create_multi_tile_context(compute::device &device);
compute::command_queue
create_multi_tile_queue(const compute::device &device,
                        const compute::context &context, const int tile_id = -1,
                        const int family_id = -1,
                        std::vector<cl_queue_properties> properties =
                            std::vector<cl_queue_properties>());
compute::command_queue
create_multi_tile_queue(const compute::device &device,
                        const compute::context &context,
                        std::vector<cl_queue_properties> properties =
                            std::vector<cl_queue_properties>());
compute::buffer create_buffer_on_tile(const compute::context &context,
                                      cl_mem_flags flags, size_t size,
                                      void *data, const int tile_id = -1);

cl_uint get_tile_id(const compute::command_queue &queue);

void enqueue_verify_memory(compute::command_queue &queue, const void *actual,
                           const void *expected, const size_t size);
template <typename T>
void enqueue_verify_memory(compute::command_queue &queue, const void *actual,
                           const std::vector<T> &expected) {
  compute::command_queue_intel q(queue);
  q.enqueue_verify_memory(actual, expected, CL_COMPARE_EQUAL);
}

void add_comment(const compute::platform &platform, const char *comment);
void add_comment(const compute::platform &platform, const std::string comment);
} // namespace level_zero_tests

#endif
