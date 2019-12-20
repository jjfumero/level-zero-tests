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

#include "xe_bandwidth.hpp"

static const char *usage_str =
    "\n xe_bandwidth [OPTIONS]"
    "\n"
    "\n OPTIONS:"
    "\n  -t, string               selectively run a particular test:"
    "\n      h2d or H2D                       run only Host-to-Device tests"
    "\n      d2h or D2H                       run only Device-to-Host tests "
    "\n                            [default:  both]"
    "\n  -v                       enable verificaton"
    "\n                            [default:  disabled]"
    "\n  -i                       set number of iterations per transfer"
    "\n                            [default:  500]"
    "\n  -s                       select only one transfer size (bytes) "
    "\n  -sb                      select beginning transfer size (bytes)"
    "\n                            [default:  1]"
    "\n  -se                      select ending transfer size (bytes)"
    "\n                            [default: 2^30]"
    "\n  -h, --help               display help message"
    "\n";

//---------------------------------------------------------------------
// Utility function which parses the arguments to xe_peak and
// sets the test parameters accordingly for main to execute the tests
// with the correct environment.
//---------------------------------------------------------------------
int XeBandwidth::parse_arguments(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
      std::cout << usage_str;
      exit(0);
    } else if (strcmp(argv[i], "-v") == 0) {
      verify = true;
    } else if (strcmp(argv[i], "-i") == 0) {
      if ((i + 1) < argc) {
        number_iterations =
            static_cast<uint32_t>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else if (strcmp(argv[i], "-s") == 0) {
      if ((i + 1) < argc) {
        transfer_lower_limit =
            static_cast<size_t>(strtoul(argv[i + 1], NULL, 0));
        transfer_upper_limit = transfer_lower_limit;
        i++;
      }
    } else if (strcmp(argv[i], "-sb") == 0) {
      if ((i + 1) < argc) {
        transfer_lower_limit =
            static_cast<size_t>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else if (strcmp(argv[i], "-se") == 0) {
      if ((i + 1) < argc) {
        transfer_upper_limit =
            static_cast<size_t>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else if ((strcmp(argv[i], "-t") == 0)) {
      run_host2dev = false;
      run_dev2host = false;

      if ((i + 1) >= argc) {
        std::cout << usage_str;
        exit(-1);
      }
      if ((strcmp(argv[i + 1], "h2d") == 0) ||
          (strcmp(argv[i + 1], "H2D") == 0)) {
        run_host2dev = true;
        i++;
      } else if ((strcmp(argv[i + 1], "d2h") == 0) ||
                 (strcmp(argv[i + 1], "D2H") == 0)) {
        run_dev2host = true;
        i++;
      } else {
        std::cout << usage_str;
        exit(-1);
      }
    } else {
      std::cout << usage_str;
      exit(-1);
    }
  }
  return 0;
}
