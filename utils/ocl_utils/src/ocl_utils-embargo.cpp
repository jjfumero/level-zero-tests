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

#include "ocl_utils/ocl_utils-embargo.hpp"

#include <boost/compute/intel/buffer.hpp>
#include <boost/compute/intel/command_queue.hpp>
#include <boost/compute/intel/context.hpp>
#include <boost/compute/intel/device.hpp>
#include <boost/compute/intel/platform.hpp>

namespace level_zero_tests {

compute::context create_multi_tile_context(compute::device &device) {
  std::vector<cl_context_properties> properties;
  properties.push_back(CL_CONTEXT_FLAGS_INTEL);
  properties.push_back(CL_CONTEXT_ALLOW_ALL_KIND_OF_QUEUES_INTEL);
  properties.push_back(0);
  return compute::context(device, properties.data());
}

compute::command_queue
create_multi_tile_queue(const compute::device &device,
                        const compute::context &context, const int tile_id,
                        const int family_id,
                        std::vector<cl_queue_properties> properties) {
  if (!properties.empty()) {
    // if vector is not empty it should have null at last place, remove it
    properties.pop_back();
  }
  if (tile_id > -1) {
    properties.push_back(CL_QUEUE_TILE_ID_INTEL);
    properties.push_back(tile_id);
  }
  if (family_id > -1) {
    properties.push_back(CL_QUEUE_FAMILY_INTEL);
    properties.push_back(family_id);
  }
  properties.push_back(0);
  return compute::command_queue_intel(context, device, properties.data());
}

compute::command_queue
create_multi_tile_queue(const compute::device &device,
                        const compute::context &context,
                        std::vector<cl_queue_properties> properties) {
  return create_multi_tile_queue(device, context, -1, -1, properties);
}

compute::buffer create_buffer_on_tile(const compute::context &context,
                                      cl_mem_flags flags, size_t size,
                                      void *data, const int tile_id) {
  std::vector<cl_mem_properties_intel> properties;
  if (flags != 0) {
    properties.push_back(CL_MEM_FLAGS);
    properties.push_back(flags);
  }
  if (tile_id > -1) {
    properties.push_back(CL_MEM_TILE_ID_INTEL);
    properties.push_back(tile_id);
  }
  properties.push_back(0);
  return compute::buffer_intel(context, properties.data(), size, data);
}

cl_uint get_tile_id(const compute::command_queue &queue) {
  const compute::command_queue_intel q(queue);
  return q.tile_id();
}

void enqueue_verify_memory(compute::command_queue &queue, const void *actual,
                           const void *expected, const size_t size) {
  compute::command_queue_intel q(queue);
  q.enqueue_verify_memory(actual, expected, size, CL_COMPARE_EQUAL);
}

void add_comment(const compute::platform &platform, const char *comment) {
  const compute::platform_intel p(platform);
  p.add_comment(comment);
}

void add_comment(const compute::platform &platform, const std::string comment) {
  const compute::platform_intel p(platform);
  p.add_comment(comment);
}

} // namespace level_zero_tests
