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

#include "xe_test_harness/xe_test_harness.hpp"
#include "xe_utils/xe_utils.hpp"
#include "utils/utils.hpp"
#include "gtest/gtest.h"
#include "xe_barrier.h"
#include <thread>
#include <chrono>

namespace level_zero_tests {

xe_module_handle_t create_module(xe_device_handle_t device,
                                 const std::string filename) {

  return (create_module(device, filename, XE_MODULE_FORMAT_IL_SPIRV, nullptr,
                        nullptr));
}

xe_module_handle_t create_module(xe_device_handle_t device,
                                 const std::string filename,
                                 const xe_module_format_t format,
                                 const char *build_flags,
                                 xe_module_build_log_handle_t *p_build_log) {

  xe_module_desc_t module_description;
  xe_module_handle_t module;
  const std::vector<uint8_t> binary_file =
      level_zero_tests::load_binary_file(filename);

  EXPECT_TRUE((format == XE_MODULE_FORMAT_IL_SPIRV) ||
              (format == XE_MODULE_FORMAT_NATIVE));
  module_description.version = XE_MODULE_DESC_VERSION_CURRENT;
  module_description.format = format;
  module_description.inputSize = static_cast<uint32_t>(binary_file.size());
  module_description.pInputModule = binary_file.data();
  module_description.pBuildFlags = build_flags;

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeModuleCreate(device, &module_description, &module, p_build_log));

  return module;
}

size_t get_build_log_size(const xe_module_build_log_handle_t build_log) {
  size_t build_log_size = 0;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeModuleBuildLogGetString(build_log, &build_log_size, nullptr));
  EXPECT_GT(build_log_size, 0);
  return build_log_size;
}

std::string get_build_log_string(const xe_module_build_log_handle_t build_log) {
  size_t build_log_size = 0;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeModuleBuildLogGetString(build_log, &build_log_size, nullptr));

  EXPECT_GT(build_log_size, 0);

  std::vector<char> build_log_c_string(build_log_size);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeModuleBuildLogGetString(build_log, &build_log_size,
                                      build_log_c_string.data()));
  return std::string(build_log_c_string.begin(), build_log_c_string.end());
}

size_t get_native_binary_size(const xe_module_handle_t module) {
  size_t native_binary_size = 0;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeModuleGetNativeBinary(module, &native_binary_size, nullptr));
  EXPECT_GT(native_binary_size, 0);
  return native_binary_size;
}

void save_native_binary_file(const xe_module_handle_t module,
                             const std::string filename) {
  size_t native_binary_size = 0;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeModuleGetNativeBinary(module, &native_binary_size, nullptr));
  EXPECT_GT(native_binary_size, 0);

  std::vector<uint8_t> native_binary(native_binary_size);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeModuleGetNativeBinary(module, &native_binary_size,
                                    native_binary.data()));
  level_zero_tests::save_binary_file(native_binary, filename);
}

void destroy_build_log(const xe_module_build_log_handle_t build_log) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeModuleBuildLogDestroy(build_log));
}

void set_argument_value(xe_function_handle_t hFunction, uint32_t argIndex,
                        size_t argSize, const void *pArgValue) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionSetArgumentValue(hFunction, argIndex,
                                                          argSize, pArgValue));
}

void set_group_size(xe_function_handle_t hFunction, uint32_t groupSizeX,
                    uint32_t groupSizeY, uint32_t groupSizeZ) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionSetGroupSize(hFunction, groupSizeX,
                                                      groupSizeY, groupSizeZ));
}

void destroy_module(xe_module_handle_t module) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeModuleDestroy(module));
}

xe_function_handle_t create_function(xe_module_handle_t module,
                                     std::string func_name) {
  return create_function(module, XE_FUNCTION_FLAG_NONE, func_name);
}

xe_function_handle_t create_function(xe_module_handle_t module,
                                     xe_function_flag_t flag,
                                     std::string func_name) {
  xe_function_handle_t function;
  xe_function_desc_t function_description;
  EXPECT_TRUE((flag == XE_FUNCTION_FLAG_NONE) ||
              (flag == XE_FUNCTION_FLAG_FORCE_RESIDENCY));
  function_description.version = XE_FUNCTION_DESC_VERSION_CURRENT;
  function_description.flags = flag;
  function_description.pFunctionName = func_name.c_str();

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionCreate(module, &function_description, &function));
  return function;
}

void destroy_function(xe_function_handle_t function) {

  EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionDestroy(function));
}

// Currently limited to creating function with 1d group and single argument.
// Expand as needed.
void create_and_execute_function(xe_device_handle_t device,
                                 xe_module_handle_t module,
                                 std::string func_name, int group_size,
                                 void *arg) {
  std::vector<FunctionArg> args;
  FunctionArg func_arg{sizeof(arg), &arg};
  args.push_back(func_arg);
  create_and_execute_function(device, module, func_name, group_size, args);
}

void create_and_execute_function(xe_device_handle_t device,
                                 xe_module_handle_t module,
                                 std::string func_name, int group_size,
                                 const std::vector<FunctionArg> &args) {

  xe_function_handle_t function = create_function(module, func_name);
  xe_command_list_handle_t cmdlist = create_command_list(device);
  xe_command_queue_handle_t cmdq = create_command_queue(device);
  uint32_t group_size_x = group_size;
  uint32_t group_size_y = 1;
  uint32_t group_size_z = 1;
  EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionSuggestGroupSize(
                                   function, group_size, 1, 1, &group_size_x,
                                   &group_size_y, &group_size_z));

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionSetGroupSize(function, group_size_x, group_size_y,
                                   group_size_z));

  int i = 0;
  for (auto arg : args) {
    EXPECT_EQ(
        XE_RESULT_SUCCESS,
        xeFunctionSetArgumentValue(function, i++, arg.arg_size, arg.arg_value));
  }

  xe_thread_group_dimensions_t thread_group_dimensions;
  thread_group_dimensions.groupCountX = 1;
  thread_group_dimensions.groupCountY = 1;
  thread_group_dimensions.groupCountZ = 1;

  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendLaunchFunction(
                                   cmdlist, function, &thread_group_dimensions,
                                   nullptr, 0, nullptr));

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendBarrier(cmdlist, nullptr, 0, nullptr));
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListClose(cmdlist));

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandQueueExecuteCommandLists(cmdq, 1, &cmdlist, nullptr));

  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandQueueSynchronize(cmdq, UINT32_MAX));

  destroy_function(function);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandQueueDestroy(cmdq));
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListDestroy(cmdlist));
}

} // namespace level_zero_tests
