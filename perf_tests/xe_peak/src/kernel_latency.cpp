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

void XePeak::xe_peak_kernel_latency(L0Context &context) {
  uint64_t num_items = get_max_work_items(context) * FETCH_PER_WI;
  uint64_t global_size = (num_items / FETCH_PER_WI);

  struct XeWorkGroups workgroup_info;
  set_workgroups(context, global_size, &workgroup_info);
  float latency = 0;
  ze_result_t result = ZE_RESULT_SUCCESS;

  std::vector<uint8_t> binary_file =
      context.load_binary_file("xe_global_bw.spv");

  context.create_module(binary_file);

  void *inputBuf;
  result = zeDriverAllocDeviceMem(
      context.driver, context.device, ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT, 0,
      static_cast<size_t>((num_items * sizeof(float))), 1, &inputBuf);
  if (result) {
    throw std::runtime_error("zeDriverAllocDeviceMem failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "inputBuf device buffer allocated\n";

  void *outputBuf;
  result = zeDriverAllocDeviceMem(
      context.driver, context.device, ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT, 0,
      static_cast<size_t>((num_items * sizeof(float))), 1, &outputBuf);
  if (result) {
    throw std::runtime_error("zeDriverAllocDeviceMem failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "outputBuf device buffer allocated\n";

  ze_kernel_handle_t local_offset_v1;
  setup_function(context, local_offset_v1, "global_bandwidth_v1_local_offset",
                 inputBuf, outputBuf);

  ///////////////////////////////////////////////////////////////////////////
  std::cout << "Kernel Launch Latency : ";
  latency = run_kernel(context, local_offset_v1, workgroup_info,
                       TimingMeasurement::KERNEL_LAUNCH_LATENCY, false);
  std::cout << latency << " (uS)\n";

  ///////////////////////////////////////////////////////////////////////////
  std::cout << "Kernel Latency : ";
  latency = run_kernel(context, local_offset_v1, workgroup_info,
                       TimingMeasurement::KERNEL_COMPLETE_LATENCY, false);
  std::cout << latency << " (uS)\n";

  result = zeKernelDestroy(local_offset_v1);
  if (result) {
    throw std::runtime_error("zeKernelDestroy failed: " +
                             std::to_string(result));
  }
  if (verbose)
    std::cout << "local_offset_v1 Function Destroyed\n";

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
