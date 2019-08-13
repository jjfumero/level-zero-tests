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

#ifndef BOOST_COMPUTE_INTEL_CONTEXT_HPP
#define BOOST_COMPUTE_INTEL_CONTEXT_HPP

#include <boost/compute/context.hpp>

#include "ocl_entrypoints/embargo/cl_intel_context_properties.h"
#include "ocl_entrypoints/embargo/cl_intel_unified_shared_memory.h"

namespace boost {
namespace compute {

class context_intel : public context {
  using context::context;

public:
  explicit context_intel(const context &other) : context(other) {}

  void *host_mem_alloc(const cl_mem_properties_intel *properties, size_t size,
                       cl_uint alignment) {
    cl_int error = CL_SUCCESS;
    void *p = clHostMemAllocINTEL(get(), properties, size, alignment, &error);
    if (!p) {
      BOOST_THROW_EXCEPTION(opencl_error(error));
    }
    return p;
  }

  void *device_mem_alloc(device device,
                         const cl_mem_properties_intel *properties, size_t size,
                         cl_uint alignment) {
    cl_int error = CL_SUCCESS;
    void *p = clDeviceMemAllocINTEL(get(), device.get(), properties, size,
                                    alignment, &error);
    if (!p) {
      BOOST_THROW_EXCEPTION(opencl_error(error));
    }
    return p;
  }

  void *shared_mem_alloc(device device,
                         const cl_mem_properties_intel *properties, size_t size,
                         cl_uint alignment) {
    cl_int error = CL_SUCCESS;
    void *p = clSharedMemAllocINTEL(get(), device.get(), properties, size,
                                    alignment, &error);
    if (!p) {
      BOOST_THROW_EXCEPTION(opencl_error(error));
    }
    return p;
  }

  void mem_free(const void *ptr) {
    cl_int error = clMemFreeINTEL(get(), ptr);
    if (error != CL_SUCCESS) {
      BOOST_THROW_EXCEPTION(opencl_error(error));
    }
  }

  template <typename T>
  T get_mem_alloc_info(const void *ptr, cl_mem_info_intel info) {
    T value;
    cl_int error =
        clGetMemAllocInfoINTEL(get(), ptr, info, sizeof(T), &value, nullptr);
    if (error != CL_SUCCESS) {
      BOOST_THROW_EXCEPTION(opencl_error(error));
    }
    return value;
  }

  operator context() const { return context(get(), false); }
};

} // namespace compute
} // namespace boost

#endif // BOOST_COMPUTE_INTEL_CONTEXT_HPP
