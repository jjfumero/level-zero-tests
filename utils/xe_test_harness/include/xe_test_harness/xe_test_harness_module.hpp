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
xe_module_handle_t create_module(xe_device_handle_t device,
                                 const std::string filename,
                                 const xe_module_format_t format,
                                 const char *build_flags,
                                 xe_module_build_log_handle_t *phBuildLog);
void destroy_module(xe_module_handle_t module);
size_t get_build_log_size(const xe_module_build_log_handle_t build_log);
std::string get_build_log_string(const xe_module_build_log_handle_t build_log);
size_t get_native_binary_size(const xe_module_handle_t module);
void save_native_binary_file(const xe_module_handle_t module,
                             const std::string filename);
void destroy_build_log(const xe_module_build_log_handle_t build_log);

xe_function_handle_t create_function(xe_module_handle_t module,
                                     std::string func_name);
xe_function_handle_t create_function(xe_module_handle_t module,
                                     xe_function_flag_t flag,
                                     std::string func_name);
void destroy_function(xe_function_handle_t function);

// This function is useful when only a single argument is needed.
void create_and_execute_function(xe_device_handle_t device,
                                 xe_module_handle_t module,
                                 std::string func_name, int group_size,
                                 void *arg);

struct FunctionArg {
  size_t arg_size;
  void *arg_value;
};

// Group size can only be set in x dimension
// Accepts arbitrary amounts of function arguments
void create_and_execute_function(xe_device_handle_t device,
                                 xe_module_handle_t module,
                                 std::string func_name, int group_size,
                                 const std::vector<FunctionArg> &args);
} // namespace compute_samples

#endif
