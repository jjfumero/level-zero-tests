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

#include "test_harness/test_harness.hpp"
#include "gtest/gtest.h"
#include "logging/logging.hpp"
#include "utils/utils.hpp"
#include "ze_api.h"

namespace lzt = level_zero_tests;

namespace level_zero_tests {

void copy_image_from_mem(lzt::ImagePNG32Bit input, ze_image_handle_t output) {

  auto command_list = lzt::create_command_list();
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendImageCopyFromMemory(
                command_list, output, input.raw_data(), nullptr, nullptr));
  lzt::append_barrier(command_list, nullptr, 0, nullptr);
  lzt::close_command_list(command_list);
  auto command_queue = lzt::create_command_queue();
  lzt::execute_command_lists(command_queue, 1, &command_list, nullptr);
  lzt::synchronize(command_queue, UINT32_MAX);
  lzt::destroy_command_queue(command_queue);
  lzt::destroy_command_list(command_list);
}

void copy_image_to_mem(ze_image_handle_t input, lzt::ImagePNG32Bit output) {

  auto command_list = lzt::create_command_list();
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendImageCopyToMemory(
                command_list, output.raw_data(), input, nullptr, nullptr));
  lzt::append_barrier(command_list, nullptr, 0, nullptr);
  lzt::close_command_list(command_list);
  auto command_queue = lzt::create_command_queue();
  lzt::execute_command_lists(command_queue, 1, &command_list, nullptr);
  lzt::synchronize(command_queue, UINT32_MAX);
  lzt::destroy_command_queue(command_queue);
  lzt::destroy_command_list(command_list);
}

void create_ze_image(ze_image_handle_t &image,
                     const ze_image_desc_t *image_descriptor) {
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeImageCreate(lzt::zeDevice::get_instance()->get_device(),
                          image_descriptor, &image));
  EXPECT_NE(nullptr, image);
}

void create_ze_image(ze_image_handle_t &image) {
  ze_image_desc_t descriptor;
  descriptor.version = ZE_IMAGE_DESC_VERSION_CURRENT;

  create_ze_image(image, &descriptor);
}

void destroy_ze_image(ze_image_handle_t image) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeImageDestroy(image));
}

void generate_ze_image_creation_flags_list(
    std::vector<ze_image_flag_t> &image_creation_flags_list) {
  for (auto image_rw_flag_a : lzt::image_creation_rw_flags) {
    for (auto image_rw_flag_b : lzt::image_creation_rw_flags) {
      for (auto image_cached_flag : lzt::image_creation_cached_flags) {
        ze_image_flag_t image_creation_flags =
            static_cast<ze_image_flag_t>(static_cast<int>(image_rw_flag_a) |
                                         static_cast<int>(image_rw_flag_b) |
                                         static_cast<int>(image_cached_flag));
        image_creation_flags_list.push_back(image_creation_flags);
      }
    }
  }
}

#define DEFAULT_WIDTH 128
#define DEFAULT_HEIGHT 128

const ze_image_desc_t zeImageCreateCommon::dflt_ze_image_desc = {
    ZE_IMAGE_DESC_VERSION_CURRENT,
    ZE_IMAGE_FLAG_PROGRAM_READ,
    ZE_IMAGE_TYPE_2D,
    {ZE_IMAGE_FORMAT_LAYOUT_8_8_8_8, ZE_IMAGE_FORMAT_TYPE_UNORM,
     ZE_IMAGE_FORMAT_SWIZZLE_R, ZE_IMAGE_FORMAT_SWIZZLE_G,
     ZE_IMAGE_FORMAT_SWIZZLE_B, ZE_IMAGE_FORMAT_SWIZZLE_A},
    DEFAULT_WIDTH,
    DEFAULT_HEIGHT,
    1,
    0,
    0};

zeImageCreateCommon::zeImageCreateCommon()
    : dflt_host_image_(DEFAULT_WIDTH, DEFAULT_HEIGHT) {
  lzt::generate_ze_image_creation_flags_list(image_creation_flags_list_);
  create_ze_image(dflt_device_image_, &dflt_ze_image_desc);
  create_ze_image(dflt_device_image_2_, &dflt_ze_image_desc);
  write_image_data_pattern(dflt_host_image_, dflt_data_pattern);
}

zeImageCreateCommon::~zeImageCreateCommon() {
  destroy_ze_image(dflt_device_image_);
  destroy_ze_image(dflt_device_image_2_);
}

void print_image_format_descriptor(const ze_image_format_desc_t descriptor) {
  LOG_DEBUG << "   LAYOUT = " << descriptor.layout
            << "   TYPE = " << descriptor.type << "   X = " << descriptor.x
            << "   Y = " << descriptor.y << "   Z = " << descriptor.z
            << "   w = " << descriptor.w;
}

void print_image_descriptor(const ze_image_desc_t descriptor) {
  LOG_DEBUG << "VERSION = " << descriptor.version
            << "   FLAGS = " << descriptor.flags
            << "   TYPE = " << descriptor.type;
  print_image_format_descriptor(descriptor.format);
  LOG_DEBUG << "   WIDTH = " << descriptor.width
            << "   HEIGHT = " << descriptor.height
            << "   DEPTH = " << descriptor.depth
            << "   ARRAYLEVELS = " << descriptor.arraylevels
            << "   MIPLEVELS = " << descriptor.miplevels;
}

ze_image_properties_t
get_ze_image_properties(ze_image_desc_t image_descriptor) {

  ze_image_properties_t image_properties = {
      ZE_IMAGE_PROPERTIES_VERSION_CURRENT};
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeImageGetProperties(lzt::zeDevice::get_instance()->get_device(),
                                 &image_descriptor, &image_properties));

  auto samplerFilterFlagsValid = (image_properties.samplerFilterFlags ==
                                  ZE_IMAGE_SAMPLER_FILTER_FLAGS_NONE) ||
                                 (image_properties.samplerFilterFlags ==
                                  ZE_IMAGE_SAMPLER_FILTER_FLAGS_POINT) ||
                                 (image_properties.samplerFilterFlags ==
                                  ZE_IMAGE_SAMPLER_FILTER_FLAGS_LINEAR);
  EXPECT_TRUE(samplerFilterFlagsValid);

  return image_properties;
}

static inline uint32_t mask_and_shift(int8_t v, uint8_t m, size_t s) {
  return static_cast<uint32_t>(v & m) << s;
}

static inline uint32_t make_pixel(int8_t r, uint8_t r_idx, int8_t g,
                                  uint8_t g_idx, int8_t b, uint8_t b_idx,
                                  int8_t a, uint8_t a_idx) {
  return mask_and_shift(r, 0xff, r_idx) | mask_and_shift(g, 0xff, g_idx) |
         mask_and_shift(b, 0xff, b_idx) | mask_and_shift(a, 0xff, a_idx);
}

static void clip_to_uint8_t(int8_t &sd, int8_t addvalue) {
  int16_t sd16 = sd;
  sd16 += addvalue;
  if ((sd16 > 127) || (sd16 < -128)) {
    sd = 0;
  } else {
    sd = static_cast<int8_t>(sd16);
  }
}

enum RGBA_index {
  X_IDX = 0,
  Y_IDX = 8,
  Z_IDX = 16,
  W_IDX = 24,
  UNKNOWN_IDX = 255
};

static inline uint8_t lookup_idx(ze_image_format_swizzle_t s,
                                 const ze_image_format_desc_t &fmt) {
  if (s == fmt.x) {
    return X_IDX;
  } else if (s == fmt.y) {
    return Y_IDX;
  } else if (s == fmt.z) {
    return Z_IDX;
  } else if (s == fmt.w) {
    return W_IDX;
  } else {
    return UNKNOWN_IDX;
  }
}

static void write_image_data_pattern(lzt::ImagePNG32Bit &image, int8_t dp,
                                     const ze_image_format_desc_t &image_format,
                                     int originX, int originY, int width,
                                     int height) {
  int8_t pixel_r = dp * 1;
  uint8_t r_idx = lookup_idx(ZE_IMAGE_FORMAT_SWIZZLE_R, image_format);
  int8_t pixel_g = dp * 2;
  uint8_t g_idx = lookup_idx(ZE_IMAGE_FORMAT_SWIZZLE_G, image_format);
  int8_t pixel_b = dp * 3;
  uint8_t b_idx = lookup_idx(ZE_IMAGE_FORMAT_SWIZZLE_B, image_format);
  int8_t pixel_a = dp * 4;
  uint8_t a_idx = lookup_idx(ZE_IMAGE_FORMAT_SWIZZLE_A, image_format);

  // FIXME:
  // If any of the above lookups fail, then the rest of this code will
  // not work.  I want to:
  // testing::Test::FAIL() << "Can't find swizzle value in format.";

  for (int y = originY; y < height; y++) {
    for (int x = originX; x < width; x++) {
      uint32_t pixel = make_pixel(pixel_r, r_idx, pixel_g, g_idx, pixel_b,
                                  b_idx, pixel_a, a_idx);
      image.set_pixel(x, y, pixel);
      clip_to_uint8_t(pixel_r, dp * 1);
      clip_to_uint8_t(pixel_g, dp * 2);
      clip_to_uint8_t(pixel_b, dp * 3);
      clip_to_uint8_t(pixel_a, dp * 4);
    }
  }
}

void write_image_data_pattern(lzt::ImagePNG32Bit &image, int8_t dp,
                              const ze_image_format_desc_t &image_format) {
  write_image_data_pattern(image, dp, image_format, 0, 0, image.width(),
                           image.height());
}

void write_image_data_pattern(lzt::ImagePNG32Bit &image, int8_t dp) {
  ze_image_desc_t img_desc = zeImageCreateCommon::dflt_ze_image_desc;

  write_image_data_pattern(image, dp, img_desc.format, 0, 0, image.width(),
                           image.height());
}

static inline uint32_t get_pixel(const uint32_t *image, int x, int y,
                                 int row_width) {
  return image[y * row_width + x];
}

int compare_data_pattern(const lzt::ImagePNG32Bit &imagepng1,
                         const lzt::ImagePNG32Bit &imagepng2, int origin1X,
                         int origin1Y, int width1, int height1, int origin2X,
                         int origin2Y, int width2, int height2) {
  ze_image_desc_t img_desc = zeImageCreateCommon::dflt_ze_image_desc;

  return compare_data_pattern(imagepng1, img_desc.format, imagepng2,
                              img_desc.format, origin1X, origin1Y, width1,
                              height1, origin2X, origin2Y, width2, height2);
}

static void decompose_pixel(uint32_t pixel, int8_t &r, uint8_t r_idx, int8_t &g,
                            uint8_t g_idx, int8_t &b, uint8_t b_idx, int8_t &a,
                            uint8_t a_idx) {
  r = (pixel >> r_idx) & 0xff;
  g = (pixel >> g_idx) & 0xff;
  b = (pixel >> b_idx) & 0xff;
  a = (pixel >> a_idx) & 0xff;
}

// Returns number of errors found, color order for both images are
// define in the image_format parameters:
int compare_data_pattern(const lzt::ImagePNG32Bit &imagepng1,
                         const ze_image_format_desc_t &image1_format,
                         const lzt::ImagePNG32Bit &imagepng2,
                         const ze_image_format_desc_t &image2_format,
                         int origin1X, int origin1Y, int width1, int height1,
                         int origin2X, int origin2Y, int width2, int height2) {
  uint8_t r1_idx = lookup_idx(ZE_IMAGE_FORMAT_SWIZZLE_R, image1_format);
  uint8_t g1_idx = lookup_idx(ZE_IMAGE_FORMAT_SWIZZLE_G, image1_format);
  uint8_t b1_idx = lookup_idx(ZE_IMAGE_FORMAT_SWIZZLE_B, image1_format);
  uint8_t a1_idx = lookup_idx(ZE_IMAGE_FORMAT_SWIZZLE_A, image1_format);
  uint8_t r2_idx = lookup_idx(ZE_IMAGE_FORMAT_SWIZZLE_R, image2_format);
  uint8_t g2_idx = lookup_idx(ZE_IMAGE_FORMAT_SWIZZLE_G, image2_format);
  uint8_t b2_idx = lookup_idx(ZE_IMAGE_FORMAT_SWIZZLE_B, image2_format);
  uint8_t a2_idx = lookup_idx(ZE_IMAGE_FORMAT_SWIZZLE_A, image2_format);

  const bool must_decompose_colors = (r1_idx != r2_idx) || (g1_idx != g2_idx) ||
                                     (b1_idx != b2_idx) || (a1_idx != a2_idx);
  const uint32_t *image1 = imagepng1.raw_data();
  const uint32_t *image2 = imagepng2.raw_data();
  int errCnt = 0, successCnt = 0;
  for (int y1 = origin1Y, y2 = origin2Y;
       (y1 < (origin1Y + height1)) && (y2 < (origin2Y + height2)); y1++, y2++) {
    for (int x1 = origin1X, x2 = origin2X;
         (x1 < (origin1X + width1)) && (x2 < (origin2X + width2)); x1++, x2++) {
      if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0)
        continue;

      uint32_t pixel1 = get_pixel(image1, x1, y1, width1);
      uint32_t pixel2 = get_pixel(image2, x2, y2, width2);
      bool correct;
      if (must_decompose_colors) {
        int8_t r1, g1, b1, a1;
        int8_t r2, g2, b2, a2;
        decompose_pixel(pixel1, r1, r1_idx, g1, g1_idx, b1, b1_idx, a1, a1_idx);
        decompose_pixel(pixel2, r2, r2_idx, g2, g2_idx, b2, b2_idx, a2, a2_idx);
        if ((r1 != r2) || (g1 != g2) || (b1 != b2) || (a1 != a2)) {
          correct = false;
        } else {
          correct = true;
        }
      } else {
        if (pixel1 != pixel2) {
          correct = false;
        } else {
          correct = true;
        }
      }
      if (!correct) {
        LOG_DEBUG << "errCnt: " << errCnt << " successCnt: " << successCnt
                  << " x1: " << x1 << " y1: " << y1 << " x2: " << x2
                  << " y2: " << y2 << " pixel1: 0x" << std::hex << pixel1
                  << " pixel2: 0x" << pixel2;
        errCnt++;
      } else {
        successCnt++;
      }
    }
    return errCnt;
  }
}

int compare_data_pattern(
    const lzt::ImagePNG32Bit &image, const ze_image_region_t *region,
    const lzt::ImagePNG32Bit &expected_fg, // expected foreground
    const lzt::ImagePNG32Bit &expected_bg) {
  const ze_image_region_t full_image_region = {uint32_t(0),
                                               uint32_t(0),
                                               uint32_t(0),
                                               uint32_t(image.width()),
                                               uint32_t(image.height()),
                                               uint32_t(1)};
  if (region == nullptr) {
    region = &full_image_region;
  }
  int errCnt = 0;
  LOG_DEBUG << "region: originX: " << region->originX
            << " originY: " << region->originY
            << " originZ: " << region->originZ << " width: " << region->width
            << " height: " << region->height << std::endl;
  for (unsigned row = 0; row < image.height(); row++) {
    for (unsigned column = 0; column < image.width(); column++) {
      uint32_t pixel = image.get_pixel(column, row);
      const lzt::ImagePNG32Bit *expected_image;
      LOG_DEBUG << "row: " << row << " and column: " << column
                << " corresponds to the ";
      if ((row >= region->originY &&
           row < (region->originY + region->height)) &&
          (column >= region->originX &&
           column < (region->originX + region->width))) {
        // The pixel is expected to be in the foreground image:
        LOG_DEBUG << "foreground." << std::endl;
        expected_image = &expected_fg;
      } else {
        // The pixel is expected to be in the background image:
        LOG_DEBUG << "background." << std::endl;
        expected_image = &expected_bg;
      }
      if (row > expected_image->height() || column > expected_image->width()) {
        LOG_DEBUG << "expected image does not have a pixel for row: " << row
                  << " and column: " << column << std::endl;
        errCnt++;
      } else {
        if (pixel != expected_image->get_pixel(column, row)) {
          LOG_DEBUG << "image's pixel: " << pixel
                    << " does not match expected image's pixel: "
                    << expected_image->get_pixel(column, row)
                    << " for row: " << row << " and column: " << column
                    << std::endl;
          errCnt++;
        }
      }
    }
  }
  return errCnt;
}

}; // namespace level_zero_tests
