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

#ifndef BOOST_COMPUTE_INTEL_BUFFER_HPP
#define BOOST_COMPUTE_INTEL_BUFFER_HPP

#include <boost/compute/buffer.hpp>

#include "ocl_entrypoints/embargo/cl_intel_create_buffer_with_properties_intel.h"

namespace boost {
namespace compute {

class buffer_intel : public buffer {
  using buffer::buffer;

public:
  explicit buffer_intel(const buffer &other) : buffer(other) {}

  buffer_intel(const context &context,
               const cl_mem_properties_intel *properties, size_t size,
               void *host_ptr = nullptr) {
    cl_int error = 0;

    m_mem = clCreateBufferWithPropertiesINTEL(
        context, properties, (std::max)(size, size_t(1)), host_ptr, &error);

    if (!m_mem) {
      BOOST_THROW_EXCEPTION(opencl_error(error));
    }
  }

  operator buffer() const { return buffer(m_mem, false); }
};

} // namespace compute
} // namespace boost

#endif // BOOST_COMPUTE_INTEL_BUFFER_HPP
