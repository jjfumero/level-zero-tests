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

#ifndef BOOST_COMPUTE_INTEL_COMMAND_QUEUE_HPP
#define BOOST_COMPUTE_INTEL_COMMAND_QUEUE_HPP

#include <boost/compute/command_queue.hpp>

#include "ocl_entrypoints/embargo/cl_intel_aubs.h"
#include "ocl_entrypoints/embargo/cl_intel_unified_shared_memory.h"
#include "ocl_entrypoints/embargo/cl_intel_command_queue_properties.h"

namespace boost {
namespace compute {

class command_queue_intel : public command_queue {
  using command_queue::command_queue;

public:
  command_queue_intel() : command_queue() {}

  explicit command_queue_intel(const command_queue &other)
      : command_queue(other) {}

  command_queue_intel(const context &context, const device &device,
                      const cl_queue_properties *queue_properties)
      : command_queue(create_command_queue_with_properties(context, device,
                                                           queue_properties),
                      false) {}

  void enqueue_verify_memory(const void *actual_data, const void *expected_data,
                             size_t size_of_comparison,
                             cl_uint comparison_mode) {

    cl_int ret = clEnqueueVerifyMemoryINTEL(
        get(), actual_data, expected_data, size_of_comparison, comparison_mode);

    if (ret != CL_SUCCESS) {
      BOOST_THROW_EXCEPTION(opencl_error(ret));
    }
  }

  template <typename T>
  void enqueue_verify_memory(const void *actual_data,
                             const std::vector<T> &expected_data,
                             cl_uint comparison_mode) {
    enqueue_verify_memory(actual_data, expected_data.data(),
                          expected_data.size() * sizeof(T), comparison_mode);
  }

  event enqueue_memset(void *dst_ptr, cl_int value, size_t size,
                       const wait_list &events = wait_list()) {
    event event_;

    cl_int ret =
        clEnqueueMemsetINTEL(get(), dst_ptr, value, size, events.size(),
                             events.get_event_ptr(), &event_.get());

    if (ret != CL_SUCCESS) {
      BOOST_THROW_EXCEPTION(opencl_error(ret));
    }

    return event_;
  }

  event enqueue_memcpy(void *dst_ptr, const void *src_ptr, size_t size,
                       const wait_list &events = wait_list()) {
    event event_;

    cl_int ret = clEnqueueMemcpyINTEL(get(), CL_TRUE, dst_ptr, src_ptr, size,
                                      events.size(), events.get_event_ptr(),
                                      &event_.get());

    if (ret != CL_SUCCESS) {
      BOOST_THROW_EXCEPTION(opencl_error(ret));
    }

    return event_;
  }

  event enqueue_memcpy_async(void *dst_ptr, const void *src_ptr, size_t size,
                             const wait_list &events = wait_list()) {
    event event_;

    cl_int ret = clEnqueueMemcpyINTEL(get(), CL_FALSE, dst_ptr, src_ptr, size,
                                      events.size(), events.get_event_ptr(),
                                      &event_.get());

    if (ret != CL_SUCCESS) {
      BOOST_THROW_EXCEPTION(opencl_error(ret));
    }

    return event_;
  }

  event enqueue_migrate_mem(const void *ptr, size_t size,
                            cl_mem_migration_flags flags,
                            const wait_list &events = wait_list()) {
    event event_;

    cl_int ret =
        clEnqueueMigrateMemINTEL(get(), ptr, size, flags, events.size(),
                                 events.get_event_ptr(), &event_.get());

    if (ret != CL_SUCCESS) {
      BOOST_THROW_EXCEPTION(opencl_error(ret));
    }

    return event_;
  }

  event enqueue_mem_advise(const void *ptr, size_t size,
                           cl_mem_advice_intel advice,
                           const wait_list &events = wait_list()) {
    event event_;

    cl_int ret =
        clEnqueueMemAdviseINTEL(get(), ptr, size, advice, events.size(),
                                events.get_event_ptr(), &event_.get());

    if (ret != CL_SUCCESS) {
      BOOST_THROW_EXCEPTION(opencl_error(ret));
    }

    return event_;
  }

  cl_uint tile_id() const { return get_info<cl_uint>(CL_QUEUE_TILE_ID_INTEL); }

  cl_uint tile_id_mask() const {
    return get_info<cl_uint>(CL_QUEUE_TILE_ID_MASK_INTEL);
  }

  cl_uint family() const { return get_info<cl_uint>(CL_QUEUE_FAMILY_INTEL); }

  operator command_queue() const { return command_queue(get(), false); }

private:
  static cl_command_queue create_command_queue_with_properties(
      const context &context, const device &device,
      const cl_queue_properties *queue_properties) {
    cl_int error = CL_SUCCESS;
    cl_command_queue q = clCreateCommandQueueWithProperties(
        context, device.id(), queue_properties, &error);
    if (!q) {
      BOOST_THROW_EXCEPTION(opencl_error(error));
    }
    return q;
  }
};

} // namespace compute
} // namespace boost

#endif // BOOST_COMPUTE_INTEL_COMMAND_QUEUE_HPP
