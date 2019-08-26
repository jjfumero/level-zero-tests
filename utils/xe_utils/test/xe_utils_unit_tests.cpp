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

#include "xe_utils/xe_utils.hpp"
#include "gtest/gtest.h"

TEST(XeApiVersionToString, XE_API_VERSION_1_0) {
  const xe_api_version_t v = XE_API_VERSION_1_0;
  EXPECT_EQ("1.0", level_zero_tests::to_string(v));
}

TEST(XeResultToString, XE_RESULT_SUCCESS) {
  const xe_result_t r = XE_RESULT_SUCCESS;
  EXPECT_EQ("XE_RESULT_SUCCESS", level_zero_tests::to_string(r));
}

TEST(XeResultToString, XE_RESULT_NOT_READY) {
  const xe_result_t r = XE_RESULT_NOT_READY;
  EXPECT_EQ("XE_RESULT_NOT_READY", level_zero_tests::to_string(r));
}

TEST(XeResultToString, XE_RESULT_ERROR_UNINITIALIZED) {
  const xe_result_t r = XE_RESULT_ERROR_UNINITIALIZED;
  EXPECT_EQ("XE_RESULT_ERROR_UNINITIALIZED", level_zero_tests::to_string(r));
}

TEST(XeResultToString, XE_RESULT_ERROR_DEVICE_LOST) {
  const xe_result_t r = XE_RESULT_ERROR_DEVICE_LOST;
  EXPECT_EQ("XE_RESULT_ERROR_DEVICE_LOST", level_zero_tests::to_string(r));
}

TEST(XeResultToString, XE_RESULT_ERROR_UNSUPPORTED) {
  const xe_result_t r = XE_RESULT_ERROR_UNSUPPORTED;
  EXPECT_EQ("XE_RESULT_ERROR_UNSUPPORTED", level_zero_tests::to_string(r));
}

TEST(XeResultToString, XE_RESULT_ERROR_INVALID_PARAMETER) {
  const xe_result_t r = XE_RESULT_ERROR_INVALID_PARAMETER;
  EXPECT_EQ("XE_RESULT_ERROR_INVALID_PARAMETER",
            level_zero_tests::to_string(r));
}

TEST(XeResultToString, XE_RESULT_ERROR_OUT_OF_HOST_MEMORY) {
  const xe_result_t r = XE_RESULT_ERROR_OUT_OF_HOST_MEMORY;
  EXPECT_EQ("XE_RESULT_ERROR_OUT_OF_HOST_MEMORY",
            level_zero_tests::to_string(r));
}

TEST(XeResultToString, XE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY) {
  const xe_result_t r = XE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY;
  EXPECT_EQ("XE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY",
            level_zero_tests::to_string(r));
}

TEST(XeResultToString, XE_RESULT_ERROR_MODULE_BUILD_FAILURE) {
  const xe_result_t r = XE_RESULT_ERROR_MODULE_BUILD_FAILURE;
  EXPECT_EQ("XE_RESULT_ERROR_MODULE_BUILD_FAILURE",
            level_zero_tests::to_string(r));
}

TEST(XeResultToString, XE_RESULT_ERROR_UNKNOWN) {
  const xe_result_t r = XE_RESULT_ERROR_UNKNOWN;
  EXPECT_EQ("XE_RESULT_ERROR_UNKNOWN", level_zero_tests::to_string(r));
}

TEST(XeResultToString, InvalidValue) {
  const xe_result_t r = static_cast<xe_result_t>(10);
  EXPECT_THROW(level_zero_tests::to_string(r), std::runtime_error);
}

TEST(XeCommandQueueDescVersionToString, XE_COMMAND_QUEUE_DESC_VERSION_CURRENT) {
  const xe_command_queue_desc_version_t d =
      XE_COMMAND_QUEUE_DESC_VERSION_CURRENT;
  EXPECT_EQ("XE_COMMAND_QUEUE_DESC_VERSION_CURRENT",
            level_zero_tests::to_string(d));
}

TEST(XeCommandQueueDescVersionToString, NonCurrentDescVersion) {
  const xe_command_queue_desc_version_t d =
      static_cast<xe_command_queue_desc_version_t>(
          static_cast<uint32_t>(XE_COMMAND_QUEUE_DESC_VERSION_CURRENT + 1));
  EXPECT_EQ("NON-CURRENT XE_COMMAND_QUEUE_DESC_VERSION",
            level_zero_tests::to_string(d));
}

TEST(XeCommandQueueDescFlagsToString, XE_COMMAND_QUEUE_FLAG_COPY_ONLY) {
  const xe_command_queue_flag_t d = XE_COMMAND_QUEUE_FLAG_COPY_ONLY;
  EXPECT_EQ("XE_COMMAND_QUEUE_FLAG_COPY_ONLY", level_zero_tests::to_string(d));
}

TEST(XeCommandQueueDescFlagsToString, InvalidValue) {
  const xe_command_queue_flag_t d = static_cast<xe_command_queue_flag_t>(
      static_cast<uint32_t>(XE_COMMAND_QUEUE_FLAG_COPY_ONLY +
                            XE_COMMAND_QUEUE_FLAG_LOGICAL_ONLY));
  EXPECT_EQ("Unknown xe_command_queue_flag_t value: " +
                std::to_string(static_cast<int>(d)),
            level_zero_tests::to_string(d));
}

TEST(XeCommandQueueDescModeToString, XE_COMMAND_QUEUE_MODE_SYNCHRONOUS) {
  const xe_command_queue_mode_t d = XE_COMMAND_QUEUE_MODE_SYNCHRONOUS;
  EXPECT_EQ("XE_COMMAND_QUEUE_MODE_SYNCHRONOUS",
            level_zero_tests::to_string(d));
}

TEST(XeCommandQueueDescModeToString, InvalidValue) {
  const xe_command_queue_mode_t d = static_cast<xe_command_queue_mode_t>(
      static_cast<uint32_t>(XE_COMMAND_QUEUE_MODE_SYNCHRONOUS +
                            XE_COMMAND_QUEUE_MODE_ASYNCHRONOUS));
  EXPECT_EQ("Unknown xe_command_queue_mode_t value: " +
                std::to_string(static_cast<int>(d)),
            level_zero_tests::to_string(d));
}

TEST(XeCommandQueueDescPriorityToString, XE_COMMAND_QUEUE_PRIORITY_LOW) {
  const xe_command_queue_priority_t d = XE_COMMAND_QUEUE_PRIORITY_LOW;
  EXPECT_EQ("XE_COMMAND_QUEUE_PRIORITY_LOW", level_zero_tests::to_string(d));
}

TEST(XeCommandQueueDescPriorityToString, InvalidValue) {
  const xe_command_queue_priority_t d =
      static_cast<xe_command_queue_priority_t>(static_cast<uint32_t>(
          XE_COMMAND_QUEUE_PRIORITY_LOW + XE_COMMAND_QUEUE_PRIORITY_HIGH));
  EXPECT_EQ("Unknown xe_command_queue_priority_t value: " +
                std::to_string(static_cast<int>(d)),
            level_zero_tests::to_string(d));
}

TEST(XeImageDescVersionToString, XE_IMAGE_DESC_VERSION_CURRENT) {
  const xe_image_desc_version_t v = XE_IMAGE_DESC_VERSION_CURRENT;
  EXPECT_EQ("XE_IMAGE_DESC_VERSION_CURRENT", level_zero_tests::to_string(v));
}

TEST(XeImageDescVersionToString, NonCurrentDescVersion) {
  const xe_image_desc_version_t d = static_cast<xe_image_desc_version_t>(
      static_cast<uint32_t>(XE_IMAGE_DESC_VERSION_CURRENT + 1));
  EXPECT_EQ("NON-CURRENT XE_IMAGE_DESC_VERSION",
            level_zero_tests::to_string(d));
}

TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_8) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_8;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_8", level_zero_tests::to_string(l));
}

TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_16) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_16;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_16", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_32) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_32;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_32", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_8_8) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_8_8;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_8_8", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_8_8_8_8) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_8_8_8_8;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_8_8_8_8", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_16_16) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_16_16;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_16_16", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_16_16_16_16) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_16_16_16_16;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_16_16_16_16",
            level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_32_32) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_32_32;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_32_32", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_32_32_32_32) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_32_32_32_32;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_32_32_32_32",
            level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_10_10_10_2) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_10_10_10_2;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_10_10_10_2",
            level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_11_11_10) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_11_11_10;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_11_11_10", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_5_6_5) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_5_6_5;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_5_6_5", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_5_5_5_1) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_5_5_5_1;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_5_5_5_1", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_4_4_4_4) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_4_4_4_4;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_4_4_4_4", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_Y8) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_Y8;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_Y8", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_NV12) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_NV12;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_NV12", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_YUYV) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_YUYV;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_YUYV", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_VYUY) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_VYUY;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_VYUY", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_YVYU) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_YVYU;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_YVYU", level_zero_tests::to_string(l));
}
TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_UYVY) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_UYVY;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_UYVY", level_zero_tests::to_string(l));
}

TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_AYUV) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_AYUV;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_AYUV", level_zero_tests::to_string(l));
}

TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_YUAV) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_YUAV;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_YUAV", level_zero_tests::to_string(l));
}

TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_P010) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_P010;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_P010", level_zero_tests::to_string(l));
}

TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_Y410) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_Y410;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_Y410", level_zero_tests::to_string(l));
}

TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_P012) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_P012;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_P012", level_zero_tests::to_string(l));
}

TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_Y16) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_Y16;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_Y16", level_zero_tests::to_string(l));
}

TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_P016) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_P016;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_P016", level_zero_tests::to_string(l));
}

TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_Y216) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_Y216;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_Y216", level_zero_tests::to_string(l));
}

TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_P216) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_P216;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_P216", level_zero_tests::to_string(l));
}

TEST(XeImageFormatLayoutToString, XE_IMAGE_FORMAT_LAYOUT_P416) {
  const xe_image_format_layout_t l = XE_IMAGE_FORMAT_LAYOUT_P416;
  EXPECT_EQ("XE_IMAGE_FORMAT_LAYOUT_P416", level_zero_tests::to_string(l));
}

TEST(XeImageFormatLayoutToString, InvalidValue) {
  const xe_image_format_layout_t l = static_cast<xe_image_format_layout_t>(
      static_cast<uint32_t>(XE_IMAGE_FORMAT_LAYOUT_P416 + 1));
  EXPECT_EQ("Unknown xe_image_format_layout_t value: " +
                std::to_string(static_cast<int>(l)),
            level_zero_tests::to_string(l));
}

TEST(XeImageFormatTypeToString, XE_IMAGE_FORMAT_TYPE_UINT) {
  const xe_image_format_type_t t = XE_IMAGE_FORMAT_TYPE_UINT;
  EXPECT_EQ("XE_IMAGE_FORMAT_TYPE_UINT", level_zero_tests::to_string(t));
}

TEST(XeImageFormatTypeToString, XE_IMAGE_FORMAT_TYPE_SINT) {
  const xe_image_format_type_t t = XE_IMAGE_FORMAT_TYPE_SINT;
  EXPECT_EQ("XE_IMAGE_FORMAT_TYPE_SINT", level_zero_tests::to_string(t));
}

TEST(XeImageFormatTypeToString, XE_IMAGE_FORMAT_TYPE_UNORM) {
  const xe_image_format_type_t t = XE_IMAGE_FORMAT_TYPE_UNORM;
  EXPECT_EQ("XE_IMAGE_FORMAT_TYPE_UNORM", level_zero_tests::to_string(t));
}

TEST(XeImageFormatTypeToString, XE_IMAGE_FORMAT_TYPE_SNORM) {
  const xe_image_format_type_t t = XE_IMAGE_FORMAT_TYPE_SNORM;
  EXPECT_EQ("XE_IMAGE_FORMAT_TYPE_SNORM", level_zero_tests::to_string(t));
}

TEST(XeImageFormatTypeToString, XE_IMAGE_FORMAT_TYPE_FLOAT) {
  const xe_image_format_type_t t = XE_IMAGE_FORMAT_TYPE_FLOAT;
  EXPECT_EQ("XE_IMAGE_FORMAT_TYPE_FLOAT", level_zero_tests::to_string(t));
}

TEST(XeImageFormatTypeToString, InvalidValue) {
  const xe_image_format_type_t t = static_cast<xe_image_format_type_t>(
      static_cast<uint32_t>(XE_IMAGE_FORMAT_TYPE_FLOAT + 1));
  EXPECT_EQ("Unknown xe_image_format_type_t value: " +
                std::to_string(static_cast<int>(t)),
            level_zero_tests::to_string(t));
}

TEST(XeImageFormatSwizzleToString, XE_IMAGE_FORMAT_SWIZZLE_R) {
  const xe_image_format_swizzle_t t = XE_IMAGE_FORMAT_SWIZZLE_R;
  EXPECT_EQ("XE_IMAGE_FORMAT_SWIZZLE_R", level_zero_tests::to_string(t));
}

TEST(XeImageFormatSwizzleToString, XE_IMAGE_FORMAT_SWIZZLE_G) {
  const xe_image_format_swizzle_t t = XE_IMAGE_FORMAT_SWIZZLE_G;
  EXPECT_EQ("XE_IMAGE_FORMAT_SWIZZLE_G", level_zero_tests::to_string(t));
}

TEST(XeImageFormatSwizzleToString, XE_IMAGE_FORMAT_SWIZZLE_B) {
  const xe_image_format_swizzle_t t = XE_IMAGE_FORMAT_SWIZZLE_B;
  EXPECT_EQ("XE_IMAGE_FORMAT_SWIZZLE_B", level_zero_tests::to_string(t));
}

TEST(XeImageFormatSwizzleToString, XE_IMAGE_FORMAT_SWIZZLE_A) {
  const xe_image_format_swizzle_t t = XE_IMAGE_FORMAT_SWIZZLE_A;
  EXPECT_EQ("XE_IMAGE_FORMAT_SWIZZLE_A", level_zero_tests::to_string(t));
}

TEST(XeImageFormatSwizzleToString, XE_IMAGE_FORMAT_SWIZZLE_0) {
  const xe_image_format_swizzle_t t = XE_IMAGE_FORMAT_SWIZZLE_0;
  EXPECT_EQ("XE_IMAGE_FORMAT_SWIZZLE_0", level_zero_tests::to_string(t));
}

TEST(XeImageFormatSwizzleToString, XE_IMAGE_FORMAT_SWIZZLE_1) {
  const xe_image_format_swizzle_t t = XE_IMAGE_FORMAT_SWIZZLE_1;
  EXPECT_EQ("XE_IMAGE_FORMAT_SWIZZLE_1", level_zero_tests::to_string(t));
}

TEST(XeImageFormatSwizzleToString, XE_IMAGE_FORMAT_SWIZZLE_X) {
  const xe_image_format_swizzle_t t = XE_IMAGE_FORMAT_SWIZZLE_X;
  EXPECT_EQ("XE_IMAGE_FORMAT_SWIZZLE_X", level_zero_tests::to_string(t));
}

TEST(XeImageFormatSwizzleToString, InvalidValue) {
  const xe_image_format_swizzle_t s = static_cast<xe_image_format_swizzle_t>(
      static_cast<uint32_t>(XE_IMAGE_FORMAT_SWIZZLE_X + 1));
  EXPECT_EQ("Unknown xe_image_format_swizzle_t value: " +
                std::to_string(static_cast<int>(s)),
            level_zero_tests::to_string(s));
}

TEST(XeImageFlagToString, XE_IMAGE_FLAG_PROGRAM_READ) {
  const xe_image_flag_t f = XE_IMAGE_FLAG_PROGRAM_READ;
  EXPECT_EQ("|XE_IMAGE_FLAG_PROGRAM_READ|", level_zero_tests::to_string(f));
}

TEST(XeImageFlagToString, XE_IMAGE_FLAG_PROGRAM_WRITE) {
  const xe_image_flag_t f = XE_IMAGE_FLAG_PROGRAM_WRITE;
  EXPECT_EQ("|XE_IMAGE_FLAG_PROGRAM_WRITE|", level_zero_tests::to_string(f));
}

TEST(XeImageFlagToString, XE_IMAGE_FLAG_BIAS_CACHED) {
  const xe_image_flag_t f = XE_IMAGE_FLAG_BIAS_CACHED;
  EXPECT_EQ("|XE_IMAGE_FLAG_BIAS_CACHED|", level_zero_tests::to_string(f));
}

TEST(XeImageFlagToString, XE_IMAGE_FLAG_BIAS_UNCACHED) {
  const xe_image_flag_t f = XE_IMAGE_FLAG_BIAS_UNCACHED;
  EXPECT_EQ("|XE_IMAGE_FLAG_BIAS_UNCACHED|", level_zero_tests::to_string(f));
}

TEST(XeImageFlagToString, XE_IMAGE_FLAG_PROGRAM_READ_WRITE) {
  const xe_image_flag_t f = static_cast<xe_image_flag_t>(static_cast<uint32_t>(
      XE_IMAGE_FLAG_PROGRAM_READ | XE_IMAGE_FLAG_PROGRAM_WRITE));
  EXPECT_EQ("|XE_IMAGE_FLAG_PROGRAM_READ||XE_IMAGE_FLAG_PROGRAM_WRITE|",
            level_zero_tests::to_string(f));
}

TEST(XeImageFlagToString, XE_IMAGE_FLAG_PROGRAM_READ_CACHED) {
  const xe_image_flag_t f = static_cast<xe_image_flag_t>(static_cast<uint32_t>(
      XE_IMAGE_FLAG_PROGRAM_READ | XE_IMAGE_FLAG_BIAS_CACHED));
  EXPECT_EQ("|XE_IMAGE_FLAG_PROGRAM_READ||XE_IMAGE_FLAG_BIAS_CACHED|",
            level_zero_tests::to_string(f));
}

TEST(XeImageFlagToString, XE_IMAGE_FLAG_PROGRAM_READ_UNCACHED) {
  const xe_image_flag_t f = static_cast<xe_image_flag_t>(static_cast<uint32_t>(
      XE_IMAGE_FLAG_PROGRAM_READ | XE_IMAGE_FLAG_BIAS_UNCACHED));
  EXPECT_EQ("|XE_IMAGE_FLAG_PROGRAM_READ||XE_IMAGE_FLAG_BIAS_UNCACHED|",
            level_zero_tests::to_string(f));
}

TEST(XeImageFlagToString, XE_IMAGE_FLAG_PROGRAM_WRITE_CACHED) {
  const xe_image_flag_t f = static_cast<xe_image_flag_t>(static_cast<uint32_t>(
      XE_IMAGE_FLAG_PROGRAM_WRITE | XE_IMAGE_FLAG_BIAS_CACHED));
  EXPECT_EQ("|XE_IMAGE_FLAG_PROGRAM_WRITE||XE_IMAGE_FLAG_BIAS_CACHED|",
            level_zero_tests::to_string(f));
}

TEST(XeImageFlagToString, XE_IMAGE_FLAG_PROGRAM_WRITE_UNCACHED) {
  const xe_image_flag_t f = static_cast<xe_image_flag_t>(static_cast<uint32_t>(
      XE_IMAGE_FLAG_PROGRAM_WRITE | XE_IMAGE_FLAG_BIAS_UNCACHED));
  EXPECT_EQ("|XE_IMAGE_FLAG_PROGRAM_WRITE||XE_IMAGE_FLAG_BIAS_UNCACHED|",
            level_zero_tests::to_string(f));
}

TEST(XeImageFlagToString, XE_IMAGE_FLAG_PROGRAM_READ_WRITE_CACHED) {
  const xe_image_flag_t f = static_cast<xe_image_flag_t>(static_cast<uint32_t>(
      XE_IMAGE_FLAG_PROGRAM_READ | XE_IMAGE_FLAG_PROGRAM_WRITE |
      XE_IMAGE_FLAG_BIAS_CACHED));
  EXPECT_EQ("|XE_IMAGE_FLAG_PROGRAM_READ||XE_IMAGE_FLAG_PROGRAM_WRITE||XE_"
            "IMAGE_FLAG_BIAS_CACHED|",
            level_zero_tests::to_string(f));
}

TEST(XeImageFlagToString, XE_IMAGE_FLAG_PROGRAM_READ_WRITE_UNCACHED) {
  const xe_image_flag_t f = static_cast<xe_image_flag_t>(static_cast<uint32_t>(
      XE_IMAGE_FLAG_PROGRAM_READ | XE_IMAGE_FLAG_PROGRAM_WRITE |
      XE_IMAGE_FLAG_BIAS_UNCACHED));
  EXPECT_EQ("|XE_IMAGE_FLAG_PROGRAM_READ||XE_IMAGE_FLAG_PROGRAM_WRITE||XE_"
            "IMAGE_FLAG_BIAS_UNCACHED|",
            level_zero_tests::to_string(f));
}

TEST(XeImageTypeToString, XE_IMAGE_TYPE_1D) {
  const xe_image_type_t t = XE_IMAGE_TYPE_1D;
  EXPECT_EQ("XE_IMAGE_TYPE_1D", level_zero_tests::to_string(t));
}

TEST(XeImageTypeToString, XE_IMAGE_TYPE_2D) {
  const xe_image_type_t t = XE_IMAGE_TYPE_2D;
  EXPECT_EQ("XE_IMAGE_TYPE_2D", level_zero_tests::to_string(t));
}

TEST(XeImageTypeToString, XE_IMAGE_TYPE_3D) {
  const xe_image_type_t t = XE_IMAGE_TYPE_3D;
  EXPECT_EQ("XE_IMAGE_TYPE_3D", level_zero_tests::to_string(t));
}

TEST(XeImageTypeToString, XE_IMAGE_TYPE_1DARRAY) {
  const xe_image_type_t t = XE_IMAGE_TYPE_1DARRAY;
  EXPECT_EQ("XE_IMAGE_TYPE_1DARRAY", level_zero_tests::to_string(t));
}
TEST(XeImageTypeToString, XE_IMAGE_TYPE_2DARRAY) {
  const xe_image_type_t t = XE_IMAGE_TYPE_2DARRAY;
  EXPECT_EQ("XE_IMAGE_TYPE_2DARRAY", level_zero_tests::to_string(t));
}

TEST(XeImageTypeToString, InvalidValue) {
  const xe_image_type_t t =
      static_cast<xe_image_type_t>(static_cast<uint32_t>(XE_IMAGE_TYPE_3D + 1));
  EXPECT_EQ("Unknown xe_image_type_t value: " +
                std::to_string(static_cast<int>(t)),
            level_zero_tests::to_string(t));
}
