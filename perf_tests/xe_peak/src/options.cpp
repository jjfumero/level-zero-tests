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

static const char *usage_str =
    "\n xe_peak [OPTIONS]"
    "\n"
    "\n OPTIONS:"
    "\n  -p, --platform num          choose platform (num starts with 0)"
    "\n  -d, --device num            choose device   (num starts with 0)"
    "\n  -e                          time using xe events instead of std "
    "chrono timer"
    "\n                              hide driver latencies [default: No]"
    "\n  -t, string                  selectively run a particular test"
    "\n      global_bw               selectively run global bandwidth test"
    "\n      hp_compute              selectively run half precision compute "
    "test"
    "\n      sp_compute              selectively run single precision compute "
    "test"
    "\n      dp_compute              selectively run double precision compute "
    "test"
    "\n      int_compute             selectively run integer compute test"
    "\n      transfer_bw             selectively run transfer bandwidth test"
    "\n      kernel_lat              selectively run kernel latency test"
    "\n  -a                          run all above tests [default]"
    "\n  -v                          enable verbose prints"
    "\n  -i                          set number of iterations to run[default: "
    "50]"
    "\n  -w                          set number of warmup iterations to "
    "run[default: 10]"
    "\n  -h, --help                  display help message"
    "\n";

//---------------------------------------------------------------------
// Utility function which parses the arguments to xe_peak and
// sets the test parameters accordingly for main to execute the tests
// with the correct environment.
//---------------------------------------------------------------------
int XePeak::parse_arguments(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
      std::cout << usage_str;
      exit(0);
    } else if ((strcmp(argv[i], "-p") == 0) ||
               (strcmp(argv[i], "--platform") == 0)) {
      if ((i + 1) < argc) {
        specified_platform = static_cast<int>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else if ((strcmp(argv[i], "-d") == 0) ||
               (strcmp(argv[i], "--device") == 0)) {
      if ((i + 1) < argc) {
        specified_device = static_cast<int>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else if (strcmp(argv[i], "-e") == 0) {
      use_event_timer = true;
    } else if (strcmp(argv[i], "-v") == 0) {
      verbose = true;
    } else if (strcmp(argv[i], "-i") == 0) {
      if ((i + 1) < argc) {
        iters = static_cast<uint32_t>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else if (strcmp(argv[i], "-w") == 0) {
      if ((i + 1) < argc) {
        warmup_iterations = static_cast<int>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else if ((strcmp(argv[i], "-t") == 0)) {
      run_global_bw = false;
      run_hp_compute = false;
      run_sp_compute = false;
      run_dp_compute = false;
      run_int_compute = false;
      run_transfer_bw = false;
      run_kernel_lat = false;
      if ((i + 1) >= argc) {
        std::cout << usage_str;
        exit(-1);
      }
      if (strcmp(argv[i + 1], "global_bw") == 0) {
        run_global_bw = true;
        i++;
      } else if (strcmp(argv[i + 1], "hp_compute") == 0) {
        run_hp_compute = true;
        i++;
      } else if (strcmp(argv[i + 1], "sp_compute") == 0) {
        run_sp_compute = true;
        i++;
      } else if (strcmp(argv[i + 1], "dp_compute") == 0) {
        run_dp_compute = true;
        i++;
      } else if (strcmp(argv[i + 1], "int_compute") == 0) {
        run_int_compute = true;
        i++;
      } else if (strcmp(argv[i + 1], "transfer_bw") == 0) {
        run_transfer_bw = true;
        i++;
      } else if (strcmp(argv[i + 1], "kernel_lat") == 0) {
        run_kernel_lat = true;
        i++;
      } else {
        std::cout << usage_str;
        exit(-1);
      }
    } else if (strcmp(argv[i], "-a") == 0) {
      run_global_bw = run_hp_compute = run_sp_compute = run_dp_compute =
          run_int_compute = run_transfer_bw = run_kernel_lat = true;
    } else {
      std::cout << usage_str;
      exit(-1);
    }
  }
  return 0;
}
