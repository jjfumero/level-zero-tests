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

#ifndef COMPUTE_SAMPLES_XE_TEST_HARNESS_MEMORY_HPP
#define COMPUTE_SAMPLES_XE_TEST_HARNESS_MEMORY_HPP

#include "xe_test_harness_device.hpp"
#include "xe_memory.h"
#include "gtest/gtest.h"

namespace compute_samples {

#ifndef XE_MAX_IPC_HANDLE_SIZE
#define XE_MAX_IPC_HANDLE_SIZE 64
#endif

const auto memory_allocation_sizes =
    ::testing::Values(1, 10, 100, 1000, 10000, 100000, 1000000, 100000000);
const auto memory_allocation_alignments =
    ::testing::Values(1, 2, 4, 8, 16, 32, 64);

void *allocate_host_memory(const size_t size);
void *allocate_host_memory(const size_t size, const size_t alignment);
void *allocate_device_memory(const size_t size);
void *allocate_device_memory(const size_t size, const size_t alignment);
void *allocate_device_memory(const size_t size, const size_t alignment,
                             const xe_device_mem_alloc_flag_t flags);
void *allocate_device_memory(const size_t size, const size_t alignment,
                             const xe_device_mem_alloc_flag_t flags,
                             xe_device_handle_t device,
                             xe_device_group_handle_t device_group);
void *allocate_device_memory(const size_t size, const size_t alignment,
                             const xe_device_mem_alloc_flag_t flags,
                             const uint32_t ordinal,
                             xe_device_handle_t device_handle,
                             xe_device_group_handle_t device_group);
void *allocate_shared_memory(const size_t size);
void *allocate_shared_memory(const size_t size, const size_t alignment);
void *allocate_shared_memory(const size_t size, const size_t alignment,
                             const xe_device_mem_alloc_flag_t dev_flags,
                             const xe_host_mem_alloc_flag_t host_flags);
void *allocate_shared_memory(const size_t size, const size_t alignment,
                             const xe_device_mem_alloc_flag_t dev_flags,
                             const xe_host_mem_alloc_flag_t host_flags,
                             xe_device_handle_t device);
void free_memory(const void *ptr);
void free_memory(xe_device_group_handle_t device_group, const void *ptr);

void allocate_mem_and_get_ipc_handle(xe_ipc_mem_handle_t *handle, void **memory,
                                     xe_memory_type_t mem_type);

void write_data_pattern(void *buff, size_t size, int8_t data_pattern);
void validate_data_pattern(void *buff, size_t size, int8_t data_pattern);

}; // namespace compute_samples
#endif
