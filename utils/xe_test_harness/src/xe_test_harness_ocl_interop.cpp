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

#include "gtest/gtest.h"
#include "xe_test_harness/xe_test_harness.hpp"

namespace level_zero_tests {

void *ocl_register_memory(cl_context context, cl_mem mem) {
  void *ptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceRegisterCLMemory(
                                   lzt::zeDevice::get_instance()->get_device(),
                                   context, mem, &ptr));
  return ptr;
}

ze_command_queue_handle_t
ocl_register_commandqueue(cl_context context, cl_command_queue command_queue) {

  ze_command_queue_handle_t l0_command_queue;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceRegisterCLCommandQueue(
                                   lzt::zeDevice::get_instance()->get_device(),
                                   context, command_queue, &l0_command_queue));
  return l0_command_queue;
}

ze_module_handle_t ocl_register_program(cl_context context,
                                        cl_program program) {

  ze_module_handle_t module_handle = nullptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceRegisterCLProgram(
                                   lzt::zeDevice::get_instance()->get_device(),
                                   context, program, &module_handle));
  return module_handle;
}

}; // namespace level_zero_tests
