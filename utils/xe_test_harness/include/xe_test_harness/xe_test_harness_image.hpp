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

#ifndef level_zero_tests_XE_TEST_HARNESS_IMAGE_HPP
#define level_zero_tests_XE_TEST_HARNESS_IMAGE_HPP

#include "xe_test_harness_device.hpp"
#include "xe_image.h"
#include "gtest/gtest.h"
#include "xe_utils/xe_utils.hpp"
#include "image/image.hpp"

namespace level_zero_tests {

const std::vector<xe_image_flag_t> image_creation_rw_flags = {
    XE_IMAGE_FLAG_PROGRAM_READ, XE_IMAGE_FLAG_PROGRAM_WRITE};

const std::vector<xe_image_flag_t> image_creation_cached_flags = {
    XE_IMAGE_FLAG_BIAS_CACHED, XE_IMAGE_FLAG_BIAS_UNCACHED};

const std::vector<size_t> image_widths = {0, 1920};

const std::vector<size_t> image_heights = {0, 1080};

const std::vector<size_t> image_depths = {0, 8};

const auto image_array_levels = ::testing::Values(0, 3);

const auto image_format_types =
    ::testing::Values(XE_IMAGE_FORMAT_TYPE_UINT, XE_IMAGE_FORMAT_TYPE_SINT,
                      XE_IMAGE_FORMAT_TYPE_UNORM, XE_IMAGE_FORMAT_TYPE_SNORM,
                      XE_IMAGE_FORMAT_TYPE_FLOAT);

const auto image_format_1d_swizzle_layouts =
    ::testing::Values(XE_IMAGE_FORMAT_LAYOUT_8, XE_IMAGE_FORMAT_LAYOUT_16,
                      XE_IMAGE_FORMAT_LAYOUT_32);

const auto image_format_2d_swizzle_layouts =
    ::testing::Values(XE_IMAGE_FORMAT_LAYOUT_8_8, XE_IMAGE_FORMAT_LAYOUT_16_16,
                      XE_IMAGE_FORMAT_LAYOUT_32_32);

const auto image_format_3d_swizzle_layouts = ::testing::Values(
    XE_IMAGE_FORMAT_LAYOUT_11_11_10, XE_IMAGE_FORMAT_LAYOUT_5_6_5);

const auto image_format_4d_swizzle_layouts = ::testing::Values(
    XE_IMAGE_FORMAT_LAYOUT_8_8_8_8, XE_IMAGE_FORMAT_LAYOUT_16_16_16_16,
    XE_IMAGE_FORMAT_LAYOUT_32_32_32_32, XE_IMAGE_FORMAT_LAYOUT_10_10_10_2,
    XE_IMAGE_FORMAT_LAYOUT_5_5_5_1, XE_IMAGE_FORMAT_LAYOUT_4_4_4_4);

const auto image_format_media_layouts =
    ::testing::Values(XE_IMAGE_FORMAT_LAYOUT_Y8, XE_IMAGE_FORMAT_LAYOUT_NV12,
                      XE_IMAGE_FORMAT_LAYOUT_YUYV, XE_IMAGE_FORMAT_LAYOUT_VYUY,
                      XE_IMAGE_FORMAT_LAYOUT_YVYU, XE_IMAGE_FORMAT_LAYOUT_UYVY,
                      XE_IMAGE_FORMAT_LAYOUT_AYUV, XE_IMAGE_FORMAT_LAYOUT_YUAV,
                      XE_IMAGE_FORMAT_LAYOUT_P010, XE_IMAGE_FORMAT_LAYOUT_Y410,
                      XE_IMAGE_FORMAT_LAYOUT_P012, XE_IMAGE_FORMAT_LAYOUT_Y16,
                      XE_IMAGE_FORMAT_LAYOUT_P016, XE_IMAGE_FORMAT_LAYOUT_Y216,
                      XE_IMAGE_FORMAT_LAYOUT_P216, XE_IMAGE_FORMAT_LAYOUT_P416);

const auto image_format_swizzles =
    ::testing::Values(XE_IMAGE_FORMAT_SWIZZLE_R, XE_IMAGE_FORMAT_SWIZZLE_G,
                      XE_IMAGE_FORMAT_SWIZZLE_B, XE_IMAGE_FORMAT_SWIZZLE_A,
                      XE_IMAGE_FORMAT_SWIZZLE_0, XE_IMAGE_FORMAT_SWIZZLE_1,
                      XE_IMAGE_FORMAT_SWIZZLE_X);

void print_image_format_descriptor(const xe_image_format_desc_t descriptor);
void print_image_descriptor(const xe_image_desc_t descriptor);
void generate_xe_image_creation_flags_list(
    std::vector<xe_image_flag_t> &image_creation_flags_list);

void create_xe_image(xe_image_handle_t &image);
void create_xe_image(xe_image_handle_t &image,
                     xe_image_desc_t *image_descriptor);
void destroy_xe_image(xe_image_handle_t image);

xe_image_properties_t get_xe_image_properties(xe_image_desc_t image_descriptor);

class xeImageCreateCommon {
public:
  xeImageCreateCommon();
  ~xeImageCreateCommon();

  std::vector<xe_image_flag_t> image_creation_flags_list_;
  level_zero_tests::ImagePNG32Bit dflt_host_image_;
  xe_image_handle_t dflt_device_image_ = nullptr;
  xe_image_handle_t dflt_device_image_2_ = nullptr;
};

class xeImageCreateCommonTests : public ::testing::Test {
protected:
  xeImageCreateCommon img;
};
}; // namespace level_zero_tests
#endif
