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

namespace compute_samples {

xe_module_handle_t create_module(xe_device_handle_t device,
                                 const std::string filename) {

  xe_module_desc_t module_description;
  xe_module_handle_t module;
  const std::vector<uint8_t> binary_file =
      compute_samples::load_binary_file(filename);

  module_description.version = XE_MODULE_DESC_VERSION_CURRENT;
  module_description.format = XE_MODULE_FORMAT_IL_SPIRV;
  module_description.inputSize = static_cast<uint32_t>(binary_file.size());
  module_description.pInputModule = binary_file.data();
  module_description.pBuildFlags = nullptr;

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeModuleCreate(device, &module_description, &module, nullptr));

  return module;
}

void destroy_module(xe_module_handle_t module) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeModuleDestroy(module));
}

xe_function_handle_t create_function(xe_module_handle_t module,
                                     std::string func_name) {
  xe_function_handle_t function;
  xe_function_desc_t function_description;

  function_description.version = XE_FUNCTION_DESC_VERSION_CURRENT;
  function_description.flags = XE_FUNCTION_FLAG_NONE;
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

  xe_function_handle_t function = create_function(module, func_name);
  xe_command_list_handle_t cmdlist = create_command_list(device);
  xe_command_queue_handle_t cmdq = create_command_queue(device);
  uint32_t group_size_x = 0;
  uint32_t group_size_y = 0;
  uint32_t group_size_z = 0;
  EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionSuggestGroupSize(
                                   function, group_size, 1, 1, &group_size_x,
                                   &group_size_y, &group_size_z));

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionSetGroupSize(function, group_size_x, group_size_y,
                                   group_size_z));

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionSetArgumentValue(function, 0, sizeof(arg), &arg));

  xe_thread_group_dimensions_t thread_group_dimensions;
  thread_group_dimensions.groupCountX = group_size_x;
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

  // FIXME: LOKI-301 - xeCommandQueueSynchronize is preferred over sleep but it
  // is currently not working
  // EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandQueueSynchronize(cmdq, UINT32_MAX));

  std::chrono::milliseconds timespan(500);
  std::this_thread::sleep_for(timespan);

  destroy_function(function);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandQueueDestroy(cmdq));
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListDestroy(cmdlist));
}

} // namespace compute_samples
