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

// There is no equivalent of cl_half (i.e. 16 bit floating point)
// in the C/C++ standard. So we are just going to allocate 16 bits
// using a C type knowing it will be the same size.
#define cl_half uint16_t

void XePeak::xe_peak_hp_compute(L0Context &context) {
  float gflops, timed;
  xe_result_t result = XE_RESULT_SUCCESS;
  TimingMeasurement type = is_bandwidth_with_event_timer();
  float flops_per_work_item = 4096;
  struct XeWorkGroups workgroup_info;
  float input_value = 1.3f;

  std::vector<uint8_t> binary_file =
      context.load_binary_file("xe_hp_compute.spv");

  context.create_module(binary_file);

  // same multiplier in clPeak
  uint64_t max_work_items = get_max_work_items(context) * 2048;
  uint64_t max_number_of_allocated_items =
      max_device_object_size(context) / sizeof(cl_half);
  uint64_t number_of_work_items =
      MIN(max_number_of_allocated_items, (max_work_items * sizeof(cl_half)));
  number_of_work_items =
      set_workgroups(context, number_of_work_items, &workgroup_info);

  void *device_output_buffer;
  result = xeDeviceGroupAllocDeviceMem(
      context.device_group, context.device, XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT, 0,
      static_cast<size_t>((number_of_work_items * sizeof(cl_half))), 1,
      &device_output_buffer);
  if (result) {
    throw std::runtime_error("xeDeviceGroupAllocDeviceMem failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "device output buffer allocated\n";

  result =
      xeCommandListAppendBarrier(context.command_list, nullptr, 0, nullptr);
  if (result) {
    throw std::runtime_error("xeCommandListAppendExecutionBarrier failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "Execution barrier appended\n";

  context.execute_commandlist_and_sync();

  /*Begin setup of Function*/

  xe_function_handle_t compute_hp_v1;
  setup_function(context, compute_hp_v1, "compute_hp_v1", device_output_buffer,
                 &input_value, sizeof(float));
  xe_function_handle_t compute_hp_v2;
  setup_function(context, compute_hp_v2, "compute_hp_v2", device_output_buffer,
                 &input_value, sizeof(float));
  xe_function_handle_t compute_hp_v4;
  setup_function(context, compute_hp_v4, "compute_hp_v4", device_output_buffer,
                 &input_value, sizeof(float));
  xe_function_handle_t compute_hp_v8;
  setup_function(context, compute_hp_v8, "compute_hp_v8", device_output_buffer,
                 &input_value, sizeof(float));
  xe_function_handle_t compute_hp_v16;
  setup_function(context, compute_hp_v16, "compute_hp_v16",
                 device_output_buffer, &input_value, sizeof(float));

  std::cout << "Half Precision Compute (GFLOPS)\n";

  ///////////////////////////////////////////////////////////////////////////
  // Vector width 1
  std::cout << "half : ";
  timed = run_kernel(context, compute_hp_v1, workgroup_info, type);
  gflops = number_of_work_items * flops_per_work_item / timed / 1e3f;
  std::cout << gflops << " GFLOPS\n";

  ///////////////////////////////////////////////////////////////////////////
  // Vector width 2
  std::cout << "half2 : ";
  timed = run_kernel(context, compute_hp_v2, workgroup_info, type);
  gflops = number_of_work_items * flops_per_work_item / timed / 1e3f;
  std::cout << gflops << " GFLOPS\n";

  ///////////////////////////////////////////////////////////////////////////
  // Vector width 4
  std::cout << "half4 : ";
  timed = run_kernel(context, compute_hp_v4, workgroup_info, type);
  gflops = number_of_work_items * flops_per_work_item / timed / 1e3f;
  std::cout << gflops << " GFLOPS\n";

  ///////////////////////////////////////////////////////////////////////////
  // Vector width 8
  std::cout << "half8 : ";
  timed = run_kernel(context, compute_hp_v8, workgroup_info, type);
  gflops = number_of_work_items * flops_per_work_item / timed / 1e3f;
  std::cout << gflops << " GFLOPS\n";

  ///////////////////////////////////////////////////////////////////////////
  // Vector width 16
  std::cout << "half16 : ";
  timed = run_kernel(context, compute_hp_v16, workgroup_info, type);
  gflops = number_of_work_items * flops_per_work_item / timed / 1e3f;
  std::cout << gflops << " GFLOPS\n";

  result = xeFunctionDestroy(compute_hp_v1);
  if (result) {
    throw std::runtime_error("xeFunctionDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "compute_hp_v1 Function Destroyed\n";

  result = xeFunctionDestroy(compute_hp_v2);
  if (result) {
    throw std::runtime_error("xeFunctionDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "compute_hp_v2 Function Destroyed\n";

  result = xeFunctionDestroy(compute_hp_v4);
  if (result) {
    throw std::runtime_error("xeFunctionDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "compute_hp_v4 Function Destroyed\n";

  result = xeFunctionDestroy(compute_hp_v8);
  if (result) {
    throw std::runtime_error("xeFunctionDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "compute_hp_v8 Function Destroyed\n";

  result = xeFunctionDestroy(compute_hp_v16);
  if (result) {
    throw std::runtime_error("xeFunctionDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "compute_hp_v16 Function Destroyed\n";

  result = xeDeviceGroupFreeMem(context.device_group, device_output_buffer);
  if (result) {
    throw std::runtime_error("xeDeviceGroupFreeMem failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "Output Buffer freed\n";

  result = xeModuleDestroy(context.module);
  if (result) {
    throw std::runtime_error("xeModuleDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "Module destroyed\n";

  print_test_complete();
}
