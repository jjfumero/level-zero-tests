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

class xeHostMemAccessTests : public ::testing::Test {
protected:
  xeHostMemAccessTests() { memory_ = lzt::allocate_host_memory(size_); };
  ~xeHostMemAccessTests() { lzt::free_memory(memory_); }
  const size_t size_ = 4 * 1024;
  void *memory_ = nullptr;
};

class xeHostMemAccessHostTests : public xeHostMemAccessTests {};

TEST_F(
    xeHostMemAccessHostTests,
    GivenHostAllocationWhenWritingAndReadingBackOnHostThenCorrectDataIsRead) {
  lzt::write_data_pattern(memory_, size_, 1);
  lzt::validate_data_pattern(memory_, size_, 1);
}

class xeHostMemAccessCommandListTests : public xeHostMemAccessTests {
protected:
  lzt::xeCommandList cmdlist_;
  lzt::xeCommandQueue cmdqueue_;
};

TEST_F(
    xeHostMemAccessCommandListTests,
    GivenHostAllocationWhenCopyingAndReadingBackOnHostThenCorrectDataIsRead) {
  lzt::write_data_pattern(memory_, size_, 1);
  void *other_host_memory = lzt::allocate_host_memory(size_);

  lzt::append_memory_copy(cmdlist_.command_list_, other_host_memory, memory_,
                          size_, nullptr, 0, nullptr);
  lzt::append_barrier(cmdlist_.command_list_, nullptr, 0, nullptr);
  lzt::close_command_list(cmdlist_.command_list_);
  lzt::execute_command_lists(cmdqueue_.command_queue_, 1,
                             &cmdlist_.command_list_, nullptr);
  lzt::synchronize(cmdqueue_.command_queue_, UINT32_MAX);
  lzt::validate_data_pattern(other_host_memory, size_, 1);
  lzt::free_memory(other_host_memory);
}

TEST_F(xeHostMemAccessCommandListTests,
       GivenHostAllocationSettingAndReadingBackOnHostThenCorrectDataIsRead) {
  const int value = 0x55;

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

class xeHostMemAccessDeviceTests : public xeHostMemAccessTests {};

TEST_F(
    xeHostMemAccessDeviceTests,
    GivenHostAllocationWhenWritingAndReadingBackOnDeviceThenCorrectDataIsRead) {
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

class xeHostSystemMemoryHostTests : public ::testing::Test {
protected:
  xeHostSystemMemoryHostTests() { memory_ = new uint8_t[size_]; }
  ~xeHostSystemMemoryHostTests() { delete[] memory_; }
  const size_t size_ = 4 * 1024;
  uint8_t *memory_ = nullptr;
};

TEST_F(
    xeHostSystemMemoryHostTests,
    GivenHostSystemAllocationWhenWritingAndReadingOnHostThenCorrectDataIsRead) {
  lzt::write_data_pattern(memory_, size_, 1);
  lzt::validate_data_pattern(memory_, size_, 1);
}

class xeHostSystemMemoryDeviceTests : public ::testing::Test {
protected:
  xeHostSystemMemoryDeviceTests() { memory_ = new uint8_t[size_]; }
  ~xeHostSystemMemoryDeviceTests() { delete[] memory_; }
  const size_t size_ = 4 * 1024;
  uint8_t *memory_ = nullptr;
  lzt::xeCommandList cmdlist_;
  lzt::xeCommandQueue cmdqueue_;
};

TEST_F(
    xeHostSystemMemoryDeviceTests,
    GivenHostSystemAllocationWhenAccessingMemoryOnDeviceThenCorrectDataIsRead) {
  // FIXME: LOKI-488
  FAIL()
      << "Fail due to Abort when accessing system memory allocation: LOKI-488";
  lzt::write_data_pattern(memory_, size_, 1);
  std::string module_name = "xe_unified_mem_test.spv";
  xe_module_handle_t module = lzt::create_module(
      lzt::xeDevice::get_instance()->get_device(), module_name);
  std::string func_name = "xe_unified_mem_test";

  lzt::FunctionArg arg;
  std::vector<lzt::FunctionArg> args;

  arg.arg_size = sizeof(uint8_t *);
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

TEST_F(
    xeHostSystemMemoryDeviceTests,
    GivenHostSystemAllocationWhenCopyingMemoryOnDeviceThenMemoryCopiedCorrectly) {
  lzt::write_data_pattern(memory_, size_, 1);
  uint8_t *other_system_memory = new uint8_t[size_];

  lzt::append_memory_copy(cmdlist_.command_list_, other_system_memory, memory_,
                          size_, nullptr, 0, nullptr);
  lzt::append_barrier(cmdlist_.command_list_, nullptr, 0, nullptr);
  lzt::close_command_list(cmdlist_.command_list_);
  lzt::execute_command_lists(cmdqueue_.command_queue_, 1,
                             &cmdlist_.command_list_, nullptr);
  lzt::synchronize(cmdqueue_.command_queue_, UINT32_MAX);
  lzt::validate_data_pattern(other_system_memory, size_, 1);
  delete[] other_system_memory;
}

TEST_F(xeHostSystemMemoryDeviceTests,
       GivenHostSystemMemoryWhenSettingMemoryOnDeviceThenMemorySetCorrectly) {
  // This test case fails due to the memory not being set: LOKI-490
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

} // namespace
