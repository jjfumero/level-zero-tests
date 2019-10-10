/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2016 - 2019 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 */

#include "../include/xe_peak.h"

void XePeak::_transfer_bw_gpu_copy(L0Context &context, void *destination_buffer,
                                   void *source_buffer, size_t buffer_size) {
  Timer timer;
  float gbps, timed;
  ze_result_t result = ZE_RESULT_SUCCESS;

  for (uint32_t i = 0; i < warmup_iterations; i++) {
    result =
        zeCommandListAppendMemoryCopy(context.command_list, destination_buffer,
                                      source_buffer, buffer_size, nullptr);
    if (result) {
      throw std::runtime_error("zeCommandListAppendMemoryCopy failed: " +
                               std::to_string(result));
    }
  }
  context.execute_commandlist_and_sync();

  timer.start();
  for (uint32_t i = 0; i < iters; i++) {
    result =
        zeCommandListAppendMemoryCopy(context.command_list, destination_buffer,
                                      source_buffer, buffer_size, nullptr);
    if (result) {
      throw std::runtime_error("zeCommandListAppendMemoryCopy failed: " +
                               std::to_string(result));
    }
  }

  context.execute_commandlist_and_sync();
  timed = timer.stopAndTime();
  timed /= static_cast<float>(iters);

  gbps = static_cast<float>(buffer_size) / timed / 1e3f;

  std::cout << gbps << " GBPS\n";
}

void XePeak::_transfer_bw_host_copy(void *destination_buffer,
                                    void *source_buffer, size_t buffer_size) {
  Timer timer;
  float gbps, timed;

  for (uint32_t i = 0; i < warmup_iterations; i++) {
    memcpy(destination_buffer, source_buffer, buffer_size);
  }

  timer.start();
  for (uint32_t i = 0; i < iters; i++) {
    memcpy(destination_buffer, source_buffer, buffer_size);
  }
  timed = timer.stopAndTime();

  timed /= static_cast<float>(iters);
  gbps = static_cast<float>(buffer_size) / timed / 1e3f;

  std::cout << gbps << " GBPS\n";
}

void XePeak::_transfer_bw_shared_memory(L0Context &context,
                                        std::vector<float> local_memory) {
  ze_result_t result = ZE_RESULT_SUCCESS;
  void *shared_memory_buffer = nullptr;
  uint64_t number_of_items = local_memory.size();
  size_t local_memory_size =
      static_cast<size_t>(number_of_items * sizeof(float));

  result = zeDriverAllocSharedMem(context.driver, context.device,
                                  ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT, 0,
                                  ZE_HOST_MEM_ALLOC_FLAG_DEFAULT,
                                  local_memory_size, 1, &shared_memory_buffer);
  if (result) {
    throw std::runtime_error("zeDriverAllocSharedMem failed: " +
                             std::to_string(result));
  }

  std::cout << "GPU Copy Host to Shared Memory : ";
  _transfer_bw_gpu_copy(context, shared_memory_buffer, local_memory.data(),
                        local_memory_size);

  std::cout << "GPU Copy Shared Memory to Host : ";
  _transfer_bw_gpu_copy(context, local_memory.data(), shared_memory_buffer,
                        local_memory_size);
  std::cout << "memcpy to mapped ptr : ";
  _transfer_bw_host_copy(shared_memory_buffer, local_memory.data(),
                         local_memory_size);
  std::cout << "memcpy from mapped ptr : ";
  _transfer_bw_host_copy(local_memory.data(), shared_memory_buffer,
                         local_memory_size);

  result = zeDriverFreeMem(context.driver, shared_memory_buffer);
  if (result) {
    throw std::runtime_error("zeDriverFreeMem failed: " +
                             std::to_string(result));
  }
}

void XePeak::xe_peak_transfer_bw(L0Context &context) {
  ze_result_t result = ZE_RESULT_SUCCESS;
  uint64_t max_number_of_allocated_items =
      max_device_object_size(context) / sizeof(float) / 2;
  uint64_t number_of_items = roundToMultipleOf(
      max_number_of_allocated_items,
      context.device_compute_property.maxGroupSizeX, transfer_bw_max_size);

  std::vector<float> local_memory(static_cast<uint32_t>(number_of_items));
  for (uint32_t i = 0; i < static_cast<uint32_t>(number_of_items); i++) {
    local_memory[i] = static_cast<float>(i);
  }

  size_t local_memory_size = (local_memory.size() * sizeof(float));

  void *device_buffer;
  result = zeDriverAllocDeviceMem(
      context.driver, context.device, ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT, 0,
      static_cast<size_t>(sizeof(float) * number_of_items), 1, &device_buffer);
  if (result) {
    throw std::runtime_error("zeDriverAllocDeviceMem failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "device buffer allocated\n";

  std::cout << "Transfer Bandwidth (GBPS)\n";

  std::cout << "enqueueWriteBuffer : ";
  _transfer_bw_gpu_copy(context, device_buffer, local_memory.data(),
                        local_memory_size);

  std::cout << "enqueueReadBuffer : ";
  /*TODO: Add support for timing the enqueue using event timers*/
  _transfer_bw_gpu_copy(context, local_memory.data(), device_buffer,
                        local_memory_size);

  _transfer_bw_shared_memory(context, local_memory);

  result = zeDriverFreeMem(context.driver, device_buffer);
  if (result) {
    throw std::runtime_error("zeDriverFreeMem failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "Device Buffer freed\n";

  print_test_complete();
}
