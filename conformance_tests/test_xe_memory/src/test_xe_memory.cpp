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

#include "xe_utils/xe_utils.hpp"
#include "xe_test_harness/xe_test_harness.hpp"
#include "logging/logging.hpp"

namespace cs = compute_samples;

#include "xe_driver.h"
#include "xe_memory.h"

namespace {

class xeDeviceGroupAllocDeviceMemTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<
          std::tuple<xe_device_mem_alloc_flag_t, size_t, size_t>> {
protected:
  void SetUp() override {
    const xe_device_mem_alloc_flag_t flags = std::get<0>(GetParam());
    size_ = std::get<1>(GetParam());
    const size_t alignment = std::get<2>(GetParam());
    memory_ = cs::allocate_device_memory(size_, alignment, flags);
  }
  void TearDown() override { cs::free_memory(memory_); }
  size_t size_;
  void *memory_ = nullptr;
};

class xeDeviceGroupAllocDeviceMemParamsTests
    : public xeDeviceGroupAllocDeviceMemTests {};

TEST_P(
    xeDeviceGroupAllocDeviceMemParamsTests,
    GivenAllocationFlagsAndSizeAndAlignmentWhenAllocatingDeviceMemoryThenNotNullPointerIsReturned) {
}

INSTANTIATE_TEST_CASE_P(
    xeDeviceGroupAllocDeviceMemTestVaryFlagsAndSizeAndAlignment,
    xeDeviceGroupAllocDeviceMemParamsTests,
    ::testing::Combine(
        ::testing::Values(XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
                          XE_DEVICE_MEM_ALLOC_FLAG_BIAS_CACHED,
                          XE_DEVICE_MEM_ALLOC_FLAG_BIAS_UNCACHED),
        cs::memory_allocation_sizes, cs::memory_allocation_alignments));

class xeDeviceMemGetPropertiesTests : public xeDeviceGroupAllocDeviceMemTests {
};

TEST_P(
    xeDeviceMemGetPropertiesTests,
    GivenValidDeviceMemoryPointerWhenGettingPropertiesThenVersionAndTypeReturned) {
  xe_memory_allocation_properties_t memory_properties;
  memory_properties.version = XE_MEMORY_ALLOCATION_PROPERTIES_VERSION_CURRENT;
  EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupGetMemProperties(
                                   cs::get_default_device_group(), memory_,
                                   &memory_properties, nullptr));

  EXPECT_EQ(XE_MEMORY_ALLOCATION_PROPERTIES_VERSION_CURRENT,
            memory_properties.version);
  EXPECT_EQ(XE_MEMORY_TYPE_DEVICE, memory_properties.type);

  if (size_ > 0) {
    uint8_t *char_mem = static_cast<uint8_t *>(memory_);
    memory_properties.version = XE_MEMORY_ALLOCATION_PROPERTIES_VERSION_CURRENT;
    EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupGetMemProperties(
                                     cs::get_default_device_group(),
                                     static_cast<void *>(char_mem + size_ - 1),
                                     &memory_properties, nullptr));
    EXPECT_EQ(XE_MEMORY_ALLOCATION_PROPERTIES_VERSION_CURRENT,
              memory_properties.version);
    EXPECT_EQ(XE_MEMORY_TYPE_DEVICE, memory_properties.type);
  }
}

INSTANTIATE_TEST_CASE_P(
    xeDeviceMemGetPropertiesTestVaryFlagsAndSizeAndAlignment,
    xeDeviceMemGetPropertiesTests,
    ::testing::Combine(::testing::Values(XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT),
                       cs::memory_allocation_sizes,
                       cs::memory_allocation_alignments));

class xeDeviceMemGetAddressRangeTests
    : public xeDeviceGroupAllocDeviceMemTests {};

TEST_P(
    xeDeviceMemGetAddressRangeTests,
    GivenValidDeviceMemoryPointerWhenGettingAddressRangeThenBaseAddressAndSizeReturned) {

  void *pBase = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetMemAddressRange(cs::get_default_device_group(),
                                            memory_, &pBase, NULL));
  EXPECT_EQ(pBase, memory_);
  size_t addr_range_size = 0;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetMemAddressRange(cs::get_default_device_group(),
                                            memory_, NULL, &addr_range_size));

  // Get device mem size rounds size up to nearest page size
  EXPECT_GE(addr_range_size, size_);
  pBase = nullptr;
  addr_range_size = 0;
  if (size_ > 0) {
    uint8_t *char_mem = static_cast<uint8_t *>(memory_);
    EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupGetMemAddressRange(
                                     cs::get_default_device_group(),
                                     static_cast<void *>(char_mem + size_ - 1),
                                     &pBase, &addr_range_size));
  } else {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupGetMemAddressRange(
                                     cs::get_default_device_group(), memory_,
                                     &pBase, &addr_range_size));
  }
  EXPECT_EQ(pBase, memory_);
  // Get device mem size rounds size up to nearest page size
  EXPECT_GE(addr_range_size, size_);
}

INSTANTIATE_TEST_CASE_P(
    xeDeviceMemGetAddressRangeTestVaryFlagsAndSizeAndAlignment,
    xeDeviceMemGetAddressRangeTests,
    ::testing::Combine(::testing::Values(XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT),
                       cs::memory_allocation_sizes,
                       cs::memory_allocation_alignments));

class xeDeviceMemFreeTests : public ::testing::Test {};

TEST_F(
    xeDeviceMemFreeTests,
    GivenValidDeviceMemAllocationWhenFreeingDeviceMemoryThenSuccessIsReturned) {
  void *memory = cs::allocate_device_memory(1);
  cs::free_memory(memory);
}

class xeDeviceGroupAllocSharedMemTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<
          std::tuple<xe_device_mem_alloc_flag_t, xe_host_mem_alloc_flag_t,
                     size_t, size_t>> {};
TEST_P(
    xeDeviceGroupAllocSharedMemTests,
    GivenAllocationFlagsSizeAndAlignmentWhenAllocatingSharedMemoryThenNotNullPointerIsReturned) {
  const xe_device_mem_alloc_flag_t dev_flags = std::get<0>(GetParam());
  const xe_host_mem_alloc_flag_t host_flags = std::get<1>(GetParam());
  const size_t size = std::get<2>(GetParam());
  const size_t alignment = std::get<3>(GetParam());

  void *memory = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupAllocSharedMem(
                cs::get_default_device_group(),
                cs::xeDevice::get_instance()->get_device(), dev_flags, 1,
                host_flags, size, alignment, &memory));
  EXPECT_NE(nullptr, memory);

  cs::free_memory(memory);
}

INSTANTIATE_TEST_CASE_P(
    TestSharedMemFlagPermutations, xeDeviceGroupAllocSharedMemTests,
    ::testing::Combine(
        ::testing::Values(XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
                          XE_DEVICE_MEM_ALLOC_FLAG_BIAS_CACHED,
                          XE_DEVICE_MEM_ALLOC_FLAG_BIAS_UNCACHED),
        ::testing::Values(XE_HOST_MEM_ALLOC_FLAG_DEFAULT,
                          XE_HOST_MEM_ALLOC_FLAG_BIAS_CACHED,
                          XE_HOST_MEM_ALLOC_FLAG_BIAS_UNCACHED,
                          XE_HOST_MEM_ALLOC_FLAG_BIAS_WRITE_COMBINED),
        cs::memory_allocation_sizes, cs::memory_allocation_alignments));

class xeSharedMemGetPropertiesTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<std::tuple<size_t, size_t>> {};

TEST_P(xeSharedMemGetPropertiesTests,
       GivenSharedAllocationWhenGettingMemPropertiesThenSuccessIsReturned) {
  const size_t size = std::get<0>(GetParam());
  const size_t alignment = std::get<1>(GetParam());

  xe_memory_allocation_properties_t mem_properties;
  void *memory = cs::allocate_shared_memory(size, alignment);

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetMemProperties(cs::get_default_device_group(),
                                          memory, &mem_properties, nullptr));

  cs::free_memory(memory);
}

INSTANTIATE_TEST_CASE_P(TestSharedMemGetPropertiesPermutations,
                        xeSharedMemGetPropertiesTests,
                        ::testing::Combine(cs::memory_allocation_sizes,
                                           cs::memory_allocation_alignments));

class xeSharedMemGetAddressRangeTests : public ::testing::Test {};

TEST_F(xeSharedMemGetAddressRangeTests,
       GivenSharedAllocationWhenGettingAddressRangeThenCorrectSizeIsReturned) {
  const size_t size = 1;
  const size_t alignment = 1;

  void *memory = cs::allocate_shared_memory(size, alignment);
  size_t size_out;

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetMemAddressRange(cs::get_default_device_group(),
                                            memory, NULL, &size_out));
  EXPECT_GE(size_out, size);
  cs::free_memory(memory);
}

TEST_F(xeSharedMemGetAddressRangeTests,
       GivenSharedAllocationWhenGettingAddressRangeThenCorrectBaseIsReturned) {
  const size_t size = 1;
  const size_t alignment = 1;

  void *memory = cs::allocate_shared_memory(size, alignment);
  void *base = nullptr;

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetMemAddressRange(cs::get_default_device_group(),
                                            memory, &base, NULL));
  EXPECT_EQ(base, memory);
  cs::free_memory(memory);
}

class xeSharedMemGetAddressRangeParameterizedTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<std::tuple<size_t, size_t>> {};

TEST_P(
    xeSharedMemGetAddressRangeParameterizedTests,
    GivenSharedAllocationWhenGettingAddressRangeThenCorrectSizeAndBaseIsReturned) {
  const size_t size = std::get<0>(GetParam());
  const size_t alignment = std::get<1>(GetParam());

  void *memory = cs::allocate_shared_memory(size, alignment);
  void *base = nullptr;
  size_t size_out;

  // Test getting address info from begining of memory range
  uint8_t *mem_target = static_cast<uint8_t *>(memory);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetMemAddressRange(cs::get_default_device_group(),
                                            memory, &base, &size_out));
  EXPECT_GE(size_out, size);
  EXPECT_EQ(base, memory);

  if (size > 1) {
    // Test getting address info from middle of memory range
    mem_target = static_cast<uint8_t *>(memory) + (size - 1) / 2;
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeDeviceGroupGetMemAddressRange(cs::get_default_device_group(),
                                              mem_target, &base, &size_out));
    EXPECT_GE(size_out, size);
    EXPECT_EQ(memory, base);

    // Test getting address info from end of memory range
    mem_target = static_cast<uint8_t *>(memory) + (size - 1);
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeDeviceGroupGetMemAddressRange(cs::get_default_device_group(),
                                              mem_target, &base, &size_out));
    EXPECT_GE(size_out, size);
    EXPECT_EQ(memory, base);
  }
  cs::free_memory(memory);
}

INSTANTIATE_TEST_CASE_P(TestSharedMemGetAddressRangePermutations,
                        xeSharedMemGetAddressRangeParameterizedTests,
                        ::testing::Combine(cs::memory_allocation_sizes,
                                           cs::memory_allocation_alignments));

class xeDeviceGroupAllocHostMemTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<
          std::tuple<xe_host_mem_alloc_flag_t, size_t, size_t>> {};

TEST_P(
    xeDeviceGroupAllocHostMemTests,
    GivenFlagsSizeAndAlignmentWhenAllocatingHostMemoryThenNotNullPointerIsReturned) {

  const xe_host_mem_alloc_flag_t flags = std::get<0>(GetParam());

  const size_t size = std::get<1>(GetParam());
  const size_t alignment = std::get<2>(GetParam());

  void *memory = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupAllocHostMem(cs::get_default_device_group(), flags,
                                      size, alignment, &memory));

  EXPECT_NE(nullptr, memory);

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupFreeMem(cs::get_default_device_group(), memory));
}

INSTANTIATE_TEST_CASE_P(
    TestHostMemParameterCombinations, xeDeviceGroupAllocHostMemTests,
    ::testing::Combine(
        ::testing::Values(XE_HOST_MEM_ALLOC_FLAG_DEFAULT,
                          XE_HOST_MEM_ALLOC_FLAG_BIAS_CACHED,
                          XE_HOST_MEM_ALLOC_FLAG_BIAS_UNCACHED,
                          XE_HOST_MEM_ALLOC_FLAG_BIAS_WRITE_COMBINED),
        cs::memory_allocation_sizes, cs::memory_allocation_alignments));

class xeHostMemPropertiesTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<std::tuple<size_t, size_t>> {};

TEST_P(
    xeHostMemPropertiesTests,
    GivenValidMemoryPointerWhenQueryingAttributesOnHostMemoryAllocationThenSuccessIsReturned) {

  const size_t size = std::get<0>(GetParam());
  const size_t alignment = std::get<1>(GetParam());

  void *memory = cs::allocate_host_memory(size, alignment);

  xe_memory_allocation_properties_t mem_properties;
  mem_properties.version = XE_MEMORY_ALLOCATION_PROPERTIES_VERSION_CURRENT;

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetMemProperties(cs::get_default_device_group(),
                                          memory, &mem_properties, nullptr));

  EXPECT_EQ(XE_MEMORY_ALLOCATION_PROPERTIES_VERSION_CURRENT,
            mem_properties.version);
  EXPECT_EQ(XE_MEMORY_TYPE_HOST, mem_properties.type);

  cs::free_memory(memory);
}

INSTANTIATE_TEST_CASE_P(TestHostMemGetPropertiesParameterCombinations,
                        xeHostMemPropertiesTests,
                        ::testing::Combine(cs::memory_allocation_sizes,
                                           cs::memory_allocation_alignments));

class xeHostMemGetAddressRangeTests : public ::testing::Test {};

TEST_F(
    xeHostMemGetAddressRangeTests,
    GivenBasePointerWhenQueryingBaseAddressofHostMemoryAllocationThenSuccessIsReturned) {

  const size_t size = 1;
  const size_t alignment = 1;

  void *base = nullptr;

  void *memory = cs::allocate_host_memory(size, alignment);

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetMemAddressRange(cs::get_default_device_group(),
                                            memory, &base, nullptr));
  EXPECT_EQ(memory, base);

  cs::free_memory(memory);
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
  void *memory = cs::allocate_host_memory(size, alignment);

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetMemAddressRange(cs::get_default_device_group(),
                                            memory, nullptr, &size_out));
  EXPECT_GE(size_out, size);

  cs::free_memory(memory);
}

INSTANTIATE_TEST_CASE_P(TestHostMemGetAddressRangeSizeTests,
                        xeHostMemGetAddressRangeSizeTests,
                        ::testing::Combine(cs::memory_allocation_sizes,
                                           cs::memory_allocation_alignments));

class xeHostMemGetAddressRangeParameterTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<std::tuple<size_t, size_t>> {};

TEST_P(
    xeHostMemGetAddressRangeParameterTests,
    GivenBasePointerAndSizeWhenQueryingBaseAddressOfHostMemoryAllocationThenSuccessIsReturned) {

  const size_t size = std::get<0>(GetParam());
  const size_t alignment = std::get<1>(GetParam());

  void *memory = cs::allocate_host_memory(size, alignment);
  void *base = nullptr;
  size_t size_out;

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetMemAddressRange(cs::get_default_device_group(),
                                            memory, &base, &size_out));
  EXPECT_EQ(memory, base);

  if (size > 1) {
    uint8_t *mem_target = static_cast<uint8_t *>(memory) + (size - 1) / 2;
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeDeviceGroupGetMemAddressRange(cs::get_default_device_group(),
                                              mem_target, &base, &size_out));
    EXPECT_EQ(memory, base);
    EXPECT_GE(size_out, size);

    mem_target = static_cast<uint8_t *>(memory) + (size - 1);
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeDeviceGroupGetMemAddressRange(cs::get_default_device_group(),
                                              mem_target, &base, &size_out));
    EXPECT_EQ(memory, base);
    EXPECT_GE(size_out, size);
  }

  cs::free_memory(memory);
}

INSTANTIATE_TEST_CASE_P(xeHostMemGetAddressRangeParameterizedTests,
                        xeHostMemGetAddressRangeParameterTests,
                        ::testing::Combine(cs::memory_allocation_sizes,
                                           cs::memory_allocation_alignments));

} // namespace

// TODO: Test reading & writing shared memory from both device and host.
// TODO: Test reading & writing device memory from both device and host.
// TODO: Test reading & writing host memory from the host.
// TODO: Test accessessing shared memory from both device and host.
// TODO: Test memory handling on a device (OpenCL C kernels)
