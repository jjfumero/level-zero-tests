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

#include "xe_test_harness/xe_test_harness.hpp"
#include "gtest/gtest.h"

namespace lzt = level_zero_tests;

#include "ze_api.h"

namespace level_zero_tests {

void *allocate_host_memory(const size_t size) {
  return allocate_host_memory(size, 1);
}

void *allocate_host_memory(const size_t size, const size_t alignment) {
  const ze_host_mem_alloc_flag_t flags = ZE_HOST_MEM_ALLOC_FLAG_DEFAULT;

  ze_driver_handle_t driver = lzt::get_default_driver();

  void *memory = nullptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverAllocHostMem(driver, flags, size, alignment, &memory));
  EXPECT_NE(nullptr, memory);

  return memory;
}

void *allocate_device_memory(const size_t size) {
  return (allocate_device_memory(size, 1));
}

void *allocate_device_memory(const size_t size, const size_t alignment) {
  return (allocate_device_memory(size, alignment,
                                 ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT));
}

void *allocate_device_memory(const size_t size, const size_t alignment,
                             const ze_device_mem_alloc_flag_t flags) {

  ze_driver_handle_t driver = lzt::get_default_driver();
  ze_device_handle_t device = zeDevice::get_instance()->get_device();
  return allocate_device_memory(size, alignment, flags, device, driver);
}

void *allocate_device_memory(const size_t size, const size_t alignment,
                             const ze_device_mem_alloc_flag_t flags,
                             ze_device_handle_t device,
                             ze_driver_handle_t driver) {

  return allocate_device_memory(size, alignment, flags, 0, device, driver);
}

void *allocate_device_memory(const size_t size, const size_t alignment,
                             const ze_device_mem_alloc_flag_t flags,
                             const uint32_t ordinal,
                             ze_device_handle_t device_handle,
                             ze_driver_handle_t driver) {
  void *memory = nullptr;

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverAllocDeviceMem(driver, device_handle, flags, ordinal, size,
                                   alignment, &memory));
  EXPECT_NE(nullptr, memory);

  return memory;
}
void *allocate_shared_memory(const size_t size) {
  return allocate_shared_memory(size, 1);
}
void *allocate_shared_memory(const size_t size, const size_t alignment) {
  return allocate_shared_memory(size, alignment,
                                ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
                                ZE_HOST_MEM_ALLOC_FLAG_DEFAULT);
}

void *allocate_shared_memory(const size_t size, const size_t alignment,
                             const ze_device_mem_alloc_flag_t dev_flags,
                             const ze_host_mem_alloc_flag_t host_flags) {

  ze_device_handle_t device = zeDevice::get_instance()->get_device();

  return allocate_shared_memory(size, alignment,
                                ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
                                ZE_HOST_MEM_ALLOC_FLAG_DEFAULT, device);
}

void *allocate_shared_memory(const size_t size, const size_t alignment,
                             const ze_device_mem_alloc_flag_t dev_flags,
                             const ze_host_mem_alloc_flag_t host_flags,
                             ze_device_handle_t device) {

  uint32_t ordinal = 0;
  auto driver = lzt::get_default_driver();

  void *memory = nullptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverAllocSharedMem(driver, device, dev_flags, ordinal,
                                   host_flags, size, alignment, &memory));
  EXPECT_NE(nullptr, memory);

  return memory;
}

void free_memory(const void *ptr) {
  free_memory(lzt::get_default_driver(), ptr);
}

void free_memory(ze_driver_handle_t driver, const void *ptr) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDriverFreeMem(driver, (void *)ptr));
}

void allocate_mem_and_get_ipc_handle(ze_ipc_mem_handle_t *mem_handle,
                                     void **memory, ze_memory_type_t mem_type) {

  *memory = nullptr;
  switch (mem_type) {
  case ZE_MEMORY_TYPE_HOST:
    *memory = allocate_host_memory(1);
    break;
  case ZE_MEMORY_TYPE_DEVICE:
    *memory = allocate_device_memory(1);
    break;
  case ZE_MEMORY_TYPE_SHARED:
    *memory = allocate_shared_memory(1);
    break;
  default:
    break;
  }

  EXPECT_EQ(
      ZE_RESULT_SUCCESS,
      zeDriverGetMemIpcHandle(lzt::get_default_driver(), *memory, mem_handle));
}

void write_data_pattern(void *buff, size_t size, int8_t data_pattern) {
  int8_t *pbuff = static_cast<int8_t *>(buff);
  int8_t dp = data_pattern;
  for (size_t i = 0; i < size; i++) {
    pbuff[i] = dp;
    dp = (dp + data_pattern) & 0xff;
  }
}

void validate_data_pattern(void *buff, size_t size, int8_t data_pattern) {
  int8_t *pbuff = static_cast<int8_t *>(buff);
  int8_t dp = data_pattern;
  for (size_t i = 0; i < size; i++) {
    EXPECT_EQ(pbuff[i], dp);
    dp = (dp + data_pattern) & 0xff;
  }
}

}; // namespace level_zero_tests
