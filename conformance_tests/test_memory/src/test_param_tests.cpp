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

enum memory_test_type { MTT_SHARED, MTT_HOST };

class xeMemAccessTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<enum memory_test_type> {

protected:
  xeMemAccessTests() { memory_ = nullptr; }
  ~xeMemAccessTests() { lzt::free_memory(memory_); }
  void Init(memory_test_type mtt) {
    mtt_ = mtt;
    if (mtt == MTT_HOST)
      memory_ = lzt::allocate_host_memory(size_);
    else
      memory_ = lzt::allocate_shared_memory(size_);
  };
  const size_t size_ = 4 * 1024;
  void *memory_ = nullptr;
  memory_test_type mtt_;
};

TEST_P(xeMemAccessTests,
       GivenMemAllocationWhenWritingAndReadingBackOnHostThenCorrectDataIsRead) {
  Init(GetParam());
  lzt::write_data_pattern(memory_, size_, 1);
  lzt::validate_data_pattern(memory_, size_, 1);
}

class xeMemAccessCommandListTests : public xeMemAccessTests {
protected:
  lzt::zeCommandList cmdlist_;
  lzt::zeCommandQueue cmdqueue_;
};

TEST_P(
    xeMemAccessCommandListTests,
    GivenMemoryAllocationWhenCopyingAndReadingBackOnHostThenCorrectDataIsRead) {
  Init(GetParam());
  lzt::write_data_pattern(memory_, size_, 1);
  void *other_memory = (mtt_ == MTT_HOST) ? lzt::allocate_host_memory(size_)
                                          : lzt::allocate_shared_memory(size_);

  lzt::append_memory_copy(cmdlist_.command_list_, other_memory, memory_, size_,
                          nullptr);
  lzt::append_barrier(cmdlist_.command_list_, nullptr, 0, nullptr);
  lzt::close_command_list(cmdlist_.command_list_);
  lzt::execute_command_lists(cmdqueue_.command_queue_, 1,
                             &cmdlist_.command_list_, nullptr);
  lzt::synchronize(cmdqueue_.command_queue_, UINT32_MAX);
  lzt::validate_data_pattern(other_memory, size_, 1);
  lzt::free_memory(other_memory);
}

TEST_P(xeMemAccessCommandListTests,
       GivenAllocationSettingAndReadingBackOnHostThenCorrectDataIsRead) {
  const int value = 0x55;
  Init(GetParam());
  memset(memory_, 0,
         size_); // Write a different pattern from what we are going to write.
  lzt::append_memory_set(cmdlist_.command_list_, memory_, value, size_);
  lzt::append_barrier(cmdlist_.command_list_, nullptr, 0, nullptr);
  lzt::close_command_list(cmdlist_.command_list_);
  lzt::execute_command_lists(cmdqueue_.command_queue_, 1,
                             &cmdlist_.command_list_, nullptr);
  lzt::synchronize(cmdqueue_.command_queue_, UINT32_MAX);
  for (unsigned int ui = 0; ui < size_; ui++)
    EXPECT_EQ(value, static_cast<uint8_t *>(memory_)[ui]);
}

TEST_P(xeMemAccessTests,
       GivenAllocationWhenWritingAndReadingBackOnDeviceThenCorrectDataIsRead) {
  Init(GetParam());
  lzt::write_data_pattern(memory_, size_, 1);
  std::string module_name = "unified_mem_test.spv";
  ze_module_handle_t module = lzt::create_module(
      lzt::zeDevice::get_instance()->get_device(), module_name);
  std::string func_name = "unified_mem_test";

  lzt::FunctionArg arg;
  std::vector<lzt::FunctionArg> args;

  arg.arg_size = sizeof(void *);
  arg.arg_value = &memory_;
  args.push_back(arg);
  arg.arg_size = sizeof(int);
  int size = static_cast<int>(size_);
  arg.arg_value = &size;
  args.push_back(arg);
  lzt::create_and_execute_function(lzt::zeDevice::get_instance()->get_device(),
                                   module, func_name, 1, args);
  lzt::validate_data_pattern(memory_, size_, -1);
  lzt::destroy_module(module);
}

INSTANTIATE_TEST_CASE_P(xeMemAccessTests, xeMemAccessTests,
                        testing::Values(MTT_HOST, MTT_SHARED));
INSTANTIATE_TEST_CASE_P(xeMemAccessCommandListTests,
                        xeMemAccessCommandListTests,
                        testing::Values(MTT_HOST, MTT_SHARED));
INSTANTIATE_TEST_CASE_P(xeMemAccessCommandListTests_1,
                        xeMemAccessCommandListTests,
                        testing::Values(MTT_HOST, MTT_SHARED));
INSTANTIATE_TEST_CASE_P(xeMemAccessDeviceTests, xeMemAccessTests,
                        testing::Values(MTT_HOST, MTT_SHARED));

} // namespace
