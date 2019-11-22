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

void XePeak::xe_peak_global_bw(L0Context &context) {
  long double timed_lo, timed_go, timed, gbps;
  ze_result_t result = ZE_RESULT_SUCCESS;
  uint64_t temp_global_size, max_total_work_items;
  struct XeWorkGroups workgroup_info;
  TimingMeasurement type = is_bandwidth_with_event_timer();

  std::vector<uint8_t> binary_file =
      context.load_binary_file("xe_global_bw.spv");

  context.create_module(binary_file);

  uint64_t maxItems = max_device_object_size(context) / sizeof(float) / 2;
  uint64_t numItems = roundToMultipleOf(
      maxItems,
      (context.device_compute_property.maxGroupSizeX * FETCH_PER_WI * 16),
      global_bw_max_size);

  numItems = set_workgroups(context, numItems, &workgroup_info);

  std::vector<float> arr(static_cast<uint32_t>(numItems));
  for (uint32_t i = 0; i < numItems; i++) {
    arr[i] = static_cast<float>(i);
  }

  void *inputBuf;
  result = zeDriverAllocDeviceMem(
      context.driver, context.device, ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT, 0,
      static_cast<size_t>((numItems * sizeof(float))), 1, &inputBuf);
  if (result) {
    throw std::runtime_error("zeDriverAllocDeviceMem failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "inputBuf device buffer allocated\n";

  void *outputBuf;
  result = zeDriverAllocDeviceMem(
      context.driver, context.device, ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT, 0,
      static_cast<size_t>((numItems * sizeof(float))), 1, &outputBuf);
  if (result) {
    throw std::runtime_error("zeDriverAllocDeviceMem failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "outputBuf device buffer allocated\n";

  result =
      zeCommandListAppendMemoryCopy(context.command_list, inputBuf, arr.data(),
                                    (arr.size() * sizeof(float)), nullptr);
  if (result) {
    throw std::runtime_error("zeCommandListAppendMemoryCopy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "Input buffer copy encoded\n";

  result =
      zeCommandListAppendBarrier(context.command_list, nullptr, 0, nullptr);
  if (result) {
    throw std::runtime_error("zeCommandListAppendExecutionBarrier failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "Execution barrier appended\n";

  context.execute_commandlist_and_sync();

  /*Begin setup of Function*/

  ze_kernel_handle_t local_offset_v1;
  setup_function(context, local_offset_v1, "global_bandwidth_v1_local_offset",
                 inputBuf, outputBuf);
  ze_kernel_handle_t global_offset_v1;
  setup_function(context, global_offset_v1, "global_bandwidth_v1_global_offset",
                 inputBuf, outputBuf);
  ze_kernel_handle_t local_offset_v2;
  setup_function(context, local_offset_v2, "global_bandwidth_v2_local_offset",
                 inputBuf, outputBuf);
  ze_kernel_handle_t global_offset_v2;
  setup_function(context, global_offset_v2, "global_bandwidth_v2_global_offset",
                 inputBuf, outputBuf);
  ze_kernel_handle_t local_offset_v4;
  setup_function(context, local_offset_v4, "global_bandwidth_v4_local_offset",
                 inputBuf, outputBuf);
  ze_kernel_handle_t global_offset_v4;
  setup_function(context, global_offset_v4, "global_bandwidth_v4_global_offset",
                 inputBuf, outputBuf);
  ze_kernel_handle_t local_offset_v8;
  setup_function(context, local_offset_v8, "global_bandwidth_v8_local_offset",
                 inputBuf, outputBuf);
  ze_kernel_handle_t global_offset_v8;
  setup_function(context, global_offset_v8, "global_bandwidth_v8_global_offset",
                 inputBuf, outputBuf);
  ze_kernel_handle_t local_offset_v16;
  setup_function(context, local_offset_v16, "global_bandwidth_v16_local_offset",
                 inputBuf, outputBuf);
  ze_kernel_handle_t global_offset_v16;
  setup_function(context, global_offset_v16,
                 "global_bandwidth_v16_global_offset", inputBuf, outputBuf);

  std::cout << "Global memory bandwidth (GBPS)\n";

  ///////////////////////////////////////////////////////////////////////////
  // Vector width 1
  std::cout << "float : ";

  // Run 2 kind of bandwidth kernel
  // lo -- local_size offset - subsequent fetches at local_size offset
  // go -- global_size offset
  temp_global_size = (numItems / FETCH_PER_WI);

  max_total_work_items =
      set_workgroups(context, temp_global_size, &workgroup_info);

  timed_lo = run_kernel(context, local_offset_v1, workgroup_info, type);
  timed_go = run_kernel(context, global_offset_v1, workgroup_info, type);
  timed = (timed_lo < timed_go) ? timed_lo : timed_go;

  gbps = calculate_gbps(timed, numItems * sizeof(float));

  std::cout << gbps << " GBPS\n";

  ///////////////////////////////////////////////////////////////////////////
  // Vector width 2
  std::cout << "float2 : ";

  temp_global_size = (numItems / 2 / FETCH_PER_WI);

  max_total_work_items =
      set_workgroups(context, temp_global_size, &workgroup_info);

  timed_lo = run_kernel(context, local_offset_v2, workgroup_info, type);
  timed_go = run_kernel(context, global_offset_v2, workgroup_info, type);
  timed = (timed_lo < timed_go) ? timed_lo : timed_go;

  gbps = calculate_gbps(timed, numItems * sizeof(float));

  std::cout << gbps << " GBPS\n";

  ///////////////////////////////////////////////////////////////////////////
  // Vector width 4
  std::cout << "float4 : ";

  temp_global_size = (numItems / 4 / FETCH_PER_WI);

  max_total_work_items =
      set_workgroups(context, temp_global_size, &workgroup_info);

  timed_lo = run_kernel(context, local_offset_v4, workgroup_info, type);
  timed_go = run_kernel(context, global_offset_v4, workgroup_info, type);
  timed = (timed_lo < timed_go) ? timed_lo : timed_go;

  gbps = calculate_gbps(timed, numItems * sizeof(float));

  std::cout << gbps << " GBPS\n";

  ///////////////////////////////////////////////////////////////////////////
  // Vector width 8
  std::cout << "float8 : ";

  temp_global_size = (numItems / 8 / FETCH_PER_WI);

  max_total_work_items =
      set_workgroups(context, temp_global_size, &workgroup_info);

  timed_lo = run_kernel(context, local_offset_v8, workgroup_info, type);
  timed_go = run_kernel(context, global_offset_v8, workgroup_info, type);
  timed = (timed_lo < timed_go) ? timed_lo : timed_go;

  gbps = calculate_gbps(timed, numItems * sizeof(float));

  std::cout << gbps << " GBPS\n";

  ///////////////////////////////////////////////////////////////////////////
  // Vector width 16
  std::cout << "float16 : ";
  temp_global_size = (numItems / 16 / FETCH_PER_WI);

  max_total_work_items =
      set_workgroups(context, temp_global_size, &workgroup_info);

  timed_lo = run_kernel(context, local_offset_v16, workgroup_info, type);
  timed_go = run_kernel(context, global_offset_v16, workgroup_info, type);
  timed = (timed_lo < timed_go) ? timed_lo : timed_go;

  gbps = calculate_gbps(timed, numItems * sizeof(float));

  std::cout << gbps << " GBPS\n";

  result = zeKernelDestroy(local_offset_v1);
  if (result) {
    throw std::runtime_error("zeKernelDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "local_offset_v1 Function Destroyed\n";

  result = zeKernelDestroy(global_offset_v1);
  if (result) {
    throw std::runtime_error("zeKernelDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "global_offset_v1 Function Destroyed\n";

  result = zeKernelDestroy(local_offset_v2);
  if (result) {
    throw std::runtime_error("zeKernelDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "local_offset_v2 Function Destroyed\n";

  result = zeKernelDestroy(global_offset_v2);
  if (result) {
    throw std::runtime_error("zeKernelDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "global_offset_v2 Function Destroyed\n";

  result = zeKernelDestroy(local_offset_v4);
  if (result) {
    throw std::runtime_error("zeKernelDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "local_offset_v4 Function Destroyed\n";

  result = zeKernelDestroy(global_offset_v4);
  if (result) {
    throw std::runtime_error("zeKernelDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "global_offset_v4 Function Destroyed\n";

  result = zeKernelDestroy(local_offset_v8);
  if (result) {
    throw std::runtime_error("zeKernelDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "local_offset_v8 Function Destroyed\n";

  result = zeKernelDestroy(global_offset_v8);
  if (result) {
    throw std::runtime_error("zeKernelDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "global_offset_v8 Function Destroyed\n";

  result = zeKernelDestroy(local_offset_v16);
  if (result) {
    throw std::runtime_error("zeKernelDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "local_offset_v16 Function Destroyed\n";

  result = zeKernelDestroy(global_offset_v16);
  if (result) {
    throw std::runtime_error("zeKernelDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "global_offset_v16 Function Destroyed\n";

  result = zeDriverFreeMem(context.driver, inputBuf);
  if (result) {
    throw std::runtime_error("zeDriverFreeMem failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "Input Buffer freed\n";

  result = zeDriverFreeMem(context.driver, outputBuf);
  if (result) {
    throw std::runtime_error("zeDriverFreeMem failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "Output Buffer freed\n";

  result = zeModuleDestroy(context.module);
  if (result) {
    throw std::runtime_error("zeModuleDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "Module destroyed\n";

  print_test_complete();
}
