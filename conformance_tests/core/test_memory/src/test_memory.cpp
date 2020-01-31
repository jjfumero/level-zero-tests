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

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <chrono>
#include <thread>

#include "gtest/gtest.h"

#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"
#include "logging/logging.hpp"

namespace lzt = level_zero_tests;

#include "ze_api.h"

namespace {

class zeDriverAllocDeviceMemTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<
          std::tuple<ze_device_mem_alloc_flag_t, size_t, size_t>> {
protected:
  void SetUp() override {
    const ze_device_mem_alloc_flag_t flags = std::get<0>(GetParam());
    size_ = std::get<1>(GetParam());
    const size_t alignment = std::get<2>(GetParam());
    memory_ = lzt::allocate_device_memory(size_, alignment, flags);
  }
  void TearDown() override { lzt::free_memory(memory_); }
  size_t size_;
  void *memory_ = nullptr;
};

class zeDriverAllocDeviceMemParamsTests : public zeDriverAllocDeviceMemTests {};

TEST_P(
    zeDriverAllocDeviceMemParamsTests,
    GivenAllocationFlagsAndSizeAndAlignmentWhenAllocatingDeviceMemoryThenNotNullPointerIsReturned) {
}

INSTANTIATE_TEST_CASE_P(
    zeDriverAllocDeviceMemTestVaryFlagsAndSizeAndAlignment,
    zeDriverAllocDeviceMemParamsTests,
    ::testing::Combine(
        ::testing::Values(ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
                          ZE_DEVICE_MEM_ALLOC_FLAG_BIAS_CACHED,
                          ZE_DEVICE_MEM_ALLOC_FLAG_BIAS_UNCACHED),
        lzt::memory_allocation_sizes, lzt::memory_allocation_alignments));

class zeDriverGetMemAllocPropertiesTests : public zeDriverAllocDeviceMemTests {
};

TEST_P(
    zeDriverGetMemAllocPropertiesTests,
    GivenValidDeviceMemoryPointerWhenGettingPropertiesThenVersionAndTypeReturned) {
  ze_memory_allocation_properties_t memory_properties;
  memory_properties.version = ZE_MEMORY_ALLOCATION_PROPERTIES_VERSION_CURRENT;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverGetMemAllocProperties(lzt::get_default_driver(), memory_,
                                          &memory_properties, nullptr));

  EXPECT_EQ(ZE_MEMORY_ALLOCATION_PROPERTIES_VERSION_CURRENT,
            memory_properties.version);
  EXPECT_EQ(ZE_MEMORY_TYPE_DEVICE, memory_properties.type);

  if (size_ > 0) {
    uint8_t *char_mem = static_cast<uint8_t *>(memory_);
    memory_properties.version = ZE_MEMORY_ALLOCATION_PROPERTIES_VERSION_CURRENT;
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeDriverGetMemAllocProperties(
                                     lzt::get_default_driver(),
                                     static_cast<void *>(char_mem + size_ - 1),
                                     &memory_properties, nullptr));
    EXPECT_EQ(ZE_MEMORY_ALLOCATION_PROPERTIES_VERSION_CURRENT,
              memory_properties.version);
    EXPECT_EQ(ZE_MEMORY_TYPE_DEVICE, memory_properties.type);
  }
}

INSTANTIATE_TEST_CASE_P(
    zeDriverGetMemAllocPropertiesTestVaryFlagsAndSizeAndAlignment,
    zeDriverGetMemAllocPropertiesTests,
    ::testing::Combine(::testing::Values(ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT),
                       lzt::memory_allocation_sizes,
                       lzt::memory_allocation_alignments));

class zeDriverMemGetAddressRangeTests : public zeDriverAllocDeviceMemTests {};

TEST_P(
    zeDriverMemGetAddressRangeTests,
    GivenValidDeviceMemoryPointerWhenGettingAddressRangeThenBaseAddressAndSizeReturned) {

  void *pBase = nullptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverGetMemAddressRange(lzt::get_default_driver(), memory_,
                                       &pBase, NULL));
  EXPECT_EQ(pBase, memory_);
  size_t addr_range_size = 0;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverGetMemAddressRange(lzt::get_default_driver(), memory_, NULL,
                                       &addr_range_size));

  // Get device mem size rounds size up to nearest page size
  EXPECT_GE(addr_range_size, size_);
  pBase = nullptr;
  addr_range_size = 0;
  if (size_ > 0) {
    uint8_t *char_mem = static_cast<uint8_t *>(memory_);
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeDriverGetMemAddressRange(
                                     lzt::get_default_driver(),
                                     static_cast<void *>(char_mem + size_ - 1),
                                     &pBase, &addr_range_size));
  } else {
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeDriverGetMemAddressRange(lzt::get_default_driver(), memory_,
                                         &pBase, &addr_range_size));
  }
  EXPECT_EQ(pBase, memory_);
  // Get device mem size rounds size up to nearest page size
  EXPECT_GE(addr_range_size, size_);
}

INSTANTIATE_TEST_CASE_P(
    zeDriverMemGetAddressRangeTestVaryFlagsAndSizeAndAlignment,
    zeDriverMemGetAddressRangeTests,
    ::testing::Combine(::testing::Values(ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT),
                       lzt::memory_allocation_sizes,
                       lzt::memory_allocation_alignments));

class zeDriverMemFreeTests : public ::testing::Test {};

TEST_F(
    zeDriverMemFreeTests,
    GivenValidDeviceMemAllocationWhenFreeingDeviceMemoryThenSuccessIsReturned) {
  void *memory = lzt::allocate_device_memory(1);
  lzt::free_memory(memory);
}

class zeDriverAllocSharedMemTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<
          std::tuple<ze_device_mem_alloc_flag_t, ze_host_mem_alloc_flag_t,
                     size_t, size_t>> {};
TEST_P(
    zeDriverAllocSharedMemTests,
    GivenAllocationFlagsSizeAndAlignmentWhenAllocatingSharedMemoryThenNotNullPointerIsReturned) {
  const ze_device_mem_alloc_flag_t dev_flags = std::get<0>(GetParam());
  const ze_host_mem_alloc_flag_t host_flags = std::get<1>(GetParam());
  const size_t size = std::get<2>(GetParam());
  const size_t alignment = std::get<3>(GetParam());

  void *memory = nullptr;
  ze_device_mem_alloc_desc_t device_desc;
  device_desc.ordinal = 1;
  device_desc.flags = dev_flags;
  ze_host_mem_alloc_desc_t host_desc;
  host_desc.flags = host_flags;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverAllocSharedMem(lzt::get_default_driver(), &device_desc,
                                   &host_desc, size, alignment,
                                   lzt::zeDevice::get_instance()->get_device(),
                                   &memory));
  EXPECT_NE(nullptr, memory);

  lzt::free_memory(memory);
}

INSTANTIATE_TEST_CASE_P(
    TestSharedMemFlagPermutations, zeDriverAllocSharedMemTests,
    ::testing::Combine(
        ::testing::Values(ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
                          ZE_DEVICE_MEM_ALLOC_FLAG_BIAS_CACHED,
                          ZE_DEVICE_MEM_ALLOC_FLAG_BIAS_UNCACHED),
        ::testing::Values(ZE_HOST_MEM_ALLOC_FLAG_DEFAULT,
                          ZE_HOST_MEM_ALLOC_FLAG_BIAS_CACHED,
                          ZE_HOST_MEM_ALLOC_FLAG_BIAS_UNCACHED,
                          ZE_HOST_MEM_ALLOC_FLAG_BIAS_WRITE_COMBINED),
        lzt::memory_allocation_sizes, lzt::memory_allocation_alignments));

class xeSharedMemGetPropertiesTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<std::tuple<size_t, size_t>> {};

TEST_P(xeSharedMemGetPropertiesTests,
       GivenSharedAllocationWhenGettingMemPropertiesThenSuccessIsReturned) {
  const size_t size = std::get<0>(GetParam());
  const size_t alignment = std::get<1>(GetParam());

  ze_memory_allocation_properties_t mem_properties;
  void *memory = lzt::allocate_shared_memory(size, alignment);

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverGetMemAllocProperties(lzt::get_default_driver(), memory,
                                          &mem_properties, nullptr));

  lzt::free_memory(memory);
}

INSTANTIATE_TEST_CASE_P(TestSharedMemGetPropertiesPermutations,
                        xeSharedMemGetPropertiesTests,
                        ::testing::Combine(lzt::memory_allocation_sizes,
                                           lzt::memory_allocation_alignments));

class xeSharedMemGetAddressRangeTests : public ::testing::Test {};

TEST_F(xeSharedMemGetAddressRangeTests,
       GivenSharedAllocationWhenGettingAddressRangeThenCorrectSizeIsReturned) {
  const size_t size = 1;
  const size_t alignment = 1;

  void *memory = lzt::allocate_shared_memory(size, alignment);
  size_t size_out;

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverGetMemAddressRange(lzt::get_default_driver(), memory, NULL,
                                       &size_out));
  EXPECT_GE(size_out, size);
  lzt::free_memory(memory);
}

TEST_F(xeSharedMemGetAddressRangeTests,
       GivenSharedAllocationWhenGettingAddressRangeThenCorrectBaseIsReturned) {
  const size_t size = 1;
  const size_t alignment = 1;

  void *memory = lzt::allocate_shared_memory(size, alignment);
  void *base = nullptr;

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverGetMemAddressRange(lzt::get_default_driver(), memory, &base,
                                       NULL));
  EXPECT_EQ(base, memory);
  lzt::free_memory(memory);
}

class xeSharedMemGetAddressRangeParameterizedTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<std::tuple<size_t, size_t>> {};

TEST_P(
    xeSharedMemGetAddressRangeParameterizedTests,
    GivenSharedAllocationWhenGettingAddressRangeThenCorrectSizeAndBaseIsReturned) {
  const size_t size = std::get<0>(GetParam());
  const size_t alignment = std::get<1>(GetParam());

  void *memory = lzt::allocate_shared_memory(size, alignment);
  void *base = nullptr;
  size_t size_out;

  // Test getting address info from begining of memory range
  uint8_t *mem_target = static_cast<uint8_t *>(memory);
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverGetMemAddressRange(lzt::get_default_driver(), memory, &base,
                                       &size_out));
  EXPECT_GE(size_out, size);
  EXPECT_EQ(base, memory);

  if (size > 1) {
    // Test getting address info from middle of memory range
    mem_target = static_cast<uint8_t *>(memory) + (size - 1) / 2;
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeDriverGetMemAddressRange(lzt::get_default_driver(), mem_target,
                                         &base, &size_out));
    EXPECT_GE(size_out, size);
    EXPECT_EQ(memory, base);

    // Test getting address info from end of memory range
    mem_target = static_cast<uint8_t *>(memory) + (size - 1);
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeDriverGetMemAddressRange(lzt::get_default_driver(), mem_target,
                                         &base, &size_out));
    EXPECT_GE(size_out, size);
    EXPECT_EQ(memory, base);
  }
  lzt::free_memory(memory);
}

INSTANTIATE_TEST_CASE_P(TestSharedMemGetAddressRangePermutations,
                        xeSharedMemGetAddressRangeParameterizedTests,
                        ::testing::Combine(lzt::memory_allocation_sizes,
                                           lzt::memory_allocation_alignments));

class zeDriverAllocHostMemTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<
          std::tuple<ze_host_mem_alloc_flag_t, size_t, size_t>> {};

TEST_P(
    zeDriverAllocHostMemTests,
    GivenFlagsSizeAndAlignmentWhenAllocatingHostMemoryThenNotNullPointerIsReturned) {

  const ze_host_mem_alloc_flag_t flags = std::get<0>(GetParam());

  const size_t size = std::get<1>(GetParam());
  const size_t alignment = std::get<2>(GetParam());

  void *memory = nullptr;
  ze_host_mem_alloc_desc_t host_desc;
  host_desc.flags = flags;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverAllocHostMem(lzt::get_default_driver(), &host_desc, size,
                                 alignment, &memory));

  EXPECT_NE(nullptr, memory);

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverFreeMem(lzt::get_default_driver(), memory));
}

INSTANTIATE_TEST_CASE_P(
    TestHostMemParameterCombinations, zeDriverAllocHostMemTests,
    ::testing::Combine(
        ::testing::Values(ZE_HOST_MEM_ALLOC_FLAG_DEFAULT,
                          ZE_HOST_MEM_ALLOC_FLAG_BIAS_CACHED,
                          ZE_HOST_MEM_ALLOC_FLAG_BIAS_UNCACHED,
                          ZE_HOST_MEM_ALLOC_FLAG_BIAS_WRITE_COMBINED),
        lzt::memory_allocation_sizes, lzt::memory_allocation_alignments));

class xeHostMemPropertiesTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<std::tuple<size_t, size_t>> {};

TEST_P(
    xeHostMemPropertiesTests,
    GivenValidMemoryPointerWhenQueryingAttributesOnHostMemoryAllocationThenSuccessIsReturned) {

  const size_t size = std::get<0>(GetParam());
  const size_t alignment = std::get<1>(GetParam());

  void *memory = lzt::allocate_host_memory(size, alignment);

  ze_memory_allocation_properties_t mem_properties;
  mem_properties.version = ZE_MEMORY_ALLOCATION_PROPERTIES_VERSION_CURRENT;

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverGetMemAllocProperties(lzt::get_default_driver(), memory,
                                          &mem_properties, nullptr));

  EXPECT_EQ(ZE_MEMORY_ALLOCATION_PROPERTIES_VERSION_CURRENT,
            mem_properties.version);
  EXPECT_EQ(ZE_MEMORY_TYPE_HOST, mem_properties.type);

  lzt::free_memory(memory);
}

INSTANTIATE_TEST_CASE_P(TestHostMemGetPropertiesParameterCombinations,
                        xeHostMemPropertiesTests,
                        ::testing::Combine(lzt::memory_allocation_sizes,
                                           lzt::memory_allocation_alignments));

class xeHostMemGetAddressRangeTests : public ::testing::Test {};

TEST_F(
    xeHostMemGetAddressRangeTests,
    GivenBasePointerWhenQueryingBaseAddressofHostMemoryAllocationThenSuccessIsReturned) {

  const size_t size = 1;
  const size_t alignment = 1;

  void *base = nullptr;

  void *memory = lzt::allocate_host_memory(size, alignment);

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverGetMemAddressRange(lzt::get_default_driver(), memory, &base,
                                       nullptr));
  EXPECT_EQ(memory, base);

  lzt::free_memory(memory);
}

class xeHostMemGetAddressRangeSizeTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<std::tuple<size_t, size_t>> {};

TEST_P(
    xeHostMemGetAddressRangeSizeTests,
    GivenSizePointerWhenQueryingSizeOfHostMemoryAllocationThenSuccessIsReturned) {

  const size_t size = std::get<0>(GetParam());
  const size_t alignment = std::get<1>(GetParam());

  size_t size_out;
  void *memory = lzt::allocate_host_memory(size, alignment);

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverGetMemAddressRange(lzt::get_default_driver(), memory,
                                       nullptr, &size_out));
  EXPECT_GE(size_out, size);

  lzt::free_memory(memory);
}

INSTANTIATE_TEST_CASE_P(TestHostMemGetAddressRangeSizeTests,
                        xeHostMemGetAddressRangeSizeTests,
                        ::testing::Combine(lzt::memory_allocation_sizes,
                                           lzt::memory_allocation_alignments));

class xeHostMemGetAddressRangeParameterTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<std::tuple<size_t, size_t>> {};

TEST_P(
    xeHostMemGetAddressRangeParameterTests,
    GivenBasePointerAndSizeWhenQueryingBaseAddressOfHostMemoryAllocationThenSuccessIsReturned) {

  const size_t size = std::get<0>(GetParam());
  const size_t alignment = std::get<1>(GetParam());

  void *memory = lzt::allocate_host_memory(size, alignment);
  void *base = nullptr;
  size_t size_out;

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverGetMemAddressRange(lzt::get_default_driver(), memory, &base,
                                       &size_out));
  EXPECT_EQ(memory, base);

  if (size > 1) {
    uint8_t *mem_target = static_cast<uint8_t *>(memory) + (size - 1) / 2;
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeDriverGetMemAddressRange(lzt::get_default_driver(), mem_target,
                                         &base, &size_out));
    EXPECT_EQ(memory, base);
    EXPECT_GE(size_out, size);

    mem_target = static_cast<uint8_t *>(memory) + (size - 1);
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeDriverGetMemAddressRange(lzt::get_default_driver(), mem_target,
                                         &base, &size_out));
    EXPECT_EQ(memory, base);
    EXPECT_GE(size_out, size);
  }

  lzt::free_memory(memory);
}

INSTANTIATE_TEST_CASE_P(xeHostMemGetAddressRangeParameterizedTests,
                        xeHostMemGetAddressRangeParameterTests,
                        ::testing::Combine(lzt::memory_allocation_sizes,
                                           lzt::memory_allocation_alignments));

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
  lzt::zeCommandList cmdlist_;
  lzt::zeCommandQueue cmdqueue_;
};

TEST_F(
    xeHostSystemMemoryDeviceTests,
    GivenHostSystemAllocationWhenAccessingMemoryOnDeviceThenCorrectDataIsRead) {
  lzt::write_data_pattern(memory_, size_, 1);
  std::string module_name = "unified_mem_test.spv";
  ze_module_handle_t module = lzt::create_module(
      lzt::zeDevice::get_instance()->get_device(), module_name);
  std::string func_name = "unified_mem_test";

  lzt::FunctionArg arg;
  std::vector<lzt::FunctionArg> args;

  arg.arg_size = sizeof(uint8_t *);
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

TEST_F(
    xeHostSystemMemoryDeviceTests,
    GivenHostSystemAllocationWhenCopyingMemoryOnDeviceThenMemoryCopiedCorrectly) {
  lzt::write_data_pattern(memory_, size_, 1);
  uint8_t *other_system_memory = new uint8_t[size_];

  lzt::append_memory_copy(cmdlist_.command_list_, other_system_memory, memory_,
                          size_, nullptr);
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
  const uint8_t value = 0x55;
  lzt::write_data_pattern(memory_, size_, 1);
  lzt::append_memory_set(cmdlist_.command_list_, memory_, &value, size_);
  lzt::append_barrier(cmdlist_.command_list_, nullptr, 0, nullptr);
  lzt::close_command_list(cmdlist_.command_list_);
  lzt::execute_command_lists(cmdqueue_.command_queue_, 1,
                             &cmdlist_.command_list_, nullptr);
  lzt::synchronize(cmdqueue_.command_queue_, UINT32_MAX);
  for (unsigned int ui = 0; ui < size_; ui++)
    EXPECT_EQ(value, static_cast<uint8_t *>(memory_)[ui]);
}

} // namespace

// TODO: Test reading & writing shared memory from both device and host.
// TODO: Test reading & writing device memory from both device and host.
// TODO: Test reading & writing host memory from the host.
// TODO: Test accessessing shared memory from both device and host.
// TODO: Test memory handling on a device (OpenCL C kernels)
