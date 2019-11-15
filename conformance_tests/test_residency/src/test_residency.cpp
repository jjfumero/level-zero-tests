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

#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"
#include "logging/logging.hpp"

namespace lzt = level_zero_tests;

#include "ze_api.h"

namespace {

class zeDeviceMakeMemoryResidentTests : public ::testing::Test {
protected:
  void SetUp() override { memory_ = lzt::allocate_device_memory(size_); }

  void TearDown() override {
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeDriverFreeMem(lzt::get_default_driver(), memory_));
  }

  void *memory_ = nullptr;
  const size_t size_ = 1024;
};

TEST_F(zeDeviceMakeMemoryResidentTests,
       GivenDeviceMemoryWhenMakingMemoryResidentThenSuccessIsReturned) {
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDeviceMakeMemoryResident(
                lzt::zeDevice::get_instance()->get_device(), memory_, size_));
}

class zeDeviceEvictMemoryTests : public zeDeviceMakeMemoryResidentTests {};

TEST_F(
    zeDeviceEvictMemoryTests,
    GivenResidentDeviceMemoryWhenEvictingResidentMemoryThenSuccessIsReturned) {
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDeviceMakeMemoryResident(
                lzt::zeDevice::get_instance()->get_device(), memory_, size_));
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDeviceEvictMemory(lzt::zeDevice::get_instance()->get_device(),
                                memory_, size_));
}

class zeDeviceMakeImageResidentTests : public lzt::zeImageCreateCommonTests {};

TEST_F(zeDeviceMakeImageResidentTests,
       GivenDeviceImageWhenMakingImageResidentThenSuccessIsReturned) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceMakeImageResident(
                                   lzt::zeDevice::get_instance()->get_device(),
                                   img.dflt_device_image_));
}

class zeDeviceEvictImageTests : public zeDeviceMakeImageResidentTests {};

TEST_F(zeDeviceEvictImageTests,
       GivenResidentDeviceImageWhenEvictingResidentImageThenSuccessIsReturned) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceMakeImageResident(
                                   lzt::zeDevice::get_instance()->get_device(),
                                   img.dflt_device_image_));
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDeviceEvictImage(lzt::zeDevice::get_instance()->get_device(),
                               img.dflt_device_image_));
}

} // namespace

// TODO: How to test residency?
// TODO: Test images
// TODO: Test different memory sizes
// TODO: Test different memory types i.e. Host, Device, Shared
// TODO: Test residency on a device using OpenCL C kernels
