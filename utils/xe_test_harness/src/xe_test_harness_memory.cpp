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

namespace cs = compute_samples;

#include "xe_memory.h"

namespace compute_samples {

void *allocate_host_memory(const size_t size) {
  return allocate_host_memory(size, 1);
}

void *allocate_host_memory(const size_t size, const size_t alignment) {
  const xe_host_mem_alloc_flag_t flags = XE_HOST_MEM_ALLOC_FLAG_DEFAULT;

  void *memory = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS, xeHostMemAlloc(flags, size, alignment, &memory));
  EXPECT_NE(nullptr, memory);

  return memory;
}

void *allocate_device_memory(const size_t size) {
  return (allocate_device_memory(size, 1));
}

void *allocate_device_memory(const size_t size, const size_t alignment) {
  return (allocate_device_memory(size, alignment,
                                 XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT));
}

void *allocate_device_memory(const size_t size, const size_t alignment,
                             const xe_device_mem_alloc_flag_t flags) {
  xe_device_handle_t device = xeDevice::get_instance()->get_device();
  return allocate_device_memory(size, alignment, flags, device);
}

void *allocate_device_memory(const size_t size, const size_t alignment,
                             const xe_device_mem_alloc_flag_t flags,
                             xe_device_handle_t device) {

  void *memory = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceMemAlloc(device, flags, size, alignment, &memory));
  EXPECT_NE(nullptr, memory);

  return memory;
}

void *allocate_shared_memory(const size_t size) {
  return allocate_shared_memory(size, 1);
}
void *allocate_shared_memory(const size_t size, const size_t alignment) {
  return allocate_shared_memory(size, alignment,
                                XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
                                XE_HOST_MEM_ALLOC_FLAG_DEFAULT);
}

void *allocate_shared_memory(const size_t size, const size_t alignment,
                             const xe_device_mem_alloc_flag_t dev_flags,
                             const xe_host_mem_alloc_flag_t host_flags) {
  xe_device_handle_t device = xeDevice::get_instance()->get_device();

  return allocate_shared_memory(size, alignment,
                                XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
                                XE_HOST_MEM_ALLOC_FLAG_DEFAULT, device);
}

void *allocate_shared_memory(const size_t size, const size_t alignment,
                             const xe_device_mem_alloc_flag_t dev_flags,
                             const xe_host_mem_alloc_flag_t host_flags,
                             xe_device_handle_t device) {
  void *memory = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS, xeSharedMemAlloc(device, dev_flags, host_flags,
                                                size, alignment, &memory));
  EXPECT_NE(nullptr, memory);

  return memory;
}

void free_memory(const void *ptr) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeMemFree(ptr));
}

void allocate_mem_and_get_ipc_handle(xe_ipc_mem_handle_t *mem_handle,
                                     void **memory, xe_memory_type_t mem_type) {

  *memory = nullptr;
  switch (mem_type) {
  case XE_MEMORY_TYPE_HOST:
    *memory = allocate_host_memory(1);
    break;
  case XE_MEMORY_TYPE_DEVICE:
    *memory = allocate_device_memory(1);
    break;
  case XE_MEMORY_TYPE_SHARED:
    *memory = allocate_shared_memory(1);
    break;
  default:
    break;
  }

  EXPECT_EQ(XE_RESULT_SUCCESS, xeIpcGetMemHandle(*memory, mem_handle));
}

void *allocate_device_group_device_memory(xe_device_group_handle_t group_handle,
                                          xe_device_handle_t device_handle,
                                          xe_device_mem_alloc_flag_t flags,
                                          uint32_t ordinal, size_t size,
                                          size_t alignment) {
  void *mem_ptr;

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupAllocDeviceMem(group_handle, device_handle, flags,
                                        ordinal, size, alignment, &mem_ptr));
  return mem_ptr;
}

void free_device_group_memory(xe_device_group_handle_t group_handle,
                              void *mem_ptr) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupFreeMem(group_handle, mem_ptr));
}
}; // namespace compute_samples
