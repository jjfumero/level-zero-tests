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

#include "ocl_entrypoints/embargo/cl_intel_aubs.h"
#include "ocl_entrypoints/loader.hpp"

namespace cs = compute_samples;

CL_API_ENTRY cl_int CL_API_CALL clEnqueueVerifyMemoryINTEL(
    cl_command_queue command_queue, const void *actual_data,
    const void *expected_data, size_t size_of_comparison,
    cl_uint comparison_mode) CL_API_SUFFIX__VERSION_1_0 {
  const auto e = cs::load_entrypoint<clEnqueueVerifyMemoryINTEL_fn>(
      command_queue, "clEnqueueVerifyMemoryINTEL");
  return e(command_queue, actual_data, expected_data, size_of_comparison,
           comparison_mode);
}

CL_API_ENTRY cl_int CL_API_CALL clAddCommentINTEL(
    cl_platform_id platform, const char *comment) CL_API_SUFFIX__VERSION_2_1 {
  const auto e =
      cs::load_entrypoint<clAddCommentINTEL_fn>(platform, "clAddCommentINTEL");
  return e(platform, comment);
}
