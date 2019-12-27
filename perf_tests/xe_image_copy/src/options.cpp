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

#include "xe_image_copy.h"

int XeImageCopy::parse_command_line(int argc, char **argv) {

  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "width,w", po::value<uint32_t>(&width)->default_value(2048),
      "set image width")("height,h",
                         po::value<uint32_t>(&height)->default_value(2048),
                         "set image height")(
      "depth,d", po::value<uint32_t>(&depth)->default_value(1),
      "set image depth")("offx",
                         po::value<uint32_t>(&xOffset)->default_value(0),
                         "set image xoffset")(
      "offy", po::value<uint32_t>(&yOffset)->default_value(0),
      "set image yoffset")("offz",
                           po::value<uint32_t>(&zOffset)->default_value(0),
                           "set image zoffset")(
      "warmup", po::value<uint32_t>(&warm_up_iterations)->default_value(10),
      "set number of warmup operations")(
      "iter", po::value<uint32_t>(&number_iterations)->default_value(50),
      "set number of iterations");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }

  return 0;
}
