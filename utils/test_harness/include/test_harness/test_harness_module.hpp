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

#ifndef level_zero_tests_ZE_TEST_HARNESS_MODULE_HPP
#define level_zero_tests_ZE_TEST_HARNESS_MODULE_HPP

#include "ze_api.h"
#include <string>

namespace level_zero_tests {

ze_module_handle_t create_module(ze_device_handle_t device,
                                 const std::string filename);
ze_module_handle_t create_module(ze_device_handle_t device,
                                 const std::string filename,
                                 const ze_module_format_t format,
                                 const char *build_flags,
                                 ze_module_build_log_handle_t *phBuildLog);
void destroy_module(ze_module_handle_t module);
size_t get_build_log_size(const ze_module_build_log_handle_t build_log);
std::string get_build_log_string(const ze_module_build_log_handle_t build_log);
size_t get_native_binary_size(const ze_module_handle_t module);
void save_native_binary_file(const ze_module_handle_t module,
                             const std::string filename);
void destroy_build_log(const ze_module_build_log_handle_t build_log);
void set_argument_value(ze_kernel_handle_t hFunction, uint32_t argIndex,
                        size_t argSize, const void *pArgValue);
void set_group_size(ze_kernel_handle_t hFunction, uint32_t groupSizeX,
                    uint32_t groupSizeY, uint32_t groupSizeZ);
ze_kernel_handle_t create_function(ze_module_handle_t module,
                                   std::string func_name);
ze_kernel_handle_t create_function(ze_module_handle_t module,
                                   ze_kernel_flag_t flag,
                                   std::string func_name);
void destroy_function(ze_kernel_handle_t function);

// This function is useful when only a single argument is needed.
void create_and_execute_function(ze_device_handle_t device,
                                 ze_module_handle_t module,
                                 std::string func_name, int group_size,
                                 void *arg);

struct FunctionArg {
  size_t arg_size;
  void *arg_value;
};

// Group size can only be set in x dimension
// Accepts arbitrary amounts of function arguments
void create_and_execute_function(ze_device_handle_t device,
                                 ze_module_handle_t module,
                                 std::string func_name, int group_size,
                                 const std::vector<FunctionArg> &args);
} // namespace level_zero_tests

#endif
