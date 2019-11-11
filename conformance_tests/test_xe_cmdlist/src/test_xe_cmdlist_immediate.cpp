/*
 * INTEL CONFIDENTIAL
 *
 * Copyright 2019 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code (Material) are owned by Intel Corporation or its
 * suppliers or licensors.
 *
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors. The Material contains trade secrets and proprietary and
 * confidential information of Intel or its suppliers and licensors.
 * The Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure
 * or delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 */

#include "gtest/gtest.h"

#include "xe_utils/xe_utils.hpp"
#include "xe_test_harness/xe_test_harness.hpp"
#include "logging/logging.hpp"

namespace lzt = level_zero_tests;

#include "ze_api.h"

namespace {

class zeImmediateCommandListExecutionTests
    : public lzt::zeEventPoolTests,
      public ::testing::WithParamInterface<ze_command_queue_mode_t> {};

TEST_P(
    zeImmediateCommandListExecutionTests,
    GivenImmediateCommandListWhenAppendingMemorySetInstructionThenVerifyImmediateExecution) {
  ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  const size_t size = 16;
  uint32_t timeout = UINT32_MAX;
  ze_command_queue_mode_t mode = GetParam();
  if (mode == ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS) {
    timeout = 0;
  } else {
    // Test may hang in HW until LOKI-589 resolved
  }
  ep.InitEventPool(1);
  ze_event_handle_t event0;
  ep.create_event(event0, ZE_EVENT_SCOPE_FLAG_HOST, ZE_EVENT_SCOPE_FLAG_NONE);

  void *buffer = lzt::allocate_shared_memory(size * sizeof(int));
  memset(buffer, 0x0, size * sizeof(int));

  EXPECT_EQ(ZE_RESULT_NOT_READY, zeEventQueryStatus(event0));
  ze_command_list_handle_t cmdlist_immediate =
      lzt::create_immediate_command_list(device, ZE_COMMAND_QUEUE_FLAG_NONE,
                                         mode, ZE_COMMAND_QUEUE_PRIORITY_NORMAL,
                                         0);
  // setting event on following instruction should flush memory
  lzt::append_memory_set(cmdlist_immediate, buffer, 0x1, size * sizeof(int),
                         event0);
  // command queue execution should be immediate, and so no timeout required for
  // synchronize
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventHostSynchronize(event0, timeout));
  for (size_t i = 0; i < size * sizeof(int); i++) {
    EXPECT_EQ(static_cast<uint8_t *>(buffer)[i], 0x1);
  }

  ep.destroy_event(event0);
  lzt::destroy_command_list(cmdlist_immediate);
  lzt::free_memory(buffer);
}

TEST_P(
    zeImmediateCommandListExecutionTests,
    GivenImmediateCommandListWhenAppendLaunchKernelInstructionThenVerifyImmediateExecution) {
  ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  const size_t size = 16;
  const int addval = 10;
  const int addval2 = 15;
  uint32_t timeout = UINT32_MAX;
  ze_command_queue_mode_t mode = GetParam();
  if (mode == ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS) {
    timeout = 0;
  } else {
    // Test may hang in HW until LOKI-589 resolved
  }

  ep.InitEventPool(1);
  ze_event_handle_t event0;
  ep.create_event(event0, ZE_EVENT_SCOPE_FLAG_HOST, ZE_EVENT_SCOPE_FLAG_NONE);

  void *buffer = lzt::allocate_shared_memory(size * sizeof(int));
  memset(buffer, 0x0, size * sizeof(int));

  EXPECT_EQ(ZE_RESULT_NOT_READY, zeEventQueryStatus(event0));
  ze_command_list_handle_t cmdlist_immediate =
      lzt::create_immediate_command_list(device, ZE_COMMAND_QUEUE_FLAG_NONE,
                                         mode, ZE_COMMAND_QUEUE_PRIORITY_NORMAL,
                                         0);

  ze_module_handle_t module =
      lzt::create_module(device, "xe_cmdlist_add.spv",
                         ZE_MODULE_FORMAT_IL_SPIRV, nullptr, nullptr);
  ze_kernel_handle_t kernel =
      lzt::create_function(module, "xe_cmdlist_add_constant");
  lzt::set_group_size(kernel, 1, 1, 1);

  int *p_dev = static_cast<int *>(buffer);
  lzt::set_argument_value(kernel, 0, sizeof(p_dev), &p_dev);
  lzt::set_argument_value(kernel, 1, sizeof(addval), &addval);
  ze_thread_group_dimensions_t tg;
  tg.groupCountX = static_cast<uint32_t>(size);
  tg.groupCountY = 1;
  tg.groupCountZ = 1;
  // setting event on following instruction should flush memory
  lzt::append_launch_function(cmdlist_immediate, kernel, &tg, event0, 0,
                              nullptr);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventHostSynchronize(event0, timeout));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventReset(event0));
  EXPECT_EQ(ZE_RESULT_NOT_READY, zeEventQueryStatus(event0));
  for (size_t i = 0; i < size; i++) {
    EXPECT_EQ(static_cast<int *>(buffer)[i], addval);
  }
  lzt::set_argument_value(kernel, 1, sizeof(addval2), &addval2);
  // setting event on following instruction should flush memory
  lzt::append_launch_function(cmdlist_immediate, kernel, &tg, event0, 0,
                              nullptr);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventHostSynchronize(event0, timeout));
  for (size_t i = 0; i < size; i++) {
    EXPECT_EQ(static_cast<int *>(buffer)[i], (addval + addval2));
  }
  ep.destroy_event(event0);
  lzt::destroy_command_list(cmdlist_immediate);
  lzt::destroy_function(kernel);
  lzt::destroy_module(module);
  lzt::free_memory(buffer);
}

INSTANTIATE_TEST_CASE_P(TestCasesforCommandListImmediateCases,
                        zeImmediateCommandListExecutionTests,
                        testing::Values(ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                        ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS,
                                        ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS));

} // namespace
