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

#include "xe_driver.h"
#include "xe_memory.h"
#include "xe_copy.h"

namespace {

class xeSharedMemAccessTests : public ::testing::Test {
protected:
  xeSharedMemAccessTests() { memory_ = lzt::allocate_shared_memory(size_); };
  ~xeSharedMemAccessTests() { lzt::free_memory(memory_); }
  const size_t size_ = 4 * 1024;
  void *memory_ = nullptr;
};

class xeSharedMemAccessHostTests : public xeSharedMemAccessTests {};

TEST_F(
    xeSharedMemAccessHostTests,
    GivenSharedAllocationWhenWritingAndReadingBackOnHostThenCorrectDataIsRead) {
  lzt::write_data_pattern(memory_, size_, 1);
  lzt::validate_data_pattern(memory_, size_, 1);
}

class xeSharedMemAccessCommandListTests : public xeSharedMemAccessTests {
protected:
  lzt::xeCommandList cmdlist_;
  lzt::xeCommandQueue cmdqueue_;
};

TEST_F(
    xeSharedMemAccessCommandListTests,
    GivenSharedAllocationWhenCopyingAndReadingBackOnHostThenCorrectDataIsRead) {
  lzt::write_data_pattern(memory_, size_, 1);
  void *other_shared_memory = lzt::allocate_shared_memory(size_);

  lzt::append_memory_copy(cmdlist_.command_list_, other_shared_memory, memory_,
                          size_, nullptr, 0, nullptr);
  lzt::append_barrier(cmdlist_.command_list_, nullptr, 0, nullptr);
  lzt::close_command_list(cmdlist_.command_list_);
  lzt::execute_command_lists(cmdqueue_.command_queue_, 1,
                             &cmdlist_.command_list_, nullptr);
  lzt::synchronize(cmdqueue_.command_queue_, UINT32_MAX);
  lzt::validate_data_pattern(other_shared_memory, size_, 1);
  lzt::free_memory(other_shared_memory);
}

TEST_F(xeSharedMemAccessCommandListTests,
       GivenSharedAllocationSettingAndReadingBackOnHostThenCorrectDataIsRead) {
  const int value = 0x55;
  lzt::write_data_pattern(memory_, size_, 1);
  lzt::append_memory_set(cmdlist_.command_list_, memory_, value, size_);
  lzt::append_barrier(cmdlist_.command_list_, nullptr, 0, nullptr);
  lzt::close_command_list(cmdlist_.command_list_);
  lzt::execute_command_lists(cmdqueue_.command_queue_, 1,
                             &cmdlist_.command_list_, nullptr);
  lzt::synchronize(cmdqueue_.command_queue_, UINT32_MAX);
  for (unsigned int ui = 0; ui < size_; ui++)
    EXPECT_EQ(value, static_cast<uint8_t *>(memory_)[ui]);
}

class xeSharedMemAccessDeviceTests : public xeSharedMemAccessTests {};

TEST_F(
    xeSharedMemAccessDeviceTests,
    GivenSharedAllocationWhenWritingAndReadingBackOnDeviceThenCorrectDataIsRead) {
  lzt::write_data_pattern(memory_, size_, 1);
  std::string module_name = "xe_unified_mem_test.spv";
  xe_module_handle_t module = lzt::create_module(
      lzt::xeDevice::get_instance()->get_device(), module_name);
  std::string func_name = "xe_unified_mem_test";

  lzt::FunctionArg arg;
  std::vector<lzt::FunctionArg> args;

  arg.arg_size = sizeof(void *);
  arg.arg_value = &memory_;
  args.push_back(arg);
  arg.arg_size = sizeof(int);
  int size = static_cast<int>(size_);
  arg.arg_value = &size;
  args.push_back(arg);
  lzt::create_and_execute_function(lzt::xeDevice::get_instance()->get_device(),
                                   module, func_name, 1, args);
  lzt::validate_data_pattern(memory_, size_, -1);
  lzt::destroy_module(module);
}

} // namespace
