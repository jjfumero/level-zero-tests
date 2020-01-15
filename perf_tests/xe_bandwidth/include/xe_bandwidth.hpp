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

#include <chrono>
#include "ze_api.h"
#include "xe_app.hpp"

class XeBandwidth {
public:
  XeBandwidth();
  ~XeBandwidth();
  int parse_arguments(int argc, char **argv);
  void test_host2device(void);
  void test_device2host(void);

  std::vector<size_t> transfer_size;
  size_t transfer_lower_limit = 1;
  size_t transfer_upper_limit = (1 << 28);
  bool verify = false;
  bool run_host2dev = true;
  bool run_dev2host = true;
  uint32_t number_iterations = 500;

private:
  void transfer_size_test(size_t size, void *destination_buffer,
                          void *source_buffer, long double &total_time_nsec);
  void transfer_size_test_verify(size_t size, long double &host2dev_time_nsec,
                                 long double &dev2host_time_nsec);
  long double measure_transfer(uint32_t num_transfer);
  void measure_transfer_verify(size_t buffer_size, uint32_t num_transfer,
                               long double &host2dev_time_nsec,
                               long double &dev2host_time_nsec);
  void print_results_host2device(size_t buffer_size,
                                 long double total_bandwidth,
                                 long double total_latency);
  void print_results_device2host(size_t buffer_size,
                                 long double total_bandwidth,
                                 long double total_latency);
  void calculate_metrics(long double total_time_nsec, /* Units in nanoseconds */
                         long double total_data_transfer, /* Units in bytes */
                         long double &total_bandwidth,
                         long double &total_latency);
  XeApp *benchmark;
  ze_command_queue_handle_t command_queue;
  ze_command_list_handle_t command_list;
  ze_command_list_handle_t command_list_verify;
  void *device_buffer;
  void *host_buffer;
  void *host_buffer_verify;
};
