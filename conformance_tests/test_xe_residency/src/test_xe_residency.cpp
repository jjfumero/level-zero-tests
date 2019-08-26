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

#include "xe_driver.h"
#include "xe_memory.h"
#include "xe_residency.h"

namespace {

class xeDeviceMakeMemoryResidentTests : public ::testing::Test {
protected:
  void SetUp() override { memory_ = lzt::allocate_device_memory(size_); }

  void TearDown() override {
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeDeviceGroupFreeMem(lzt::get_default_device_group(), memory_));
  }

  void *memory_ = nullptr;
  const size_t size_ = 1024;
};

TEST_F(xeDeviceMakeMemoryResidentTests,
       GivenDeviceMemoryWhenMakingMemoryResidentThenSuccessIsReturned) {
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceMakeMemoryResident(
                lzt::xeDevice::get_instance()->get_device(), memory_, size_));
}

class xeDeviceEvictMemoryTests : public xeDeviceMakeMemoryResidentTests {};

TEST_F(
    xeDeviceEvictMemoryTests,
    GivenResidentDeviceMemoryWhenEvictingResidentMemoryThenSuccessIsReturned) {
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceMakeMemoryResident(
                lzt::xeDevice::get_instance()->get_device(), memory_, size_));
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceEvictMemory(lzt::xeDevice::get_instance()->get_device(),
                                memory_, size_));
}

class xeDeviceMakeImageResidentTests : public lzt::xeImageCreateCommonTests {};

TEST_F(xeDeviceMakeImageResidentTests,
       GivenDeviceImageWhenMakingImageResidentThenSuccessIsReturned) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceMakeImageResident(
                                   lzt::xeDevice::get_instance()->get_device(),
                                   img.dflt_device_image_));
}

class xeDeviceEvictImageTests : public xeDeviceMakeImageResidentTests {};

TEST_F(xeDeviceEvictImageTests,
       GivenResidentDeviceImageWhenEvictingResidentImageThenSuccessIsReturned) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceMakeImageResident(
                                   lzt::xeDevice::get_instance()->get_device(),
                                   img.dflt_device_image_));
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceEvictImage(lzt::xeDevice::get_instance()->get_device(),
                               img.dflt_device_image_));
}

} // namespace

// TODO: How to test residency?
// TODO: Test images
// TODO: Test different memory sizes
// TODO: Test different memory types i.e. Host, Device, Shared
// TODO: Test residency on a device using OpenCL C kernels
