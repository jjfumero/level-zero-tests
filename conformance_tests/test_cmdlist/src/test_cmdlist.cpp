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

class zeCommandListCreateTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<ze_command_list_flag_t> {};

TEST_P(
    zeCommandListCreateTests,
    GivenValidDeviceAndCommandListDescriptorWhenCreatingCommandListThenNotNullCommandListIsReturned) {
  ze_command_list_desc_t descriptor;
  descriptor.version = ZE_COMMAND_LIST_DESC_VERSION_CURRENT;
  descriptor.flags = GetParam();

  ze_command_list_handle_t command_list = nullptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListCreate(lzt::zeDevice::get_instance()->get_device(),
                                &descriptor, &command_list));
  EXPECT_NE(nullptr, command_list);

  lzt::destroy_command_list(command_list);
}

INSTANTIATE_TEST_CASE_P(
    CreateFlagParameterizedTest, zeCommandListCreateTests,
    ::testing::Values(ZE_COMMAND_LIST_FLAG_NONE, ZE_COMMAND_LIST_FLAG_COPY_ONLY,
                      ZE_COMMAND_LIST_FLAG_RELAXED_ORDERING));

class zeCommandListDestroyTests : public ::testing::Test {};

TEST_F(
    zeCommandListDestroyTests,
    GivenValidDeviceAndCommandListDescriptorWhenDestroyingCommandListThenSuccessIsReturned) {
  ze_command_list_desc_t descriptor;
  descriptor.version = ZE_COMMAND_LIST_DESC_VERSION_CURRENT;

  ze_command_list_handle_t command_list = nullptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListCreate(lzt::zeDevice::get_instance()->get_device(),
                                &descriptor, &command_list));
  EXPECT_NE(nullptr, command_list);

  lzt::destroy_command_list(command_list);
}

// zeCommandListCreateImmediateTests currently fail with
// 'ZE_RESULT_ERROR_UNSUPPORTED' LOKI-289 is open for the implementation of the
// API
class zeCommandListCreateImmediateTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<
          std::tuple<ze_command_queue_flag_t, ze_command_queue_mode_t,
                     ze_command_queue_priority_t>> {};

TEST_P(zeCommandListCreateImmediateTests,
       GivenImplicitCommandQueueWhenCreatingCommandListThenSuccessIsReturned) {

  ze_command_queue_desc_t descriptor = {
      ZE_COMMAND_QUEUE_DESC_VERSION_CURRENT, // version
      std::get<0>(GetParam()),               // flags
      std::get<1>(GetParam()),               // mode
      std::get<2>(GetParam())                // priority
  };

  ze_command_list_handle_t command_list = nullptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListCreateImmediate(
                                   lzt::zeDevice::get_instance()->get_device(),
                                   &descriptor, &command_list));

  lzt::destroy_command_list(command_list);
}

INSTANTIATE_TEST_CASE_P(
    ImplictCommandQueueParameterizedTest, zeCommandListCreateImmediateTests,
    ::testing::Combine(
        ::testing::Values(ZE_COMMAND_QUEUE_FLAG_NONE,
                          ZE_COMMAND_QUEUE_FLAG_COPY_ONLY,
                          ZE_COMMAND_QUEUE_FLAG_LOGICAL_ONLY,
                          ZE_COMMAND_QUEUE_FLAG_SINGLE_SLICE_ONLY),
        ::testing::Values(ZE_COMMAND_QUEUE_MODE_DEFAULT,
                          ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS,
                          ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS),
        ::testing::Values(ZE_COMMAND_QUEUE_PRIORITY_NORMAL,
                          ZE_COMMAND_QUEUE_PRIORITY_LOW,
                          ZE_COMMAND_QUEUE_PRIORITY_HIGH)));

class zeCommandListCloseTests : public lzt::zeCommandListTests {};

TEST_F(zeCommandListCloseTests,
       GivenEmptyCommandListWhenClosingCommandListThenSuccessIsReturned) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(cl.command_list_));
}

class zeCommandListResetTests : public lzt::zeCommandListTests {};

TEST_F(zeCommandListResetTests,
       GivenEmptyCommandListWhenResettingCommandListThenSuccessIsReturned) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListReset(cl.command_list_));
}

class zeCommandListReuseTests : public ::testing::Test {};

TEST(zeCommandListReuseTests, GivenCommandListWhenItIsExecutedItCanBeRunAgain) {
  auto cmdlist = lzt::create_command_list();
  auto cmdq = lzt::create_command_queue();
  const size_t size = 16;
  auto buffer = lzt::allocate_shared_memory(size);

  lzt::append_memory_set(cmdlist, buffer, 0x1, size);
  lzt::close_command_list(cmdlist);

  const int num_execute = 5;
  for (int i = 0; i < num_execute; i++) {
    memset(buffer, 0x0, size);
    for (int j = 0; j < size; j++)
      ASSERT_EQ(static_cast<uint8_t *>(buffer)[j], 0x0)
          << "Memory Set did not match.";

    lzt::execute_command_lists(cmdq, 1, &cmdlist, nullptr);
    lzt::synchronize(cmdq, UINT32_MAX);
    for (int j = 0; j < size; j++)
      ASSERT_EQ(static_cast<uint8_t *>(buffer)[j], 0x1)
          << "Memory Set did not match.";
  }

  lzt::destroy_command_list(cmdlist);
  lzt::destroy_command_queue(cmdq);
  lzt::free_memory(buffer);
}

class zeCommandListCloseAndResetTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<ze_command_list_flag_t> {};

TEST_P(zeCommandListCloseAndResetTests,
       GivenClosedCommandListWhenAppendAttemptedThenNotSuccessful) {
  ze_command_list_flag_t flags = GetParam();
  ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  ze_command_list_handle_t cmdlist = lzt::create_command_list(device, flags);
  ze_command_queue_handle_t cmdq;
  if (flags == ZE_COMMAND_LIST_FLAG_COPY_ONLY) {
    cmdq = lzt::create_command_queue(device, ZE_COMMAND_QUEUE_FLAG_COPY_ONLY,
                                     ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                     ZE_COMMAND_QUEUE_PRIORITY_NORMAL, 0);
  } else {
    cmdq = lzt::create_command_queue(device);
  }
  const size_t size = 16;
  auto buffer = lzt::allocate_shared_memory(size);
  const uint8_t set_succeed_1 = 0x1;
  const uint8_t set_fail_2 = 0x2;
  const uint8_t set_fail_3 = 0x3;

  memset(buffer, 0x0, size);
  for (size_t j = 0; j < size; j++) {
    EXPECT_EQ(static_cast<uint8_t *>(buffer)[j], 0x0);
  }
  // Command list setup with only memory set and barrier
  lzt::append_memory_set(cmdlist, buffer, set_succeed_1, size);
  lzt::append_barrier(cmdlist, nullptr, 0, nullptr);
  lzt::close_command_list(cmdlist);
  // Attempt to append command list after close should fail
  lzt::append_memory_set(cmdlist, buffer, set_fail_2, size);
  lzt::execute_command_lists(cmdq, 1, &cmdlist, nullptr);
  lzt::synchronize(cmdq, UINT32_MAX);
  for (size_t j = 0; j < size; j++) {
    EXPECT_EQ(static_cast<uint8_t *>(buffer)[j], set_succeed_1);
  }

  memset(buffer, 0x0, size);
  for (size_t j = 0; j < size; j++) {
    EXPECT_EQ(static_cast<uint8_t *>(buffer)[j], 0x0);
  }
  // Reset command list and immediately close
  lzt::reset_command_list(cmdlist);
  lzt::close_command_list(cmdlist);
  // Attempt to append command list after close should fail
  lzt::append_memory_set(cmdlist, buffer, set_fail_3, size);
  lzt::execute_command_lists(cmdq, 1, &cmdlist, nullptr);
  lzt::synchronize(cmdq, UINT32_MAX);
  // No commands should be executed by command queue
  // This test fails for fulsim/cobalt, and passes for Gen9HW
  // (It will NOT fail, if the append_memory_set, 5 lines above, is commented
  // out)
  for (size_t j = 0; j < size; j++) {
    EXPECT_EQ(static_cast<uint8_t *>(buffer)[j], 0x0);
  }
  lzt::destroy_command_list(cmdlist);
  lzt::destroy_command_queue(cmdq);
  lzt::free_memory(buffer);
}

TEST_P(zeCommandListCloseAndResetTests,
       GivenCommandListWhenResetThenVerifyOnlySubsequentInstructionsExecuted) {
  ze_command_list_flag_t flags = GetParam();
  ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  ze_command_list_handle_t cmdlist = lzt::create_command_list(device, flags);
  ze_command_queue_handle_t cmdq;
  if (flags == ZE_COMMAND_LIST_FLAG_COPY_ONLY) {
    cmdq = lzt::create_command_queue(device, ZE_COMMAND_QUEUE_FLAG_COPY_ONLY,
                                     ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                     ZE_COMMAND_QUEUE_PRIORITY_NORMAL, 0);
  } else {
    cmdq = lzt::create_command_queue(device);
  }
  const size_t num_instr = 8;
  const size_t size = 16;

  std::vector<void *> buffer;
  std::vector<uint8_t> val;
  for (size_t i = 0; i < num_instr; i++) {
    buffer.push_back(lzt::allocate_shared_memory(size));
    val.push_back(static_cast<uint8_t>(i + 1));
  }

  // Begin with num_instr command list instructions, reset and reduce by one
  // each time.
  size_t test_instr = num_instr;

  while (test_instr) {
    for (auto buf : buffer) {
      memset(buf, 0x0, size);
    }
    for (size_t i = 0; i < test_instr; i++) {
      lzt::append_memory_set(cmdlist, buffer[i], val[test_instr - (i + 1)],
                             size);
    }
    lzt::append_barrier(cmdlist, nullptr, 0, nullptr);
    lzt::close_command_list(cmdlist);
    lzt::execute_command_lists(cmdq, 1, &cmdlist, nullptr);
    lzt::synchronize(cmdq, UINT32_MAX);
    for (size_t i = 0; i < test_instr; i++) {
      for (size_t j = 0; j < size; j++) {
        EXPECT_EQ(static_cast<uint8_t *>(buffer[i])[j],
                  val[test_instr - (i + 1)]);
      }
    }
    for (size_t i = test_instr; i < num_instr; i++) {
      for (size_t j = 0; j < size; j++) {
        EXPECT_EQ(static_cast<uint8_t *>(buffer[i])[j], 0x0);
      }
    }
    lzt::reset_command_list(cmdlist);
    test_instr--;
  }
  // Last check:  no instructions should be executed
  for (auto buf : buffer) {
    memset(buf, 0x0, size);
  }
  lzt::close_command_list(cmdlist);
  lzt::execute_command_lists(cmdq, 1, &cmdlist, nullptr);
  lzt::synchronize(cmdq, UINT32_MAX);
  for (size_t i = 0; i < num_instr; i++) {
    for (size_t j = 0; j < size; j++) {
      EXPECT_EQ(static_cast<uint8_t *>(buffer[i])[j], 0x0);
    }
  }

  // Command list setup with only memory set and barrier

  lzt::destroy_command_list(cmdlist);
  lzt::destroy_command_queue(cmdq);
  for (auto buf : buffer) {
    lzt::free_memory(buf);
  }
}

INSTANTIATE_TEST_CASE_P(
    TestCasesforCommandListCloseAndCommandListReset,
    zeCommandListCloseAndResetTests,
    testing::Values(ZE_COMMAND_LIST_FLAG_NONE, ZE_COMMAND_LIST_FLAG_COPY_ONLY,
                    ZE_COMMAND_LIST_FLAG_RELAXED_ORDERING,
                    ZE_COMMAND_LIST_FLAG_MAXIMIZE_THROUGHPUT,
                    ZE_COMMAND_LIST_FLAG_EXPLICIT_ONLY));

} // namespace
