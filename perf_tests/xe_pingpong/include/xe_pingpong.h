/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2019 Intel Corporation. All Rights Reserved.
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

#ifndef XE_PINGPONG_H
#define XE_PINGPONG_H

#include <chrono>
#include <cstdint>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <math.h>
#include <numeric>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

/* xe includes */
#include "ze_api.h"

enum TestType {
  DEVICE_MEM_KERNEL_ONLY,
  DEVICE_MEM_XFER,
  HOST_MEM_KERNEL_ONLY,
  HOST_MEM_NO_XFER,
  SHARED_MEM_KERNEL_ONLY,
  SHARED_MEM_MAP
};

struct L0Context {
  ze_command_queue_handle_t command_queue = nullptr;
  ze_command_list_handle_t command_list = nullptr;
  ze_module_handle_t module = nullptr;
  ze_driver_handle_t driver = nullptr;
  ze_device_handle_t device = nullptr;
  ze_kernel_handle_t function = nullptr;
  ze_group_count_t thread_group_dimensions = {1, 1, 1};
  void *device_input = nullptr;
  void *host_output = nullptr;
  void *shared_output = nullptr;
  uint32_t device_count = 0;
  const uint32_t default_device = 0;
  const uint32_t command_queue_id = 0;
  ze_device_properties_t device_property;
  ze_device_compute_properties_t device_compute_property;
  void init();
  void destroy();
  void print_ze_device_properties(const ze_device_properties_t &props);
  std::vector<uint8_t> load_binary_file(const std::string &file_path);
};

class XePingPong {
public:
  int num_execute = 20000;
  /* Helper Functions */
  void create_module(L0Context &context, std::vector<uint8_t> binary_file,
                     ze_module_format_t format, const char *build_flag);
  void set_argument_value(L0Context &context, uint32_t argIndex, size_t argSize,
                          const void *pArgValue);
  void setup_commandlist(L0Context &context, enum TestType test);
  void run_test(L0Context &context);
  void run_command_queue(L0Context &context);
  double measure_benchmark(L0Context &context, enum TestType test);
  void reset_commandlist(L0Context &context);
  void synchronize_command_queue(L0Context &context);
  void verify_result(int result);
};

#endif /* XE_PINGPONG_H */
