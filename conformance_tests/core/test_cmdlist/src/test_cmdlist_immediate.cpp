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

#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"
#include "logging/logging.hpp"

namespace lzt = level_zero_tests;

#include "ze_api.h"

namespace {

class zeImmediateCommandListExecutionTests
    : public lzt::zeEventPoolTests,
      public ::testing::WithParamInterface<ze_command_queue_mode_t> {

protected:
  void SetUp() override {
    mode = GetParam();

    if (mode == ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS) {
      timeout = 0;
    }

    ep.InitEventPool(10);
    ep.create_event(event0, ZE_EVENT_SCOPE_FLAG_HOST, ZE_EVENT_SCOPE_FLAG_HOST);

    cmdlist_immediate = lzt::create_immediate_command_list(
        lzt::zeDevice::get_instance()->get_device(), ZE_COMMAND_QUEUE_FLAG_NONE,
        mode, ZE_COMMAND_QUEUE_PRIORITY_NORMAL, 0);
  }

  void TearDown() override {
    ep.destroy_event(event0);
    lzt::destroy_command_list(cmdlist_immediate);
  }
  ze_event_handle_t event0 = nullptr;
  ze_command_list_handle_t cmdlist_immediate = nullptr;
  ze_command_queue_mode_t mode;
  uint32_t timeout = UINT32_MAX - 1;
};

TEST_P(
    zeImmediateCommandListExecutionTests,
    GivenImmediateCommandListWhenAppendingMemorySetInstructionThenVerifyImmediateExecution) {
  const size_t size = 16;
  const uint8_t one = 1;

  void *buffer = lzt::allocate_shared_memory(size * sizeof(int));
  memset(buffer, 0x0, size * sizeof(int));

  EXPECT_EQ(ZE_RESULT_NOT_READY, zeEventQueryStatus(event0));

  // setting event on following instruction should flush memory
  lzt::append_memory_set(cmdlist_immediate, buffer, &one, size * sizeof(int),
                         event0);
  // command queue execution should be immediate, and so no timeout required for
  // synchronize
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventHostSynchronize(event0, timeout));
  for (size_t i = 0; i < size * sizeof(int); i++) {
    EXPECT_EQ(static_cast<uint8_t *>(buffer)[i], 0x1);
  }

  lzt::free_memory(buffer);
}

TEST_P(
    zeImmediateCommandListExecutionTests,
    GivenImmediateCommandListWhenAppendLaunchKernelInstructionThenVerifyImmediateExecution) {
  const size_t size = 16;
  const int addval = 10;
  const int addval2 = 15;

  void *buffer = lzt::allocate_shared_memory(size * sizeof(int));
  memset(buffer, 0x0, size * sizeof(int));

  EXPECT_EQ(ZE_RESULT_NOT_READY, zeEventQueryStatus(event0));

  ze_module_handle_t module = lzt::create_module(
      lzt::zeDevice::get_instance()->get_device(), "cmdlist_add.spv",
      ZE_MODULE_FORMAT_IL_SPIRV, nullptr, nullptr);
  ze_kernel_handle_t kernel =
      lzt::create_function(module, "cmdlist_add_constant");
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
  lzt::destroy_function(kernel);
  lzt::destroy_module(module);
  lzt::free_memory(buffer);
}

TEST_P(zeImmediateCommandListExecutionTests,
       GivenImmediateCommandListWhenAppendMemoryCopyThenVerifyCopyIsCorrect) {
  const size_t size = 4096;
  std::vector<uint8_t> host_memory1(size), host_memory2(size, 0);
  void *device_memory =
      lzt::allocate_device_memory(lzt::size_in_bytes(host_memory1));

  lzt::write_data_pattern(host_memory1.data(), size, 1);

  // This should execute immediately
  lzt::append_memory_copy(cmdlist_immediate, device_memory, host_memory1.data(),
                          lzt::size_in_bytes(host_memory1), event0);

  if (mode == ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS) {
    zeEventHostSynchronize(event0, timeout);
    zeEventReset(event0);
  }
  lzt::append_memory_copy(cmdlist_immediate, host_memory2.data(), device_memory,
                          lzt::size_in_bytes(host_memory2), event0);

  if (mode == ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS) {
    zeEventHostSynchronize(event0, timeout);
  }
  lzt::validate_data_pattern(host_memory2.data(), size, 1);
  lzt::free_memory(device_memory);
}

TEST_P(zeImmediateCommandListExecutionTests,
       GivenImmediateCommandListWhenAppendImageCopyThenVerifyCopyIsCorrect) {

  lzt::zeImageCreateCommon img;
  // dest_host_image_upper is used to validate that the above image copy
  // operation(s) were correct:
  lzt::ImagePNG32Bit dest_host_image_upper(img.dflt_host_image_.width(),
                                           img.dflt_host_image_.height());
  // Scribble a known incorrect data pattern to dest_host_image_upper to
  // ensure we are validating actual data from the L0 functionality:
  lzt::write_image_data_pattern(dest_host_image_upper, -1);

  // First, copy the image from the host to the device:
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendImageCopyFromMemory(
                cmdlist_immediate, img.dflt_device_image_2_,
                img.dflt_host_image_.raw_data(), nullptr, event0));
  if (mode == ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS) {
    zeEventHostSynchronize(event0, timeout);
    zeEventReset(event0);
  }
  // Now, copy the image from the device to the device:
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopy(
                                   cmdlist_immediate, img.dflt_device_image_,
                                   img.dflt_device_image_2_, event0));

  if (mode == ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS) {
    zeEventHostSynchronize(event0, timeout);
    zeEventReset(event0);
  }
  // Finally copy the image from the device to the dest_host_image_upper for
  // validation:
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendImageCopyToMemory(
                cmdlist_immediate, dest_host_image_upper.raw_data(),
                img.dflt_device_image_, nullptr, event0));

  if (mode == ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS) {
    zeEventHostSynchronize(event0, timeout);
    zeEventReset(event0);
  }

  // Validate the result of the above operations:
  // If the operation is a straight image copy, or the second region is null
  // then the result should be the same:
  EXPECT_EQ(0, compare_data_pattern(dest_host_image_upper, img.dflt_host_image_,
                                    0, 0, img.dflt_host_image_.width(),
                                    img.dflt_host_image_.height(), 0, 0,
                                    img.dflt_host_image_.width(),
                                    img.dflt_host_image_.height()));
}

TEST_P(
    zeImmediateCommandListExecutionTests,
    GivenImmediateCommandListWhenAppendEventResetThenSuccesIsReturnedAndEventIsReset) {
  ze_event_handle_t event1 = nullptr;
  ep.create_event(event1, ZE_EVENT_SCOPE_FLAG_HOST, ZE_EVENT_SCOPE_FLAG_HOST);
  zeEventHostSignal(event0);
  ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventQueryStatus(event0));
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendEventReset(cmdlist_immediate, event0));
  zeCommandListAppendBarrier(cmdlist_immediate, event1, 0, nullptr);
  if (mode == ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS)
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventHostSynchronize(event1, timeout));
  EXPECT_EQ(ZE_RESULT_NOT_READY, zeEventQueryStatus(event0))
      << "Failure investigated in LOKI-905";
  ep.destroy_event(event1);
}

TEST_P(
    zeImmediateCommandListExecutionTests,
    GivenImmediateCommandListWhenAppendSignalEventThenSuccessIsReturnedAndEventIsSignaled) {
  ze_event_handle_t event1 = nullptr;
  ep.create_event(event1, ZE_EVENT_SCOPE_FLAG_HOST, ZE_EVENT_SCOPE_FLAG_HOST);
  ASSERT_EQ(ZE_RESULT_NOT_READY, zeEventQueryStatus(event0));
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendSignalEvent(cmdlist_immediate, event0));
  zeCommandListAppendBarrier(cmdlist_immediate, event1, 0, nullptr);
  if (mode == ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS)
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventHostSynchronize(event1, timeout));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventHostSynchronize(event0, timeout))
      << "Failure investigated in LOKI-905";
  ep.destroy_event(event1);
}

TEST_P(zeImmediateCommandListExecutionTests,
       GivenImmediateCommandListWhenAppendWaitOnEventsThenSuccessIsReturned) {
  if (mode != ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS)
    return;
  ze_event_handle_t event1 = nullptr;
  ep.create_event(event1, ZE_EVENT_SCOPE_FLAG_HOST, ZE_EVENT_SCOPE_FLAG_NONE);
  EXPECT_EQ(ZE_RESULT_NOT_READY, zeEventQueryStatus(event1));
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendWaitOnEvents(cmdlist_immediate, 1, &event0));
  zeCommandListAppendBarrier(cmdlist_immediate, event1, 0, nullptr);
  zeEventHostSignal(event0);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventHostSynchronize(event0, timeout));
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventHostSynchronize(event1, timeout));
  ep.destroy_event(event1);
}

TEST_P(zeImmediateCommandListExecutionTests,
       GivenImmediateCommandListWhenAppendMemoryPrefetchThenSuccessIsReturned) {
  size_t size = 4096;
  void *memory = lzt::allocate_host_memory(size);
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendMemoryPrefetch(cmdlist_immediate, memory, size));
  lzt::free_memory(memory);
}

TEST_P(zeImmediateCommandListExecutionTests,
       GivenImmediateCommandListWhenAppendMemAdviseThenSuccessIsReturned) {
  size_t size = 4096;
  void *memory = lzt::allocate_host_memory(size);
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendMemAdvise(
                cmdlist_immediate, lzt::zeDevice::get_instance()->get_device(),
                memory, size, ZE_MEMORY_ADVICE_SET_READ_MOSTLY));
  lzt::free_memory(memory);
}

INSTANTIATE_TEST_CASE_P(TestCasesforCommandListImmediateCases,
                        zeImmediateCommandListExecutionTests,
                        testing::Values(ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                        ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS,
                                        ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS));

} // namespace
