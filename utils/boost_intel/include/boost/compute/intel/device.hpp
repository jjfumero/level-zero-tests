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

#ifndef BOOST_COMPUTE_INTEL_DEVICE_HPP
#define BOOST_COMPUTE_INTEL_DEVICE_HPP

#include <boost/compute/device.hpp>

#include "ocl_entrypoints/embargo/cl_intel_command_queue_properties.h"
#include "ocl_entrypoints/embargo/cl_intel_unified_shared_memory.h"
#include "ocl_entrypoints/embargo/cl_intel_function_pointers.h"

namespace boost {
namespace compute {

using function_pointer_intel = cl_ulong;

class device_intel : public device {
  using device::device;

public:
  explicit device_intel(const device &other) : device(other) {}

  cl_uint num_tiles() const {
    return get_info<cl_uint>(CL_DEVICE_NUM_TILES_INTEL);
  }

  cl_uint num_queue_families() const {
    return get_info<cl_uint>(CL_DEVICE_NUM_QUEUE_FAMILIES_INTEL);
  }

  cl_unified_shared_memory_capabilities_intel host_mem_capabilities() const {
    return get_info<cl_unified_shared_memory_capabilities_intel>(
        CL_DEVICE_HOST_MEM_CAPABILITIES_INTEL);
  }

  cl_unified_shared_memory_capabilities_intel device_mem_capabilities() const {
    return get_info<cl_unified_shared_memory_capabilities_intel>(
        CL_DEVICE_DEVICE_MEM_CAPABILITIES_INTEL);
  }

  cl_unified_shared_memory_capabilities_intel
  single_device_shared_mem_capabilities() const {
    return get_info<cl_unified_shared_memory_capabilities_intel>(
        CL_DEVICE_SINGLE_DEVICE_SHARED_MEM_CAPABILITIES_INTEL);
  }

  cl_unified_shared_memory_capabilities_intel
  cross_device_shared_mem_capabilities() const {
    return get_info<cl_unified_shared_memory_capabilities_intel>(
        CL_DEVICE_CROSS_DEVICE_SHARED_MEM_CAPABILITIES_INTEL);
  }

  cl_unified_shared_memory_capabilities_intel
  shared_system_mem_capabilities() const {
    return get_info<cl_unified_shared_memory_capabilities_intel>(
        CL_DEVICE_SHARED_SYSTEM_MEM_CAPABILITIES_INTEL);
  }

  function_pointer_intel get_function_pointer(const program &program,
                                              const char *function_name) const {
    function_pointer_intel pointer;
    cl_int ret = clGetDeviceFunctionPointerINTEL(get(), program.get(),
                                                 function_name, &pointer);

    if (ret != CL_SUCCESS) {
      BOOST_THROW_EXCEPTION(opencl_error(ret));
    }

    return pointer;
  }
};

} // namespace compute
} // namespace boost

#endif // BOOST_COMPUTE_INTEL_DEVICE_HPP
