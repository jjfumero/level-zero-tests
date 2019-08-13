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

#include "logging/logging.hpp"
#include "xe_driver.h"

namespace {

TEST(xeInitTests, GivenNoneFlagWhenInitializingDriverThenSuccessIsReturned) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeInit(XE_INIT_FLAG_NONE));
}

TEST(
    xeInitTests,
    GivenDriverWasAlreadyInitializedWhenInitializingDriverThenSuccessIsReturned) {
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeInit(XE_INIT_FLAG_NONE));
  }
}

class xeGetDriverVersionTests : public ::testing::Test {
  void SetUp() override {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeInit(XE_INIT_FLAG_NONE));
  }
};

TEST_F(xeGetDriverVersionTests,
       GivenNullVersionWhenGettingDriverVersionThenSuccessIsReturned) {
  uint32_t version = 0;
  EXPECT_EQ(XE_RESULT_SUCCESS, xeGetDriverVersion(&version));
}

TEST_F(xeGetDriverVersionTests,
       GivenZeroVersionWhenGettingDriverVersionThenNonZeroVersionIsReturned) {
  uint32_t version = 0;
  xeGetDriverVersion(&version);
  LOG_INFO << "Driver version: " << version;
  EXPECT_NE(0u, version);
}

} // namespace
