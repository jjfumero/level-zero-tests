/*
 * INTEL CONFIDENTIAL
 *
 * Copyright 2019 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code (Material) are owned by Intel Corporation or its
 * suppliers or licensors.
 *
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors. The Material contains trade secrets and proprietary and
 * confidential information of Intel or its suppliers and licensors.
 * The Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure
 * or delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 */

#include "gtest/gtest.h"

#include "xe_utils/xe_utils.hpp"
#include "xe_test_harness/xe_test_harness.hpp"
#include "logging/logging.hpp"

namespace lzt = level_zero_tests;

#include "xe_copy.h"
#include "xe_image.h"
#include "xe_driver.h"
#include "xe_sampler.h"
#include "image/image.hpp"

namespace {

const auto sampler_address_modes = ::testing::Values(
    XE_SAMPLER_ADDRESS_MODE_NONE, XE_SAMPLER_ADDRESS_MODE_REPEAT,
    XE_SAMPLER_ADDRESS_MODE_CLAMP, XE_SAMPLER_ADDRESS_MODE_MIRROR);

const auto sampler_filter_modes = ::testing::Values(
    XE_SAMPLER_FILTER_MODE_NEAREST, XE_SAMPLER_FILTER_MODE_LINEAR);

class xeDeviceCreateSamplerTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<std::tuple<
          xe_sampler_address_mode_t, xe_sampler_filter_mode_t, xe_bool_t>> {};
TEST_P(xeDeviceCreateSamplerTests,
       GivenSamplerDescriptorWhenCreatingSamplerThenNotNullSamplerIsReturned) {
  xe_sampler_desc_t descriptor;
  descriptor.version = XE_SAMPLER_DESC_VERSION_CURRENT;
  descriptor.addressMode = std::get<0>(GetParam());
  descriptor.filterMode = std::get<1>(GetParam());
  descriptor.isNormalized = std::get<2>(GetParam());

  xe_sampler_handle_t sampler = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeSamplerCreate(lzt::xeDevice::get_instance()->get_device(),
                            &descriptor, &sampler));
  EXPECT_NE(nullptr, sampler);

  EXPECT_EQ(XE_RESULT_SUCCESS, xeSamplerDestroy(sampler));
}

INSTANTIATE_TEST_CASE_P(SamplerCreationCombinations, xeDeviceCreateSamplerTests,
                        ::testing::Combine(sampler_address_modes,
                                           sampler_filter_modes,
                                           ::testing::Values(true, false)));

TEST(xeSamplerTests,
     GivenSamplerWhenPassingAsFunctionArgumentThenSuccessIsReturned) {

  xe_sampler_handle_t sampler = lzt::create_sampler();

  std::string module_name = "xe_sampler.spv";
  xe_module_handle_t module = lzt::create_module(
      lzt::xeDevice::get_instance()->get_device(), module_name);
  std::string func_name = "xe_sampler_noop";

  lzt::FunctionArg arg;
  std::vector<lzt::FunctionArg> args;

  arg.arg_size = sizeof(sampler);
  arg.arg_value = &sampler;
  args.push_back(arg);

  lzt::create_and_execute_function(lzt::xeDevice::get_instance()->get_device(),
                                   module, func_name, 1, args);
  lzt::destroy_module(module);
  lzt::destroy_sampler(sampler);
}

static xe_image_handle_t create_sampler_image(lzt::ImagePNG32Bit png_image,
                                              int height, int width) {
  xe_image_desc_t image_description;
  image_description.format.layout = XE_IMAGE_FORMAT_LAYOUT_8;
  image_description.version = XE_IMAGE_DESC_VERSION_CURRENT;
  image_description.flags = XE_IMAGE_FLAG_PROGRAM_WRITE;
  image_description.type = XE_IMAGE_TYPE_2D;
  image_description.format.type = XE_IMAGE_FORMAT_TYPE_UINT;
  image_description.format.x = XE_IMAGE_FORMAT_SWIZZLE_R;
  image_description.format.y = XE_IMAGE_FORMAT_SWIZZLE_G;
  image_description.format.z = XE_IMAGE_FORMAT_SWIZZLE_B;
  image_description.format.w = XE_IMAGE_FORMAT_SWIZZLE_A;
  image_description.width = width;
  image_description.height = height;
  image_description.depth = 1;
  xe_image_handle_t image = nullptr;

  lzt::create_xe_image(image, &image_description);

  return image;
}

static xe_image_handle_t create_sampler_image(lzt::ImagePNG32Bit png_image) {
  return create_sampler_image(png_image, png_image.height(), png_image.width());
}

TEST(
    xeSamplerTests,
    GivenSamplerWhenPassingAsFunctionArgumentThenOutputMatchesInKernelSampler) {

  lzt::ImagePNG32Bit input("test_input.png");
  int output_width = input.width() / 2;
  int output_height = input.height() / 2;
  lzt::ImagePNG32Bit output_inhost(output_width, output_height);
  lzt::ImagePNG32Bit output_inkernel(output_width, output_height);
  std::string module_name = "xe_sampler.spv";
  xe_module_handle_t module = lzt::create_module(
      lzt::xeDevice::get_instance()->get_device(), module_name);
  std::string func_name_inhost = "xe_sampler_inhost";
  std::string func_name_inkernel = "xe_sampler_inkernel";

  auto sampler = lzt::create_sampler(XE_SAMPLER_ADDRESS_MODE_CLAMP,
                                     XE_SAMPLER_FILTER_MODE_NEAREST, true);
  auto input_xeimage = create_sampler_image(input);
  auto output_xeimage_host =
      create_sampler_image(input, output_height, output_width);
  auto output_xeimage_kernel =
      create_sampler_image(input, output_height, output_width);

  lzt::FunctionArg arg;
  std::vector<lzt::FunctionArg> args_inkernel;
  std::vector<lzt::FunctionArg> args_inhost;

  // copy image data to input xe image
  lzt::copy_image_from_mem(input, input_xeimage);

  // input image arg
  arg.arg_size = sizeof(input_xeimage);
  arg.arg_value = &input_xeimage;
  args_inhost.push_back(arg);
  args_inkernel.push_back(arg);

  // output image arg
  arg.arg_size = sizeof(output_xeimage_host);
  arg.arg_value = &output_xeimage_host;
  args_inhost.push_back(arg);

  arg.arg_size = sizeof(output_xeimage_kernel);
  arg.arg_value = &output_xeimage_kernel;
  args_inkernel.push_back(arg);

  // sampler arg
  arg.arg_size = sizeof(sampler);
  arg.arg_value = &sampler;
  args_inhost.push_back(arg);

  lzt::create_and_execute_function(lzt::xeDevice::get_instance()->get_device(),
                                   module, func_name_inhost, 1, args_inhost);

  lzt::create_and_execute_function(lzt::xeDevice::get_instance()->get_device(),
                                   module, func_name_inkernel, 1,
                                   args_inkernel);

  lzt::copy_image_to_mem(output_xeimage_host, output_inhost);
  lzt::copy_image_to_mem(output_xeimage_kernel, output_inkernel);

  // compare output kernel vs host
  EXPECT_EQ(0, memcmp(output_inhost.raw_data(), output_inkernel.raw_data(),
                      output_inhost.size_in_bytes()));

  EXPECT_EQ(XE_RESULT_SUCCESS, xeSamplerDestroy(sampler));
  lzt::destroy_module(module);
  lzt::destroy_xe_image(input_xeimage);
  lzt::destroy_xe_image(output_xeimage_host);
  lzt::destroy_xe_image(output_xeimage_kernel);
}

} // namespace

// TODO: Test all sampler properties
// TODO: Test sampler on a device using OpenCL C kernels
