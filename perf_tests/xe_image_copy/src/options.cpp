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

#include "../include/xe_image_copy.h"

static const char *usage_str =
    "\n xe_image_copy [OPTIONS]"
    "\n"
    "\n OPTIONS:"
    "\n  -w                          choose image width (default setting with "
    "128)"
    "\n  -h                          choose image height (default setting with "
    "128)"
    "\n  -d                          choose image depth (default setting with "
    "1)"
    "\n  -ox                         choose The origin x offset for region in "
    "pixels (default setting with 0)"
    "\n  -oy                         choose The origin y offset for region in "
    "pixels (default setting with 0)"
    "\n  -oz                         choose The origin z offset for region in "
    "pixels (default setting with 0)"
    "\n  -i                          set number of iterations to run[default: "
    "50]"
    "\n  -w                          set number of warmup iterations to "
    "run[default: 10]"
    "\n  --help                      display help message"
    "\n";

//---------------------------------------------------------------------
// Utility function which parses the arguments to xe_peak and
// sets the test parameters accordingly for main to execute the tests
// with the correct environment.
//---------------------------------------------------------------------
int XeImageCopy::parse_arguments(int argc, char **argv) {

  for (int i = 1; i < argc; i++) {
    if ((strcmp(argv[i], "--help") == 0)) {
      std::cout << usage_str;
      exit(0);
    } else if (strcmp(argv[i], "-w") == 0) {
      if ((i + 1) < argc) {
        width = static_cast<uint32_t>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else if (strcmp(argv[i], "-h") == 0) {
      if ((i + 1) < argc) {
        height = static_cast<uint32_t>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else if (strcmp(argv[i], "-d") == 0) {
      if ((i + 1) < argc) {
        depth = static_cast<uint32_t>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else if (strcmp(argv[i], "-ox") == 0) {
      if ((i + 1) < argc) {
        xOffset = static_cast<uint32_t>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else if (strcmp(argv[i], "-oy") == 0) {
      if ((i + 1) < argc) {
        yOffset = static_cast<uint32_t>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else if (strcmp(argv[i], "-oz") == 0) {
      if ((i + 1) < argc) {
        zOffset = static_cast<uint32_t>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else if (strcmp(argv[i], "-w") == 0) {
      if ((i + 1) < argc) {
        warm_up_iterations = static_cast<int>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else if (strcmp(argv[i], "-i") == 0) {
      if ((i + 1) < argc) {
        number_iterations =
            static_cast<uint32_t>(strtoul(argv[i + 1], NULL, 0));
        i++;
      }
    } else {
      std::cout << usage_str;
      exit(-1);
    }
  }
  return 0;
}
