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
namespace lzt = level_zero_tests;

#include "xe_driver.h"
#include "xe_module.h"
#include "xe_barrier.h"

namespace {

enum TestType { FUNCTION, FUNCTION_INDIRECT, MULTIPLE_INDIRECT, HOST_FUNCTION };

struct FunctionData {
  void *host_buffer;
  void *shared_buffer;
};

void TestHostFunction(void *user_data) {
  struct FunctionData *hf = static_cast<FunctionData *>(user_data);
  int *host_addval_offset = static_cast<int *>(hf->host_buffer);
  int *shared_addval_offset = static_cast<int *>(hf->shared_buffer);
  shared_addval_offset[0] += host_addval_offset[0];
  host_addval_offset[0] += 12345;
}

class xeModuleCreateTests : public ::testing::Test {};

TEST_F(
    xeModuleCreateTests,
    DISABLED_GivenModuleWithGlobalVariableWhenRetrivingGlobalPointerThenPointerPointsToValidGlobalVariable) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  xe_module_handle_t module =
      lzt::create_module(device, "single_global_variable.spv");
  const std::string global_name = "global_variable";
  void *global_pointer = nullptr;

  EXPECT_EQ(
      XE_RESULT_SUCCESS,
      xeModuleGetGlobalPointer(module, global_name.c_str(), &global_pointer));
  EXPECT_NE(nullptr, global_pointer);

  int *typed_global_pointer = static_cast<int *>(global_pointer);
  const int expected_value = 123;
  EXPECT_EQ(expected_value, *typed_global_pointer);
  lzt::destroy_module(module);
}

TEST_F(
    xeModuleCreateTests,
    DISABLED_WhenRetrivingMultipleGlobalPointersFromTheSameVariableThenAllPointersAreTheSame) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  xe_module_handle_t module =
      lzt::create_module(device, "single_global_variable.spv");

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
  lzt::destroy_module(module);
}

TEST_F(
    xeModuleCreateTests,
    DISABLED_GivenModuleWithMultipleGlobalVariablesWhenRetrivingGlobalPointersThenAllPointersPointToValidGlobalVariable) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  xe_module_handle_t module =
      lzt::create_module(device, "multiple_global_variables.spv");

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
  lzt::destroy_module(module);
}

TEST_F(
    xeModuleCreateTests,
    DISABLED_GivenGlobalPointerWhenUpdatingGlobalVariableOnDeviceThenGlobalPointerPointsToUpdatedVariable) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  xe_module_handle_t module =
      lzt::create_module(device, "update_variable_on_device.spv");

  const std::string global_name = "global_variable";
  void *global_pointer = nullptr;

  EXPECT_EQ(
      XE_RESULT_SUCCESS,
      xeModuleGetGlobalPointer(module, global_name.c_str(), &global_pointer));
  EXPECT_NE(nullptr, global_pointer);

  int *typed_global_pointer = static_cast<int *>(global_pointer);
  const int expected_initial_value = 1;
  EXPECT_EQ(expected_initial_value, *typed_global_pointer);

  lzt::create_and_execute_function(device, module, "test", 1, nullptr);

  const int expected_updated_value = 2;
  EXPECT_EQ(expected_updated_value, *typed_global_pointer);
  lzt::destroy_module(module);
}

TEST_F(
    xeModuleCreateTests,
    GivenModuleWithFunctionWhenRetrivingFunctionPointerThenPointerPointsToValidFunction) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  xe_module_handle_t module = lzt::create_module(device, "xe_module_add.spv");
  const std::string function_name = "xe_module_add_constant";
  void *function_pointer = nullptr;

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeModuleGetFunctionPointer(module, function_name.c_str(),
                                       &function_pointer));
  EXPECT_NE(nullptr, function_pointer);

  lzt::destroy_module(module);
}

TEST_F(
    xeModuleCreateTests,
    GivenValidDeviceAndBinaryFileWhenCreatingModuleThenReturnSuccessfulAndDestroyModule) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  xe_module_handle_t module =
      lzt::create_module(device, "xe_atomic_access.spv");
  lzt::destroy_module(module);
}

TEST_F(
    xeModuleCreateTests,
    GivenValidDeviceAndBinaryFileWhenCreatingModuleThenOutputBuildLogAndReturnSuccessful) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  xe_module_build_log_handle_t build_log;
  xe_module_handle_t module =
      lzt::create_module(device, "xe_module_add.spv", XE_MODULE_FORMAT_IL_SPIRV,
                         nullptr, &build_log);
  lzt::destroy_module(module);
  size_t build_log_size = lzt::get_build_log_size(build_log);
  std::string build_log_str = lzt::get_build_log_string(build_log);
  LOG_INFO << "Build Log Size = " << build_log_size;
  LOG_INFO << "Build Log String = " << build_log_str;
  lzt::destroy_build_log(build_log);
}

TEST_F(
    xeModuleCreateTests,
    GivenValidModuleWhenGettingNativeBinaryFileThenRetrieveFileAndReturnSuccessful) {
  const xe_device_handle_t device = lzt::xeDevice::get_instance()->get_device();
  size_t size = 0;

  xe_module_handle_t module = lzt::create_module(
      device, "xe_module_add.spv", XE_MODULE_FORMAT_IL_SPIRV, nullptr, nullptr);
  size = lzt::get_native_binary_size(module);
  LOG_INFO << "Native binary size: " << size;
  lzt::save_native_binary_file(module, "xe_module_add.native");
  lzt::destroy_module(module);
  module = lzt::create_module(device, "xe_module_add.native",
                              XE_MODULE_FORMAT_NATIVE, nullptr, nullptr);
  lzt::destroy_module(module);
}

class xeFunctionCreateTests : public lzt::xeEventPoolTests {
protected:
  void SetUp() override {
    device_ = lzt::xeDevice::get_instance()->get_device();
    module_ = lzt::create_module(device_, "xe_module_add.spv",
                                 XE_MODULE_FORMAT_IL_SPIRV, nullptr, nullptr);
  }

  void run_test(xe_thread_group_dimensions_t th_group_dim,
                uint32_t group_size_x, uint32_t group_size_y,
                uint32_t group_size_z, bool signal_to_host,
                bool signal_from_host, enum TestType type) {
    uint32_t num_events = std::min(group_size_x, static_cast<uint32_t>(6));
    xe_event_handle_t event_kernel_to_host = nullptr;
    std::vector<xe_event_handle_t> events_host_to_kernel(num_events, nullptr);
    std::vector<int> inpa = {0, 1, 2,  3,  4,  5,  6,  7,
                             8, 9, 10, 11, 12, 13, 14, 15};
    std::vector<int> inpb = {1, 2,  3,  4,  5,  6,  7,  8,
                             9, 10, 11, 12, 13, 14, 15, 16};
    void *args_buff = lzt::allocate_shared_memory(
        2 * sizeof(xe_thread_group_dimensions_t), sizeof(int),
        XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT, XE_HOST_MEM_ALLOC_FLAG_DEFAULT,
        device_);
    void *actual_launch = lzt::allocate_shared_memory(
        sizeof(uint32_t), sizeof(uint32_t), XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
        XE_HOST_MEM_ALLOC_FLAG_DEFAULT, device_);
    void *input_a = lzt::allocate_shared_memory(
        16 * sizeof(int), 1, XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
        XE_HOST_MEM_ALLOC_FLAG_DEFAULT, device_);
    void *mult_out = lzt::allocate_shared_memory(
        16 * sizeof(int), 1, XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
        XE_HOST_MEM_ALLOC_FLAG_DEFAULT, device_);
    void *mult_in = lzt::allocate_shared_memory(
        16 * sizeof(int), 1, XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
        XE_HOST_MEM_ALLOC_FLAG_DEFAULT, device_);
    void *host_buff = lzt::allocate_host_memory(sizeof(int));
    int *host_addval_offset = static_cast<int *>(host_buff);

    int addval = 10;
    int host_offset = 200;
    int *input_a_int = static_cast<int *>(input_a);

    if (signal_to_host) {
      ep.create_event(event_kernel_to_host, XE_EVENT_SCOPE_FLAG_HOST,
                      XE_EVENT_SCOPE_FLAG_HOST);
    }
    if (signal_from_host) {
      ep.create_events(events_host_to_kernel, num_events,
                       XE_EVENT_SCOPE_FLAG_HOST, XE_EVENT_SCOPE_FLAG_HOST);
    }

    xe_function_handle_t function =
        lzt::create_function(module_, "xe_module_add_constant");
    xe_command_list_handle_t cmd_list = lzt::create_command_list(device_);
    xe_command_queue_handle_t cmd_q = lzt::create_command_queue(device_);
    memset(input_a, 0, 16);

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetArgumentValue(function, 0, sizeof(input_a_int),
                                         &input_a_int));
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetArgumentValue(function, 1, sizeof(addval), &addval));
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetGroupSize(function, group_size_x, group_size_y,
                                     group_size_z));

    xe_event_handle_t signal_event = nullptr;
    uint32_t num_wait = 0;
    xe_event_handle_t *p_wait_events = nullptr;
    if (signal_to_host) {
      signal_event = event_kernel_to_host;
    }
    if (signal_from_host) {
      p_wait_events = events_host_to_kernel.data();
      num_wait = num_events;
    }
    if (type == FUNCTION) {
      EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendLaunchFunction(
                                       cmd_list, function, &th_group_dim,
                                       signal_event, num_wait, p_wait_events));
    } else if (type == FUNCTION_INDIRECT) {
      xe_thread_group_dimensions_t *tg_dim =
          static_cast<xe_thread_group_dimensions_t *>(args_buff);

      EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendLaunchFunctionIndirect(
                                       cmd_list, function, tg_dim, signal_event,
                                       num_wait, p_wait_events));

      // Intentionally update args_buff after Launch API
      memcpy(args_buff, &th_group_dim, sizeof(xe_thread_group_dimensions_t));
    } else if (type == MULTIPLE_INDIRECT) {
      int *mult_out_int = static_cast<int *>(mult_out);
      int *mult_in_int = static_cast<int *>(mult_in);
      xe_thread_group_dimensions_t *tg_dim =
          static_cast<xe_thread_group_dimensions_t *>(args_buff);
      memcpy(mult_out_int, inpa.data(), 16 * sizeof(int));
      memcpy(mult_in_int, inpb.data(), 16 * sizeof(int));
      std::vector<xe_function_handle_t> function_list;
      std::vector<xe_thread_group_dimensions_t> arg_buffer_list;
      std::vector<uint32_t> num_launch_arg_list;
      function_list.push_back(function);
      xe_function_handle_t mult_function =
          lzt::create_function(module_, "xe_module_add_two_arrays");
      function_list.push_back(mult_function);

      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeFunctionSetArgumentValue(
                    mult_function, 0, sizeof(mult_out_int), &mult_out_int));
      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeFunctionSetArgumentValue(mult_function, 1,
                                           sizeof(mult_in_int), &mult_in_int));
      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeFunctionSetGroupSize(mult_function, 1, 1, 1));

      arg_buffer_list.push_back(th_group_dim);
      xe_thread_group_dimensions_t mult_th_group_dim;
      mult_th_group_dim.groupCountX = 16;
      mult_th_group_dim.groupCountY = 1;
      mult_th_group_dim.groupCountZ = 1;
      arg_buffer_list.push_back(mult_th_group_dim);
      size_t *num_launch_arg = static_cast<size_t *>(actual_launch);

      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeCommandListAppendLaunchMultipleFunctionsIndirect(
                    cmd_list, 2, function_list.data(), num_launch_arg,
                    arg_buffer_list.data(), signal_event, num_wait,
                    p_wait_events));

      // Intentionally update args buffer and num_args after API
      num_launch_arg[0] = 2;
      memcpy(args_buff, arg_buffer_list.data(),
             2 * sizeof(xe_thread_group_dimensions_t));
    } else if (type == HOST_FUNCTION) {
      // Append host function to command list followed by device function
      // Host function execution blocks device function execution

      host_addval_offset[0] = host_offset;
      struct FunctionData user_data;
      user_data.host_buffer = host_buff;
      user_data.shared_buffer = input_a;
      xe_host_pfn_t hostFunction = &TestHostFunction;

      EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendLaunchHostFunction(
                                       cmd_list, hostFunction, &user_data));
      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeCommandListAppendLaunchFunction(
                    cmd_list, function, &th_group_dim, nullptr, 0, nullptr));
    }

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandListAppendBarrier(cmd_list, nullptr, 0, nullptr));

    EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListClose(cmd_list));

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandQueueExecuteCommandLists(cmd_q, 1, &cmd_list, nullptr));
    // Note: Current test for Wait Events will hang on Fulsim and Cobalt
    if (signal_from_host) {
      for (uint32_t i = 0; i < num_events; i++) {
        EXPECT_EQ(XE_RESULT_SUCCESS,
                  xeEventHostSignal(events_host_to_kernel[i]));
      }
    }

    EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandQueueSynchronize(cmd_q, UINT32_MAX));

    if (signal_to_host) {
      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeEventHostSynchronize(event_kernel_to_host, UINT32_MAX));
    }

    int offset = 0;
    if (type == HOST_FUNCTION) {
      offset = host_offset;
      EXPECT_EQ(host_addval_offset[0], host_offset + 12345);
    }
    EXPECT_EQ(input_a_int[0],
              offset + ((addval * (group_size_x * th_group_dim.groupCountX) *
                         (group_size_y * th_group_dim.groupCountY) *
                         (group_size_z * th_group_dim.groupCountZ))));
    if (type == MULTIPLE_INDIRECT) {
      for (uint32_t i = 0; i < 16; i++) {
        EXPECT_EQ(input_a_int[i], inpa[i] + inpb[i]);
      }
    }
    EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandQueueDestroy(cmd_q));
    EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListDestroy(cmd_list));
    lzt::destroy_function(function);
    lzt::free_memory(host_buff);
    lzt::free_memory(mult_in);
    lzt::free_memory(mult_out);
    lzt::free_memory(input_a);
    lzt::free_memory(actual_launch);
    lzt::free_memory(args_buff);
    if (signal_to_host) {
      ep.destroy_event(event_kernel_to_host);
    }
    if (signal_from_host) {
      ep.destroy_events(events_host_to_kernel);
    }
  }

  void TearDown() override { lzt::destroy_module(module_); }

  xe_device_handle_t device_ = nullptr;
  xe_module_handle_t module_ = nullptr;
};

TEST_F(xeFunctionCreateTests,
       GivenValidModuleWhenCreatingFunctionThenReturnSuccessful) {
  xe_function_handle_t function = lzt::create_function(
      module_, XE_FUNCTION_FLAG_NONE, "xe_module_add_constant");
  lzt::destroy_function(function);
  function = lzt::create_function(module_, XE_FUNCTION_FLAG_FORCE_RESIDENCY,
                                  "xe_module_add_two_arrays");
  lzt::destroy_function(function);
}

TEST_F(xeFunctionCreateTests,
       GivenValidFunctionWhenSettingGroupSizeThenReturnSuccessful) {
  xe_function_handle_t function = lzt::create_function(
      module_, XE_FUNCTION_FLAG_NONE, "xe_module_add_constant");
  xe_device_compute_properties_t dev_compute_properties;
  dev_compute_properties.version = XE_DEVICE_COMPUTE_PROPERTIES_VERSION_CURRENT;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGetComputeProperties(device_, &dev_compute_properties));
  for (uint32_t x = 1; x < dev_compute_properties.maxGroupSizeX; x++) {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionSetGroupSize(function, x, 1, 1));
  }
  for (uint32_t y = 1; y < dev_compute_properties.maxGroupSizeY; y++) {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionSetGroupSize(function, 1, y, 1));
  }
  for (uint32_t z = 1; z < dev_compute_properties.maxGroupSizeZ; z++) {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionSetGroupSize(function, 1, 1, z));
  }
  uint32_t x = 1;
  uint32_t y = 1;
  uint32_t z = 1;
  while (x * y < dev_compute_properties.maxTotalGroupSize) {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionSetGroupSize(function, x++, y++, 1));
  }
  x = y = z = 1;
  while (y * z < dev_compute_properties.maxTotalGroupSize) {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionSetGroupSize(function, 1, y++, z++));
  }
  x = y = z = 1;
  while (x * z < dev_compute_properties.maxTotalGroupSize) {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionSetGroupSize(function, x++, 1, z++));
  }
  x = y = z = 1;
  while (x * y * z < dev_compute_properties.maxTotalGroupSize) {
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetGroupSize(function, x++, y++, z++));
  }

  lzt::destroy_function(function);
}

TEST_F(xeFunctionCreateTests,
       GivenValidFunctionWhenSuggestingGroupSizeThenReturnSuccessful) {
  xe_function_handle_t function = lzt::create_function(
      module_, XE_FUNCTION_FLAG_NONE, "xe_module_add_constant");

  xe_device_compute_properties_t dev_compute_properties;
  dev_compute_properties.version = XE_DEVICE_COMPUTE_PROPERTIES_VERSION_CURRENT;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGetComputeProperties(device_, &dev_compute_properties));
  uint32_t group_size_x = 0;
  uint32_t group_size_y = 0;
  uint32_t group_size_z = 0;
  for (uint32_t x = UINT32_MAX; x > 0; x = x >> 1) {
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSuggestGroupSize(function, x, 1, 1, &group_size_x,
                                         &group_size_y, &group_size_z));
    EXPECT_LE(group_size_x, dev_compute_properties.maxGroupSizeX);
  }
  for (uint32_t y = UINT32_MAX; y > 0; y = y >> 1) {
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSuggestGroupSize(function, 1, y, 1, &group_size_x,
                                         &group_size_y, &group_size_z));
    EXPECT_LE(group_size_y, dev_compute_properties.maxGroupSizeY);
  }
  for (uint32_t z = UINT32_MAX; z > 0; z = z >> 1) {
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSuggestGroupSize(function, 1, 1, z, &group_size_x,
                                         &group_size_y, &group_size_z));
    EXPECT_LE(group_size_z, dev_compute_properties.maxGroupSizeZ);
  }
  for (uint32_t i = UINT32_MAX; i > 0; i = i >> 1) {
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSuggestGroupSize(function, i, i, i, &group_size_x,
                                         &group_size_y, &group_size_z));
    EXPECT_LE(group_size_x, dev_compute_properties.maxGroupSizeX);
    EXPECT_LE(group_size_y, dev_compute_properties.maxGroupSizeY);
    EXPECT_LE(group_size_z, dev_compute_properties.maxGroupSizeZ);
  }

  lzt::destroy_function(function);
}

TEST_F(xeFunctionCreateTests,
       GivenValidFunctionWhenSettingArgumentsThenReturnSuccessful) {

  void *input_a =
      lzt::allocate_shared_memory(16, 1, XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
                                  XE_HOST_MEM_ALLOC_FLAG_DEFAULT, device_);
  void *input_b =
      lzt::allocate_shared_memory(16, 1, XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
                                  XE_HOST_MEM_ALLOC_FLAG_DEFAULT, device_);
  int addval = 10;
  int *input_a_int = static_cast<int *>(input_a);
  int *input_b_int = static_cast<int *>(input_b);
  xe_function_handle_t function =
      lzt::create_function(module_, "xe_module_add_constant");
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionSetArgumentValue(function, 0, sizeof(input_a_int),
                                       &input_a_int));
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionSetArgumentValue(function, 1, sizeof(addval), &addval));

  lzt::destroy_function(function);
  function = lzt::create_function(module_, "xe_module_add_two_arrays");
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionSetArgumentValue(function, 0, sizeof(input_a_int),
                                       &input_a_int));
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionSetArgumentValue(function, 1, sizeof(input_b_int),
                                       &input_b_int));
  lzt::destroy_function(function);
  lzt::free_memory(input_a);
  lzt::free_memory(input_b);
}

TEST_F(xeFunctionCreateTests,
       GivenValidFunctionWhenGettingAttributesThenReturnSuccessful) {

  xe_function_handle_t function =
      lzt::create_function(module_, "xe_module_add_constant");

  uint32_t attribute_val = 0;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionGetAttribute(function, XE_FUNCTION_GET_ATTR_MAX_REGS_USED,
                                   &attribute_val));
  LOG_INFO << "Maximum Device Registers = " << attribute_val;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionGetAttribute(function,
                                   XE_FUNCTION_GET_ATTR_NUM_THREAD_DIMENSIONS,
                                   &attribute_val));
  LOG_INFO << "Maximum Thread Dimensions for Group = " << attribute_val;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionGetAttribute(function,
                                   XE_FUNCTION_GET_ATTR_MAX_SHARED_MEM_SIZE,
                                   &attribute_val));
  LOG_INFO << "Maximum Shared Memory = " << attribute_val;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionGetAttribute(
                function, XE_FUNCTION_GET_ATTR_HAS_SPILL_FILL, &attribute_val));
  LOG_INFO << "SPILL/FILLs = " << attribute_val;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionGetAttribute(function, XE_FUNCTION_GET_ATTR_HAS_BARRIERS,
                                   &attribute_val));
  LOG_INFO << "Barriers = " << attribute_val;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionGetAttribute(function, XE_FUNCTION_GET_ATTR_HAS_DPAS,
                                   &attribute_val));
  LOG_INFO << "DPAs = " << attribute_val;
  lzt::destroy_function(function);
}

TEST_F(xeFunctionCreateTests,
       GivenValidFunctionWhenSettingAttributesThenReturnSuccessful) {

  xe_function_handle_t function =
      lzt::create_function(module_, "xe_module_add_constant");
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionSetAttribute(
                function, XE_FUNCTION_SET_ATTR_INDIRECT_HOST_ACCESS, true));
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionSetAttribute(
                function, XE_FUNCTION_SET_ATTR_INDIRECT_HOST_ACCESS, false));
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionSetAttribute(
                function, XE_FUNCTION_SET_ATTR_INDIRECT_DEVICE_ACCESS, true));
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionSetAttribute(
                function, XE_FUNCTION_SET_ATTR_INDIRECT_DEVICE_ACCESS, false));
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionSetAttribute(
                function, XE_FUNCTION_SET_ATTR_INDIRECT_SHARED_ACCESS, true));
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeFunctionSetAttribute(
                function, XE_FUNCTION_SET_ATTR_INDIRECT_SHARED_ACCESS, false));
  lzt::destroy_function(function);
}

class xeFunctionLaunchTests
    : public ::xeFunctionCreateTests,
      public ::testing::WithParamInterface<enum TestType> {};

TEST_P(
    xeFunctionLaunchTests,
    GivenValidFunctionWhenAppendLaunchFunctionThenReturnSuccessfulAndVerifyExecution) {
  xe_device_compute_properties_t dev_compute_properties;
  dev_compute_properties.version = XE_DEVICE_COMPUTE_PROPERTIES_VERSION_CURRENT;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGetComputeProperties(device_, &dev_compute_properties));

  uint32_t group_size_x;
  uint32_t group_size_y;
  uint32_t group_size_z;
  xe_thread_group_dimensions_t thread_group_dimensions;

  enum TestType test_type = GetParam();

  std::vector<int> dim = {1, 2, 3};
  std::vector<uint32_t> tg_count = {1, 2, 3, 4};
  std::vector<uint32_t> grp_size = {1, 2, 3, 4};
  std::vector<bool> sig_to_host = {false, true};
  std::vector<bool> sig_from_host = {false, true};
  if ((test_type == MULTIPLE_INDIRECT) || (test_type == HOST_FUNCTION)) {
    dim.erase(dim.begin(), dim.begin() + 2);
    tg_count.erase(tg_count.begin(), tg_count.begin() + 3);
    grp_size.erase(grp_size.begin(), grp_size.begin() + 3);
    sig_to_host.erase(sig_to_host.begin(), sig_to_host.begin() + 1);
    sig_from_host.erase(sig_from_host.begin(), sig_from_host.begin() + 1);
  }

  for (auto d : dim) {
    LOG_INFO << d << "-Dimensional Group Size Tests";
    for (auto tg : tg_count) {
      thread_group_dimensions.groupCountX = tg;
      thread_group_dimensions.groupCountY = (d > 1) ? tg : 1;
      thread_group_dimensions.groupCountZ = (d > 2) ? tg : 1;
      for (auto grp : grp_size) {
        group_size_x = grp;
        group_size_y = (d > 1) ? grp : 1;
        group_size_z = (d > 2) ? grp : 1;
        ASSERT_LE(group_size_x * group_size_y * group_size_z,
                  dev_compute_properties.maxTotalGroupSize);
        for (auto sig1 : sig_to_host) {
          for (auto sig2 : sig_from_host) {
            run_test(thread_group_dimensions, group_size_x, group_size_y,
                     group_size_z, sig1, sig2, test_type);
          }
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(
    TestFunctionAndFunctionIndirectAndMultipleFunctionsIndirect,
    xeFunctionLaunchTests,
    testing::Values(FUNCTION, FUNCTION_INDIRECT, MULTIPLE_INDIRECT,
                    HOST_FUNCTION));

} // namespace
