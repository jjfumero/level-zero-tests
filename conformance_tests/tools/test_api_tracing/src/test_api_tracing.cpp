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
#include "test_harness/test_harness.hpp"
#include "ze_api.h"
#include "zet_api.h"

namespace lzt = level_zero_tests;

namespace {

TEST(
    TracingCreateTests,
    GivenValidDeviceAndTracerDescriptionWhenCreatingTracerThenTracerIsNotNull) {
  uint32_t user_data;

  zet_tracer_desc_t tracer_desc;
  tracer_desc.version = ZET_TRACER_DESC_VERSION_CURRENT;
  tracer_desc.pUserData = &user_data;

  zet_tracer_handle_t tracer_handle = lzt::create_tracer_handle(tracer_desc);
  EXPECT_NE(tracer_handle, nullptr);

  lzt::destroy_tracer_handle(tracer_handle);
}

class TracingCreateMultipleTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<uint32_t> {};

TEST_P(TracingCreateMultipleTests,
       GivenExistingTracersWhenCreatingNewTracerThenSuccesIsReturned) {

  std::vector<zet_tracer_handle_t> tracers(GetParam());
  std::vector<zet_tracer_desc_t> descs(GetParam());
  uint32_t *user_data = new uint32_t[GetParam()];

  for (uint32_t i = 0; i < GetParam(); ++i) {
    descs[i].version = ZET_TRACER_DESC_VERSION_CURRENT;
    descs[i].pUserData = &user_data[i];

    tracers[i] = lzt::create_tracer_handle(descs[i]);
    EXPECT_NE(tracers[i], nullptr);
  }

  for (auto tracer : tracers) {
    lzt::destroy_tracer_handle(tracer);
  }
  delete[] user_data;
}

INSTANTIATE_TEST_CASE_P(CreateMultipleTracerTest, TracingCreateMultipleTests,
                        ::testing::Values(1, 10, 100, 1000));

TEST(TracingDestroyTests,
     GivenSingleDisabledTracerWhenDestroyingTracerThenSuccessIsReturned) {
  uint32_t user_data;

  zet_tracer_desc_t tracer_desc;
  tracer_desc.version = ZET_TRACER_DESC_VERSION_CURRENT;
  tracer_desc.pUserData = &user_data;
  zet_tracer_handle_t tracer_handle = lzt::create_tracer_handle(tracer_desc);

  lzt::destroy_tracer_handle(tracer_handle);
}

} // namespace
