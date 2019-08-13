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
#include "xe_utils/xe_utils.hpp"
#include "xe_test_harness/xe_test_harness.hpp"
#include "logging/logging.hpp"

namespace cs = compute_samples;

#include "xe_driver.h"
#include "xe_module.h"

namespace {

class xeModuleGetGlobalPointerTests : public ::testing::Test {
protected:
  xe_module_handle_t create_module(const std::string path) {
    const std::vector<uint8_t> binary_file = cs::load_binary_file(path);

    xe_module_desc_t module_description;
    module_description.version = XE_MODULE_DESC_VERSION_CURRENT;
    module_description.format = XE_MODULE_FORMAT_IL_SPIRV;
    module_description.inputSize = static_cast<uint32_t>(binary_file.size());
    module_description.pInputModule = binary_file.data();
    module_description.pBuildFlags = nullptr;

    xe_module_handle_t module = nullptr;
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeModuleCreate(device_, &module_description, &module, nullptr));

    return module;
  }

  void run_function(xe_module_handle_t module) {
    xe_function_desc_t function_description;
    function_description.version = XE_FUNCTION_DESC_VERSION_CURRENT;
    function_description.flags = XE_FUNCTION_FLAG_NONE;
    function_description.pFunctionName = "test";

    xe_function_handle_t function = nullptr;
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionCreate(module, &function_description, &function));

    EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionSetGroupSize(function, 1, 1, 1));

    xe_command_list_desc_t command_list_description;
    command_list_description.version = XE_COMMAND_LIST_DESC_VERSION_CURRENT;

    xe_command_list_handle_t command_list = nullptr;
    EXPECT_EQ(
        XE_RESULT_SUCCESS,
        xeCommandListCreate(device_, &command_list_description, &command_list));

    xe_thread_group_dimensions_t thread_group_dimensions = {1, 1, 1};

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandListAppendLaunchFunction(command_list, function,
                                                &thread_group_dimensions,
                                                nullptr, 0, nullptr));

    EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListClose(command_list));

    const uint32_t command_queue_id = 0;
    xe_command_queue_desc_t command_queue_description;
    command_queue_description.version = XE_COMMAND_QUEUE_DESC_VERSION_CURRENT;
    command_queue_description.ordinal = command_queue_id;
    command_queue_description.mode = XE_COMMAND_QUEUE_MODE_ASYNCHRONOUS;

    xe_command_queue_handle_t command_queue = nullptr;
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandQueueCreate(device_, &command_queue_description,
                                   &command_queue));

    EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandQueueExecuteCommandLists(
                                     command_queue, 1, &command_list, nullptr));
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandQueueSynchronize(command_queue, UINT32_MAX));

    EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandQueueDestroy(command_queue));
    EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListDestroy(command_list));
    EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionDestroy(function));
  }

  xe_device_handle_t device_ = nullptr;
};

TEST_F(
    xeModuleGetGlobalPointerTests,
    GivenModuleWithGlobalVariableWhenRetrivingGlobalPointerThenPointerPointsToValidGlobalVariable) {
  xe_module_handle_t module = create_module("single_global_variable.spv");

  const std::string global_name = "global_variable";
  void *global_pointer = nullptr;

  EXPECT_EQ(
      XE_RESULT_SUCCESS,
      xeModuleGetGlobalPointer(module, global_name.c_str(), &global_pointer));
  EXPECT_NE(nullptr, global_pointer);

  int *typed_global_pointer = static_cast<int *>(global_pointer);
  const int expected_value = 123;
  EXPECT_EQ(expected_value, *typed_global_pointer);

  EXPECT_EQ(XE_RESULT_SUCCESS, xeModuleDestroy(module));
}

TEST_F(
    xeModuleGetGlobalPointerTests,
    WhenRetrivingMultipleGlobalPointersFromTheSameVariableThenAllPointersAreTheSame) {
  xe_module_handle_t module = create_module("single_global_variable.spv");

  const std::string global_name = "global_variable";

  void *previous_pointer = nullptr;
  for (int i = 0; i < 5; ++i) {
    void *current_pointer = nullptr;
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeModuleGetGlobalPointer(module, global_name.c_str(),
                                       &current_pointer));
    EXPECT_NE(nullptr, current_pointer);

    if (i > 0) {
      EXPECT_EQ(previous_pointer, current_pointer);
    }

    previous_pointer = current_pointer;
  }

  EXPECT_EQ(XE_RESULT_SUCCESS, xeModuleDestroy(module));
}

TEST_F(
    xeModuleGetGlobalPointerTests,
    GivenModuleWithMultipleGlobalVariablesWhenRetrivingGlobalPointersThenAllPointersPointToValidGlobalVariable) {
  xe_module_handle_t module = create_module("multiple_global_variables.spv");

  const int global_count = 5;
  for (int i = 0; i < global_count; ++i) {
    const std::string global_name = "global_" + std::to_string(i);
    void *global_pointer = nullptr;
    EXPECT_EQ(
        XE_RESULT_SUCCESS,
        xeModuleGetGlobalPointer(module, global_name.c_str(), &global_pointer));
    EXPECT_NE(nullptr, global_pointer);

    int *typed_global_pointer = static_cast<int *>(global_pointer);
    const int expected_value = i;
    EXPECT_EQ(expected_value, *typed_global_pointer);
  }

  EXPECT_EQ(XE_RESULT_SUCCESS, xeModuleDestroy(module));
}

TEST_F(
    xeModuleGetGlobalPointerTests,
    GivenGlobalPointerWhenUpdatingGlobalVariableOnDeviceThenGlobalPointerPointsToUpdatedVariable) {
  xe_module_handle_t module = create_module("update_variable_on_device.spv");

  const std::string global_name = "global_variable";
  void *global_pointer = nullptr;

  EXPECT_EQ(
      XE_RESULT_SUCCESS,
      xeModuleGetGlobalPointer(module, global_name.c_str(), &global_pointer));
  EXPECT_NE(nullptr, global_pointer);

  int *typed_global_pointer = static_cast<int *>(global_pointer);
  const int expected_initial_value = 1;
  EXPECT_EQ(expected_initial_value, *typed_global_pointer);

  run_function(module);

  const int expected_updated_value = 2;
  EXPECT_EQ(expected_updated_value, *typed_global_pointer);

  EXPECT_EQ(XE_RESULT_SUCCESS, xeModuleDestroy(module));
}

// TODO: Update variable on host
// TODO: Update variable using memory copy
// TODO: Test different types of variables e.g. char, int, long etc.
// TODO: Test different adrress spaces of variables e.g. global, local, constant
// TODO: Pass pointer to a constant variable and update it in a kernel
// TODO: Sharing between functions
// TODO: Sharing between modules

} // namespace
