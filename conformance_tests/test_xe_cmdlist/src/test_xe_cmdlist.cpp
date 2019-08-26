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

namespace {

class xeCommandListCreateTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<xe_command_list_flag_t> {};

TEST_P(
    xeCommandListCreateTests,
    GivenValidDeviceAndCommandListDescriptorWhenCreatingCommandListThenNotNullCommandListIsReturned) {
  xe_command_list_desc_t descriptor;
  descriptor.version = XE_COMMAND_LIST_DESC_VERSION_CURRENT;
  descriptor.flags = GetParam();

  xe_command_list_handle_t command_list = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListCreate(lzt::xeDevice::get_instance()->get_device(),
                                &descriptor, &command_list));
  EXPECT_NE(nullptr, command_list);

  lzt::destroy_command_list(command_list);
}

INSTANTIATE_TEST_CASE_P(
    CreateFlagParameterizedTest, xeCommandListCreateTests,
    ::testing::Values(XE_COMMAND_LIST_FLAG_NONE, XE_COMMAND_LIST_FLAG_COPY_ONLY,
                      XE_COMMAND_LIST_FLAG_RELAXED_ORDERING));

class xeCommandListDestroyTests : public ::testing::Test {};

TEST_F(
    xeCommandListDestroyTests,
    GivenValidDeviceAndCommandListDescriptorWhenDestroyingCommandListThenSuccessIsReturned) {
  xe_command_list_desc_t descriptor;
  descriptor.version = XE_COMMAND_LIST_DESC_VERSION_CURRENT;

  xe_command_list_handle_t command_list = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListCreate(lzt::xeDevice::get_instance()->get_device(),
                                &descriptor, &command_list));
  EXPECT_NE(nullptr, command_list);

  lzt::destroy_command_list(command_list);
}

// xeCommandListCreateImmediateTests currently fail with
// 'XE_RESULT_ERROR_UNSUPPORTED' LOKI-289 is open for the implementation of the
// API
class xeCommandListCreateImmediateTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<
          std::tuple<xe_command_queue_flag_t, xe_command_queue_mode_t,
                     xe_command_queue_priority_t>> {};

TEST_P(xeCommandListCreateImmediateTests,
       GivenImplicitCommandQueueWhenCreatingCommandListThenSuccessIsReturned) {

  xe_command_queue_desc_t descriptor = {
      XE_COMMAND_QUEUE_DESC_VERSION_CURRENT, // version
      std::get<0>(GetParam()),               // flags
      std::get<1>(GetParam()),               // mode
      std::get<2>(GetParam())                // priority
  };

  xe_command_list_handle_t command_list = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListCreateImmediate(
                                   lzt::xeDevice::get_instance()->get_device(),
                                   &descriptor, &command_list));

  lzt::destroy_command_list(command_list);
}

INSTANTIATE_TEST_CASE_P(
    ImplictCommandQueueParameterizedTest, xeCommandListCreateImmediateTests,
    ::testing::Combine(
        ::testing::Values(XE_COMMAND_QUEUE_FLAG_NONE,
                          XE_COMMAND_QUEUE_FLAG_COPY_ONLY,
                          XE_COMMAND_QUEUE_FLAG_LOGICAL_ONLY,
                          XE_COMMAND_QUEUE_FLAG_SINGLE_SLICE_ONLY),
        ::testing::Values(XE_COMMAND_QUEUE_MODE_DEFAULT,
                          XE_COMMAND_QUEUE_MODE_SYNCHRONOUS,
                          XE_COMMAND_QUEUE_MODE_ASYNCHRONOUS),
        ::testing::Values(XE_COMMAND_QUEUE_PRIORITY_NORMAL,
                          XE_COMMAND_QUEUE_PRIORITY_LOW,
                          XE_COMMAND_QUEUE_PRIORITY_HIGH)));

class xeCommandListCloseTests : public lzt::xeCommandListTests {};

TEST_F(xeCommandListCloseTests,
       GivenEmptyCommandListWhenClosingCommandListThenSuccessIsReturned) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListClose(cl.command_list_));
}

class xeCommandListResetTests : public lzt::xeCommandListTests {};

TEST_F(xeCommandListResetTests,
       GivenEmptyCommandListWhenResettingCommandListThenSuccessIsReturned) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListReset(cl.command_list_));
}

class xeCommandListParameterTests : public lzt::xeCommandListTests {};

TEST_F(xeCommandListParameterTests,
       GivenEmptyCommandListWhenGettingTbdParameterThenSuccessIsReturned) {
  uint32_t value;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListGetParameter(cl.command_list_,
                                      XE_COMMAND_LIST_PARAMETER_TBD, &value));
}

TEST_F(xeCommandListParameterTests,
       GivenEmptyCommandListWhenSettingTbdParameterThenSuccessIsReturned) {
  uint32_t value = 0;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListSetParameter(cl.command_list_,
                                      XE_COMMAND_LIST_PARAMETER_TBD, value));
}

TEST_F(xeCommandListParameterTests,
       GivenEmptyCommandListWhenResettingParametersThenSuccessIsReturned) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListResetParameters(cl.command_list_));
}

} // namespace

// TODO: Check memory leaks after call to xeCommandListDestroy
// TODO: How to verify xeCommanListClose, xeCommandListReset,
// without command list reflection?
