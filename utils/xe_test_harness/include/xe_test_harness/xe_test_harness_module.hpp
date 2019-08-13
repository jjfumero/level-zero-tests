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

#ifndef COMPUTE_SAMPLES_XE_TEST_HARNESS_MODULE_HPP
#define COMPUTE_SAMPLES_XE_TEST_HARNESS_MODULE_HPP

#include "xe_module.h"
#include <string>

namespace compute_samples {

xe_module_handle_t create_module(xe_device_handle_t device,
                                 const std::string filename);
void destroy_module(xe_module_handle_t module);

xe_function_handle_t create_function(xe_module_handle_t module,
                                     std::string func_name);
void destroy_function(xe_function_handle_t function);

void create_and_execute_function(xe_device_handle_t device,
                                 xe_module_handle_t module,
                                 std::string func_name, int group_size,
                                 void *arg);

} // namespace compute_samples

#endif
