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

#include "ocl_entrypoints/embargo/cl_intel_create_buffer_with_properties_intel.h"
#include "ocl_entrypoints/loader.hpp"

namespace lzt = level_zero_tests;

CL_API_ENTRY cl_mem CL_API_CALL clCreateBufferWithPropertiesINTEL(
    cl_context context, const cl_mem_properties_intel *properties, size_t size,
    void *hostPtr, cl_int *errcodeRet) CL_API_SUFFIX__VERSION_1_2 {
  const auto e = lzt::load_entrypoint<clCreateBufferWithPropertiesINTEL_fn>(
      context, "clCreateBufferWithPropertiesINTEL");
  return e(context, properties, size, hostPtr, errcodeRet);
}
