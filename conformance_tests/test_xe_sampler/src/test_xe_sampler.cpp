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

namespace cs = compute_samples;

#include "xe_driver.h"
#include "xe_sampler.h"

namespace {

class xeDeviceCreateSamplerTests : public ::testing::Test {};

TEST_F(xeDeviceCreateSamplerTests,
       GivenSamplerDescriptorWhenCreatingSamplerThenNotNullSamplerIsReturned) {
  xe_sampler_desc_t descriptor;
  descriptor.version = XE_SAMPLER_DESC_VERSION_CURRENT;

  xe_sampler_handle_t sampler = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeSamplerCreate(cs::xeDevice::get_instance()->get_device(),
                            &descriptor, &sampler));
  EXPECT_NE(nullptr, sampler);

  EXPECT_EQ(XE_RESULT_SUCCESS, xeSamplerDestroy(sampler));
}

} // namespace

// TODO: Test all sampler properties
// TODO: Test sampler on a device using OpenCL C kernels
