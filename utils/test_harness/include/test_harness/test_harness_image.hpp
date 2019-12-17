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

#ifndef level_zero_tests_ZE_TEST_HARNESS_IMAGE_HPP
#define level_zero_tests_ZE_TEST_HARNESS_IMAGE_HPP

#include "test_harness_device.hpp"
#include "ze_api.h"
#include "gtest/gtest.h"
#include "utils/utils.hpp"
#include "image/image.hpp"

namespace level_zero_tests {

const std::vector<ze_image_flag_t> image_creation_rw_flags = {
    ZE_IMAGE_FLAG_PROGRAM_READ, ZE_IMAGE_FLAG_PROGRAM_WRITE};

const std::vector<ze_image_flag_t> image_creation_cached_flags = {
    ZE_IMAGE_FLAG_BIAS_CACHED, ZE_IMAGE_FLAG_BIAS_UNCACHED};

const std::vector<size_t> image_widths = {0, 1920};

const std::vector<size_t> image_heights = {0, 1080};

const std::vector<size_t> image_depths = {0, 8};

const std::vector<size_t> image_array_levels = {0, 3};

const auto image_format_types =
    ::testing::Values(ZE_IMAGE_FORMAT_TYPE_UINT, ZE_IMAGE_FORMAT_TYPE_SINT,
                      ZE_IMAGE_FORMAT_TYPE_UNORM, ZE_IMAGE_FORMAT_TYPE_SNORM,
                      ZE_IMAGE_FORMAT_TYPE_FLOAT);

const auto image_format_1d_swizzle_layouts =
    ::testing::Values(ZE_IMAGE_FORMAT_LAYOUT_8, ZE_IMAGE_FORMAT_LAYOUT_16,
                      ZE_IMAGE_FORMAT_LAYOUT_32);

const auto image_format_2d_swizzle_layouts =
    ::testing::Values(ZE_IMAGE_FORMAT_LAYOUT_8_8, ZE_IMAGE_FORMAT_LAYOUT_16_16,
                      ZE_IMAGE_FORMAT_LAYOUT_32_32);

const auto image_format_3d_swizzle_layouts = ::testing::Values(
    ZE_IMAGE_FORMAT_LAYOUT_11_11_10, ZE_IMAGE_FORMAT_LAYOUT_5_6_5);

const auto image_format_4d_swizzle_layouts = ::testing::Values(
    ZE_IMAGE_FORMAT_LAYOUT_8_8_8_8, ZE_IMAGE_FORMAT_LAYOUT_16_16_16_16,
    ZE_IMAGE_FORMAT_LAYOUT_32_32_32_32, ZE_IMAGE_FORMAT_LAYOUT_10_10_10_2,
    ZE_IMAGE_FORMAT_LAYOUT_5_5_5_1, ZE_IMAGE_FORMAT_LAYOUT_4_4_4_4);

const auto image_format_media_layouts =
    ::testing::Values(ZE_IMAGE_FORMAT_LAYOUT_Y8, ZE_IMAGE_FORMAT_LAYOUT_NV12,
                      ZE_IMAGE_FORMAT_LAYOUT_YUYV, ZE_IMAGE_FORMAT_LAYOUT_VYUY,
                      ZE_IMAGE_FORMAT_LAYOUT_YVYU, ZE_IMAGE_FORMAT_LAYOUT_UYVY,
                      ZE_IMAGE_FORMAT_LAYOUT_AYUV, ZE_IMAGE_FORMAT_LAYOUT_YUAV,
                      ZE_IMAGE_FORMAT_LAYOUT_P010, ZE_IMAGE_FORMAT_LAYOUT_Y410,
                      ZE_IMAGE_FORMAT_LAYOUT_P012, ZE_IMAGE_FORMAT_LAYOUT_Y16,
                      ZE_IMAGE_FORMAT_LAYOUT_P016, ZE_IMAGE_FORMAT_LAYOUT_Y216,
                      ZE_IMAGE_FORMAT_LAYOUT_P216, ZE_IMAGE_FORMAT_LAYOUT_P416);

const std::vector<ze_image_format_swizzle_t> image_format_swizzles = {
    ZE_IMAGE_FORMAT_SWIZZLE_R};

const std::vector<ze_image_format_swizzle_t> image_format_swizzles_all = {
    ZE_IMAGE_FORMAT_SWIZZLE_R, ZE_IMAGE_FORMAT_SWIZZLE_G,
    ZE_IMAGE_FORMAT_SWIZZLE_B, ZE_IMAGE_FORMAT_SWIZZLE_A,
    ZE_IMAGE_FORMAT_SWIZZLE_0, ZE_IMAGE_FORMAT_SWIZZLE_1,
    ZE_IMAGE_FORMAT_SWIZZLE_X};

void print_image_format_descriptor(const ze_image_format_desc_t descriptor);
void print_image_descriptor(const ze_image_desc_t descriptor);
void generate_ze_image_creation_flags_list(
    std::vector<ze_image_flag_t> &image_creation_flags_list);

void create_ze_image(ze_image_handle_t &image);
void create_ze_image(ze_image_handle_t &image,
                     ze_image_desc_t *image_descriptor);
void destroy_ze_image(ze_image_handle_t image);

ze_image_properties_t get_ze_image_properties(ze_image_desc_t image_descriptor);

void copy_image_from_mem(lzt::ImagePNG32Bit input, ze_image_handle_t output);
void copy_image_to_mem(ze_image_handle_t input, lzt::ImagePNG32Bit output);

class zeImageCreateCommon {
public:
  zeImageCreateCommon();
  ~zeImageCreateCommon();
  ze_image_desc_t get_dflt_ze_image_desc(void) const;

  std::vector<ze_image_flag_t> image_creation_flags_list_;
  level_zero_tests::ImagePNG32Bit dflt_host_image_;
  ze_image_handle_t dflt_device_image_ = nullptr;
  ze_image_handle_t dflt_device_image_2_ = nullptr;
};

class zeImageCreateCommonTests : public ::testing::Test {
protected:
  zeImageCreateCommon img;
};

// write_image_data_pattern() writes the image in the default color order,
// that I define here:
// a bits  0 ...  7
// b bits  8 ... 15
// g bits 16 ... 23
// r bits 24 ... 31
void write_image_data_pattern(lzt::ImagePNG32Bit &image, int8_t dp);
// The following uses arbitrary color order as defined in image_format:
void write_image_data_pattern(lzt::ImagePNG32Bit &image, int8_t dp,
                              const ze_image_format_desc_t &image_format);

// Returns number of errors found, assumes default color order:
int compare_data_pattern(const lzt::ImagePNG32Bit &imagepng1,
                         const lzt::ImagePNG32Bit &imagepng2, int origin1X,
                         int origin1Y, int width1, int height1, int origin2X,
                         int origin2Y, int width2, int height2);
// Returns number of errors found, color order for both images are
// define in the image_format parameters:
int compare_data_pattern(const lzt::ImagePNG32Bit &imagepng1,
                         const ze_image_format_desc_t &image1_format,
                         const lzt::ImagePNG32Bit &imagepng2,
                         const ze_image_format_desc_t &image2_format,
                         int origin1X, int origin1Y, int width1, int height1,
                         int origin2X, int origin2Y, int width2, int height2);

}; // namespace level_zero_tests

#endif
