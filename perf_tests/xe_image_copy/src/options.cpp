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

  std::string layout = "";
  std::string flags = "";
  std::string type = "";
  std::string format = "";

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
      "set number of iterations")(
      "layout", po::value<string>(&layout),
      " image layout like "
      "8/16/32/8_8/8_8_8_8/16_16/16_16_16_16/32_32/32_32_32_32/10_10_10_2/"
      "11_11_10/5_6_5/5_5_5_1/4_4_4_4/Y8/NV12/YUYV/VYUY/YVYU/UYVY/AYUV/YUAV/"
      "P010/Y410/P012/Y16/P016/Y216/P216/P416")(
      "flags", po::value<string>(&flags),
      "image program flags like READ/WRITE/CACHED/UNCACHED")(
      "type", po::value<string>(&type),
      "Image  type like 1D/2D/3D/1DARRAY/2DARRAY")(
      "format", po::value<string>(&format),
      "image format like UINT/SINT/UNORM/SNORM/FLOAT")(
      "data-validation", po::value<uint32_t>(&data_validation),
      "optional param for validating the copied image is correct or not");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (layout.size() != 0)
    Imagelayout = level_zero_tests::to_layout(layout);
  if (flags.size() != 0)
    Imageflags = level_zero_tests::to_flag(flags);
  if (format.size() != 0)
    Imageformat = level_zero_tests::to_format_type(format);
  if (type.size() != 0)
    Imagetype = level_zero_tests::to_image_type(type);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  } else if (Imagelayout == -1) {
    std::cout << "unknown format layout" << endl;
    std::cout << desc << "\n";
    return 1;
  } else if (Imageflags == -1) {
    std::cout << "unknown image flags" << endl;
    std::cout << desc << "\n";
    return 1;
  } else if (Imageformat == -1) {
    std::cout << "unknown  Imageformat" << endl;
    std::cout << desc << "\n";
    return 1;
  } else if (Imagetype == -1) {
    std::cout << "unknown  Imagetype" << endl;
    std::cout << desc << "\n";
    return 1;
  }

  return 0;
}
