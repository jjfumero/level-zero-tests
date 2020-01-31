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
#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"
#include "logging/logging.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>

namespace lzt = level_zero_tests;

#include "ze_api.h"

namespace {

enum TestType { FUNCTION, FUNCTION_INDIRECT, MULTIPLE_INDIRECT };

struct FunctionData {
  void *host_buffer;
  void *shared_buffer;
};

std::vector<ze_module_handle_t> create_module_vector_and_log(
    ze_device_handle_t device, const std::string filename_prefix,
    std::vector<ze_module_build_log_handle_t> *build_log) {
  std::vector<ze_module_handle_t> module;

  std::vector<const char *> build_flag = {
      nullptr, "-ze-opt-disable", "-ze-opt-greater-than-4GB-buffer-required",
      "-ze-opt-large-register-file"};
  // Check with LOKI-453: last build option may cause crash on Gen12

  auto start = std::chrono::system_clock::now();
  auto end = std::chrono::system_clock::now();
  // Create pseudo-random integer to add to native binary filename
  srand(time(NULL) +
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            .count());
  std::string filename_native =
      filename_prefix + std::to_string(rand()) + ".native";
  std::string filename_spirv = filename_prefix + ".spv";

  if (build_log) {
    build_log->resize(2 * build_flag.size());
    size_t count = 0;

    for (auto flag : build_flag) {

      module.push_back(lzt::create_module(device, filename_spirv,
                                          ZE_MODULE_FORMAT_IL_SPIRV, flag,
                                          &build_log->at(count)));
      count++;

      lzt::save_native_binary_file(module.back(), filename_native);
      module.push_back(lzt::create_module(device, filename_native,
                                          ZE_MODULE_FORMAT_NATIVE, nullptr,
                                          &build_log->at(count)));
      count++;
      std::remove(filename_native.c_str());
    }
  } else {
    for (auto flag : build_flag) {
      module.push_back(lzt::create_module(
          device, filename_spirv, ZE_MODULE_FORMAT_IL_SPIRV, flag, nullptr));

      lzt::save_native_binary_file(module.back(), filename_native);
      module.push_back(lzt::create_module(
          device, filename_native, ZE_MODULE_FORMAT_NATIVE, nullptr, nullptr));
      std::remove(filename_native.c_str());
    }
  }
  return (module);
}

std::vector<ze_module_handle_t>
create_module_vector(ze_device_handle_t device,
                     const std::string filename_prefix) {
  return (create_module_vector_and_log(device, filename_prefix, nullptr));
}
class zeModuleCreateTests : public ::testing::Test {};

TEST_F(
    zeModuleCreateTests,
    GivenModuleWithGlobalVariableWhenRetrievingGlobalPointerThenPointerPointsToValidGlobalVariable) {
  const ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  std::vector<ze_module_handle_t> module =
      create_module_vector(device, "single_global_variable");

  const std::string global_name = "global_variable";
  void *global_pointer;
  const int expected_value = 123;
  int *typed_global_pointer;

  for (auto mod : module) {
    global_pointer = nullptr;
    ASSERT_EQ(
        ZE_RESULT_SUCCESS,
        zeModuleGetGlobalPointer(mod, global_name.c_str(), &global_pointer));
    EXPECT_NE(nullptr, global_pointer);
    typed_global_pointer = static_cast<int *>(global_pointer);
    EXPECT_EQ(expected_value, *typed_global_pointer);
    lzt::destroy_module(mod);
  }
}

TEST_F(
    zeModuleCreateTests,
    WhenRetrievingMultipleGlobalPointersFromTheSameVariableThenAllPointersAreTheSame) {
  const ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  std::vector<ze_module_handle_t> module =
      create_module_vector(device, "single_global_variable");

  const std::string global_name = "global_variable";
  void *previous_pointer;
  void *current_pointer;

  for (auto mod : module) {
    previous_pointer = nullptr;

    for (int i = 0; i < 5; ++i) {
      current_pointer = nullptr;
      ASSERT_EQ(
          ZE_RESULT_SUCCESS,
          zeModuleGetGlobalPointer(mod, global_name.c_str(), &current_pointer));
      EXPECT_NE(nullptr, current_pointer);

      if (i > 0) {
        EXPECT_EQ(previous_pointer, current_pointer);
      }
      previous_pointer = current_pointer;
    }
    lzt::destroy_module(mod);
  }
}

TEST_F(
    zeModuleCreateTests,
    GivenModuleWithMultipleGlobalVariablesWhenRetrievingGlobalPointersThenAllPointersPointToValidGlobalVariable) {

  const ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  std::vector<ze_module_handle_t> module =
      create_module_vector(device, "multiple_global_variables");

  const int global_count = 5;
  void *global_pointer;
  int *typed_global_pointer;

  for (auto mod : module) {
    for (int i = 0; i < global_count; ++i) {
      std::string global_name = "global_" + std::to_string(i);
      global_pointer = nullptr;
      ASSERT_EQ(
          ZE_RESULT_SUCCESS,
          zeModuleGetGlobalPointer(mod, global_name.c_str(), &global_pointer));
      EXPECT_NE(nullptr, global_pointer);

      typed_global_pointer = static_cast<int *>(global_pointer);
      EXPECT_EQ(i, *typed_global_pointer);
    }
    lzt::destroy_module(mod);
  }
}

TEST_F(
    zeModuleCreateTests,
    GivenGlobalPointerWhenUpdatingGlobalVariableOnDeviceThenGlobalPointerPointsToUpdatedVariable) {
  const ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  std::vector<ze_module_handle_t> module =
      create_module_vector(device, "update_variable_on_device");

  const std::string global_name = "global_variable";
  void *global_pointer;
  int *typed_global_pointer;
  const int expected_initial_value = 1;
  const int expected_updated_value = 2;

  for (auto mod : module) {
    global_pointer = nullptr;
    ASSERT_EQ(
        ZE_RESULT_SUCCESS,
        zeModuleGetGlobalPointer(mod, global_name.c_str(), &global_pointer));
    EXPECT_NE(nullptr, global_pointer);
    typed_global_pointer = static_cast<int *>(global_pointer);
    EXPECT_EQ(expected_initial_value, *typed_global_pointer);
    lzt::create_and_execute_function(device, mod, "test", 1, nullptr);
    EXPECT_EQ(expected_updated_value, *typed_global_pointer);
    lzt::destroy_module(mod);
  }
}

TEST_F(
    zeModuleCreateTests,
    GivenModuleWithFunctionWhenRetrievingFunctionPointerThenPointerPointsToValidFunction) {
  const ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  std::vector<ze_module_handle_t> module =
      create_module_vector(device, "module_add");

  const std::string function_name = "module_add_constant";
  void *function_pointer;

  for (auto mod : module) {
    function_pointer = nullptr;
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeModuleGetFunctionPointer(mod, function_name.c_str(),
                                         &function_pointer));
    EXPECT_NE(nullptr, function_pointer);
    lzt::destroy_module(mod);
  }
}

TEST_F(
    zeModuleCreateTests,
    GivenValidDeviceAndBinaryFileWhenCreatingModuleThenReturnSuccessfulAndDestroyModule) {
  const ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  std::vector<ze_module_handle_t> module =
      create_module_vector(device, "module_add");
  for (auto mod : module) {
    lzt::destroy_module(mod);
  }
}

TEST_F(
    zeModuleCreateTests,
    GivenValidDeviceAndBinaryFileWhenCreatingModuleThenOutputBuildLogAndReturnSuccessful) {
  const ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();

  std::vector<ze_module_build_log_handle_t> build_log;
  std::vector<ze_module_handle_t> module =
      create_module_vector_and_log(device, "module_add", &build_log);
  // Should native mode provide build log?
  size_t build_log_size;
  std::string build_log_str;

  for (auto log : build_log) {
    build_log_size = lzt::get_build_log_size(log);
    build_log_str = lzt::get_build_log_string(log);
    // Build log empty if no errors from IGC
    EXPECT_EQ(1, build_log_size);
    EXPECT_EQ('\0', build_log_str[0]);
    LOG_INFO << "Build Log Size = " << build_log_size;
    LOG_INFO << "Build Log String = " << build_log_str;

    lzt::destroy_build_log(log);
  }

  for (auto mod : module) {
    lzt::destroy_module(mod);
  }
}

TEST_F(
    zeModuleCreateTests,
    GivenInvalidDeviceAndBinaryFileWhenCreatingModuleThenFailsAndOutputBuildReturnsErrorString) {
  const ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  ze_module_build_log_handle_t build_log_error;
  ze_module_desc_t module_description;
  size_t build_log_size;
  std::string build_log_str;
  ze_module_handle_t module_error = nullptr;
  const std::vector<uint8_t> binary_file =
      level_zero_tests::load_binary_file("module_build_error.spv");

  module_description.version = ZE_MODULE_DESC_VERSION_CURRENT;
  module_description.format = ZE_MODULE_FORMAT_IL_SPIRV;
  module_description.inputSize = static_cast<uint32_t>(binary_file.size());
  module_description.pInputModule = binary_file.data();
  module_description.pBuildFlags = nullptr;
  EXPECT_EQ(ZE_RESULT_ERROR_MODULE_BUILD_FAILURE,
            zeModuleCreate(device, &module_description, &module_error,
                           &build_log_error));
  EXPECT_EQ(nullptr, module_error);
  build_log_size = lzt::get_build_log_size(build_log_error);
  build_log_str = lzt::get_build_log_string(build_log_error);
  EXPECT_GT(build_log_size, 1);
  EXPECT_NE('\0', build_log_str[0]);
  LOG_INFO << "Build Log Size = " << build_log_size;
  LOG_INFO << "Build Log String = " << build_log_str;
}

TEST_F(
    zeModuleCreateTests,
    GivenValidModuleWhenGettingNativeBinaryFileThenRetrieveFileAndReturnSuccessful) {
  const ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  size_t size = 0;
  ze_module_build_log_handle_t build_log;
  // Note: Only one example shown here, as subset of functionality of
  // "create_module_vector"

  auto start = std::chrono::system_clock::now();
  auto end = std::chrono::system_clock::now();
  // Create pseudo-random integer to add to native binary filename
  srand(time(NULL) +
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            .count());
  std::string filename_native =
      "module_add" + std::to_string(rand()) + ".native";
  std::string filename_spirv = "module_add.spv";

  ze_module_handle_t module =
      lzt::create_module(device, filename_spirv, ZE_MODULE_FORMAT_IL_SPIRV,
                         "-ze-opt-disable", nullptr);
  size = lzt::get_native_binary_size(module);
  LOG_INFO << "Native binary size: " << size;
  lzt::save_native_binary_file(module, filename_native);

  std::ifstream stream(filename_native, std::ios::in | std::ios::binary);
  stream.seekg(0, stream.end);
  EXPECT_EQ(static_cast<size_t>(stream.tellg()), size);
  std::remove(filename_native.c_str());
  lzt::destroy_module(module);
}

class zeKernelCreateTests : public lzt::zeEventPoolTests {
protected:
  void SetUp() override {
    device_ = lzt::zeDevice::get_instance()->get_device();
    module_ = create_module_vector(device_, "module_add");
  }

  void run_test(ze_module_handle_t mod, ze_group_count_t th_group_dim,
                uint32_t group_size_x, uint32_t group_size_y,
                uint32_t group_size_z, bool signal_to_host,
                bool signal_from_host, enum TestType type) {
    uint32_t num_events = std::min(group_size_x, static_cast<uint32_t>(6));
    ze_event_handle_t event_kernel_to_host = nullptr;
    ze_kernel_handle_t function;
    ze_kernel_handle_t mult_function;
    std::vector<ze_event_handle_t> events_host_to_kernel(num_events, nullptr);
    std::vector<int> inpa = {0, 1, 2,  3,  4,  5,  6,  7,
                             8, 9, 10, 11, 12, 13, 14, 15};
    std::vector<int> inpb = {1, 2,  3,  4,  5,  6,  7,  8,
                             9, 10, 11, 12, 13, 14, 15, 16};
    void *args_buff =
        lzt::allocate_shared_memory(2 * sizeof(ze_group_count_t), sizeof(int),
                                    ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
                                    ZE_HOST_MEM_ALLOC_FLAG_DEFAULT, device_);
    void *actual_launch = lzt::allocate_shared_memory(
        sizeof(uint32_t), sizeof(uint32_t), ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
        ZE_HOST_MEM_ALLOC_FLAG_DEFAULT, device_);
    void *input_a = lzt::allocate_shared_memory(
        16 * sizeof(int), 1, ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
        ZE_HOST_MEM_ALLOC_FLAG_DEFAULT, device_);
    void *mult_out = lzt::allocate_shared_memory(
        16 * sizeof(int), 1, ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
        ZE_HOST_MEM_ALLOC_FLAG_DEFAULT, device_);
    void *mult_in = lzt::allocate_shared_memory(
        16 * sizeof(int), 1, ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
        ZE_HOST_MEM_ALLOC_FLAG_DEFAULT, device_);
    void *host_buff = lzt::allocate_host_memory(sizeof(int));
    int *host_addval_offset = static_cast<int *>(host_buff);

    const int addval = 10;
    const int host_offset = 200;
    int *input_a_int = static_cast<int *>(input_a);

    if (signal_to_host) {
      ep.create_event(event_kernel_to_host, ZE_EVENT_SCOPE_FLAG_HOST,
                      ZE_EVENT_SCOPE_FLAG_HOST);
    }
    if (signal_from_host) {
      ep.create_events(events_host_to_kernel, num_events,
                       ZE_EVENT_SCOPE_FLAG_HOST, ZE_EVENT_SCOPE_FLAG_HOST);
    }

    function = lzt::create_function(mod, "module_add_constant");
    ze_command_list_handle_t cmd_list = lzt::create_command_list(device_);
    ze_command_queue_handle_t cmd_q = lzt::create_command_queue(device_);
    memset(input_a, 0, 16);

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(function, 0, sizeof(input_a_int),
                                       &input_a_int));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(function, 1, sizeof(addval), &addval));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetGroupSize(function, group_size_x, group_size_y,
                                   group_size_z));

    ze_kernel_properties_t kernel_properties;
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelGetProperties(function, &kernel_properties));
    EXPECT_EQ(kernel_properties.requiredGroupSizeX, group_size_x);
    EXPECT_EQ(kernel_properties.requiredGroupSizeY, group_size_y);
    EXPECT_EQ(kernel_properties.requiredGroupSizeZ, group_size_z);
    EXPECT_EQ(kernel_properties.numKernelArgs, 2);
    EXPECT_STREQ(kernel_properties.name, "module_add_constant");

    ze_event_handle_t signal_event = nullptr;
    uint32_t num_wait = 0;
    ze_event_handle_t *p_wait_events = nullptr;
    if (signal_to_host) {
      signal_event = event_kernel_to_host;
    }
    if (signal_from_host) {
      p_wait_events = events_host_to_kernel.data();
      num_wait = num_events;
    }
    if (type == FUNCTION) {
      // This test failing pending fix for LOKI-551
      EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendLaunchKernel(
                                       cmd_list, function, &th_group_dim,
                                       signal_event, num_wait, p_wait_events));
    } else if (type == FUNCTION_INDIRECT) {
      ze_group_count_t *tg_dim = static_cast<ze_group_count_t *>(args_buff);
      // This test failing pending fix for LOKI-551
      EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendLaunchKernelIndirect(
                                       cmd_list, function, tg_dim, signal_event,
                                       num_wait, p_wait_events));

      // Intentionally update args_buff after Launch API
      memcpy(args_buff, &th_group_dim, sizeof(ze_group_count_t));
    } else if (type == MULTIPLE_INDIRECT) {
      int *mult_out_int = static_cast<int *>(mult_out);
      int *mult_in_int = static_cast<int *>(mult_in);
      ze_group_count_t *tg_dim = static_cast<ze_group_count_t *>(args_buff);
      memcpy(mult_out_int, inpa.data(), 16 * sizeof(int));
      memcpy(mult_in_int, inpb.data(), 16 * sizeof(int));
      std::vector<ze_kernel_handle_t> function_list;
      std::vector<ze_group_count_t> arg_buffer_list;
      std::vector<uint32_t> num_launch_arg_list;
      function_list.push_back(function);
      mult_function = lzt::create_function(mod, "module_add_two_arrays");
      function_list.push_back(mult_function);

      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeKernelSetArgumentValue(mult_function, 0, sizeof(mult_out_int),
                                         &mult_out_int));
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeKernelSetArgumentValue(mult_function, 1, sizeof(mult_in_int),
                                         &mult_in_int));
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeKernelSetGroupSize(mult_function, 16, 1, 1));

      arg_buffer_list.push_back(th_group_dim);
      ze_group_count_t mult_th_group_dim;
      mult_th_group_dim.groupCountX = 16;
      mult_th_group_dim.groupCountY = 1;
      mult_th_group_dim.groupCountZ = 1;
      arg_buffer_list.push_back(mult_th_group_dim);
      uint32_t *num_launch_arg = static_cast<uint32_t *>(actual_launch);
      ze_group_count_t *mult_tg_dim =
          static_cast<ze_group_count_t *>(args_buff);

      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeCommandListAppendLaunchMultipleKernelsIndirect(
                    cmd_list, 2, function_list.data(), num_launch_arg,
                    mult_tg_dim, signal_event, num_wait, p_wait_events));

      // Intentionally update args buffer and num_args after API
      num_launch_arg[0] = 2;
      memcpy(args_buff, arg_buffer_list.data(), 2 * sizeof(ze_group_count_t));
    }

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendBarrier(cmd_list, nullptr, 0, nullptr));

    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(cmd_list));

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandQueueExecuteCommandLists(cmd_q, 1, &cmd_list, nullptr));
    // Note: Current test for Wait Events will hang on Fulsim and Cobalt
    if (signal_from_host) {
      for (uint32_t i = 0; i < num_events; i++) {
        EXPECT_EQ(ZE_RESULT_SUCCESS,
                  zeEventHostSignal(events_host_to_kernel[i]));
      }
    }

    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandQueueSynchronize(cmd_q, UINT32_MAX));

    if (signal_to_host) {
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeEventHostSynchronize(event_kernel_to_host, UINT32_MAX - 1));
    }

    int offset = 0;

    EXPECT_EQ(input_a_int[0],
              offset + ((addval * (group_size_x * th_group_dim.groupCountX) *
                         (group_size_y * th_group_dim.groupCountY) *
                         (group_size_z * th_group_dim.groupCountZ))));
    if (type == MULTIPLE_INDIRECT) {
      int *mult_out_int = static_cast<int *>(mult_out);
      for (uint32_t i = 0; i < 16; i++) {
        EXPECT_EQ(mult_out_int[i], inpa[i] + inpb[i]);
      }
      lzt::destroy_function(mult_function);
    }
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandQueueDestroy(cmd_q));
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListDestroy(cmd_list));
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

  void TearDown() override {
    for (auto mod : module_) {
      lzt::destroy_module(mod);
    }
  }

  ze_device_handle_t device_ = nullptr;
  std::vector<ze_module_handle_t> module_;
};

TEST_F(zeKernelCreateTests,
       GivenValidModuleWhenCreatingFunctionThenReturnSuccessful) {
  ze_kernel_handle_t function;

  for (auto mod : module_) {
    function =
        lzt::create_function(mod, ZE_KERNEL_FLAG_NONE, "module_add_constant");
    lzt::destroy_function(function);
    function = lzt::create_function(mod, ZE_KERNEL_FLAG_FORCE_RESIDENCY,
                                    "module_add_two_arrays");
    lzt::destroy_function(function);
  }
}

TEST_F(zeKernelCreateTests,
       GivenValidFunctionWhenSettingGroupSizeThenReturnSuccessful) {
  ze_kernel_handle_t function;
  ze_device_compute_properties_t dev_compute_properties;
  uint32_t x;
  uint32_t y;
  uint32_t z;

  for (auto mod : module_) {
    function =
        lzt::create_function(mod, ZE_KERNEL_FLAG_NONE, "module_add_constant");
    dev_compute_properties.version =
        ZE_DEVICE_COMPUTE_PROPERTIES_VERSION_CURRENT;
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeDeviceGetComputeProperties(device_, &dev_compute_properties));
    for (uint32_t x = 1; x < dev_compute_properties.maxGroupSizeX; x++) {
      EXPECT_EQ(ZE_RESULT_SUCCESS, zeKernelSetGroupSize(function, x, 1, 1));
    }
    for (uint32_t y = 1; y < dev_compute_properties.maxGroupSizeY; y++) {
      EXPECT_EQ(ZE_RESULT_SUCCESS, zeKernelSetGroupSize(function, 1, y, 1));
    }
    for (uint32_t z = 1; z < dev_compute_properties.maxGroupSizeZ; z++) {
      EXPECT_EQ(ZE_RESULT_SUCCESS, zeKernelSetGroupSize(function, 1, 1, z));
    }
    x = y = z = 1;
    while (x * y < dev_compute_properties.maxTotalGroupSize) {
      EXPECT_EQ(ZE_RESULT_SUCCESS, zeKernelSetGroupSize(function, x++, y++, 1));
    }
    x = y = z = 1;
    while (y * z < dev_compute_properties.maxTotalGroupSize) {
      EXPECT_EQ(ZE_RESULT_SUCCESS, zeKernelSetGroupSize(function, 1, y++, z++));
    }
    x = y = z = 1;
    while (x * z < dev_compute_properties.maxTotalGroupSize) {
      EXPECT_EQ(ZE_RESULT_SUCCESS, zeKernelSetGroupSize(function, x++, 1, z++));
    }
    x = y = z = 1;
    while (x * y * z < dev_compute_properties.maxTotalGroupSize) {
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeKernelSetGroupSize(function, x++, y++, z++));
    }
    lzt::destroy_function(function);
  }
}

TEST_F(zeKernelCreateTests,
       GivenValidFunctionWhenSuggestingGroupSizeThenReturnSuccessful) {
  ze_kernel_handle_t function;
  ze_device_compute_properties_t dev_compute_properties;
  uint32_t group_size_x;
  uint32_t group_size_y;
  uint32_t group_size_z;

  for (auto mod : module_) {
    function =
        lzt::create_function(mod, ZE_KERNEL_FLAG_NONE, "module_add_constant");
    dev_compute_properties.version =
        ZE_DEVICE_COMPUTE_PROPERTIES_VERSION_CURRENT;
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeDeviceGetComputeProperties(device_, &dev_compute_properties));
    group_size_x = group_size_y = group_size_z = 0;
    for (uint32_t x = UINT32_MAX; x > 0; x = x >> 1) {
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeKernelSuggestGroupSize(function, x, 1, 1, &group_size_x,
                                         &group_size_y, &group_size_z));
      EXPECT_LE(group_size_x, dev_compute_properties.maxGroupSizeX);
    }
    for (uint32_t y = UINT32_MAX; y > 0; y = y >> 1) {
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeKernelSuggestGroupSize(function, 1, y, 1, &group_size_x,
                                         &group_size_y, &group_size_z));
      EXPECT_LE(group_size_y, dev_compute_properties.maxGroupSizeY);
    }
    for (uint32_t z = UINT32_MAX; z > 0; z = z >> 1) {
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeKernelSuggestGroupSize(function, 1, 1, z, &group_size_x,
                                         &group_size_y, &group_size_z));
      EXPECT_LE(group_size_z, dev_compute_properties.maxGroupSizeZ);
    }
    for (uint32_t i = UINT32_MAX; i > 0; i = i >> 1) {
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeKernelSuggestGroupSize(function, i, i, i, &group_size_x,
                                         &group_size_y, &group_size_z));
      EXPECT_LE(group_size_x, dev_compute_properties.maxGroupSizeX);
      EXPECT_LE(group_size_y, dev_compute_properties.maxGroupSizeY);
      EXPECT_LE(group_size_z, dev_compute_properties.maxGroupSizeZ);
    }
    lzt::destroy_function(function);
  }
}

TEST_F(zeKernelCreateTests,
       GivenValidFunctionWhenSettingArgumentsThenReturnSuccessful) {

  void *input_a =
      lzt::allocate_shared_memory(16, 1, ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
                                  ZE_HOST_MEM_ALLOC_FLAG_DEFAULT, device_);
  void *input_b =
      lzt::allocate_shared_memory(16, 1, ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
                                  ZE_HOST_MEM_ALLOC_FLAG_DEFAULT, device_);
  const int addval = 10;
  int *input_a_int = static_cast<int *>(input_a);
  int *input_b_int = static_cast<int *>(input_b);
  ze_kernel_handle_t function;

  for (auto mod : module_) {
    function = lzt::create_function(mod, "module_add_constant");
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(function, 0, sizeof(input_a_int),
                                       &input_a_int));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(function, 1, sizeof(addval), &addval));

    lzt::destroy_function(function);
    function = lzt::create_function(mod, "module_add_two_arrays");
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(function, 0, sizeof(input_a_int),
                                       &input_a_int));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(function, 1, sizeof(input_b_int),
                                       &input_b_int));
    lzt::destroy_function(function);
  }
  lzt::free_memory(input_a);
  lzt::free_memory(input_b);
}

TEST_F(
    zeKernelCreateTests,
    GivenValidFunctionWhenGettingPropertiesThenReturnSuccessfulAndPropertiesAreValid) {
  ze_device_compute_properties_t dev_compute_properties;
  dev_compute_properties.version = ZE_DEVICE_COMPUTE_PROPERTIES_VERSION_CURRENT;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDeviceGetComputeProperties(device_, &dev_compute_properties));

  ze_kernel_handle_t function;
  uint32_t attribute_val;

  for (auto mod : module_) {
    function = lzt::create_function(mod, "module_add_constant");
    ze_kernel_properties_t kernel_properties;
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelGetProperties(function, &kernel_properties));
    EXPECT_LE(kernel_properties.requiredGroupSizeX,
              dev_compute_properties.maxGroupCountX);
    EXPECT_LE(kernel_properties.requiredGroupSizeY,
              dev_compute_properties.maxGroupCountY);
    EXPECT_LE(kernel_properties.requiredGroupSizeZ,
              dev_compute_properties.maxGroupCountZ);

    LOG_INFO << "Kernel Name = " << kernel_properties.name;
    LOG_INFO << "Num of Arguments = " << kernel_properties.numKernelArgs;
    LOG_INFO << "Group Size in X dim = "
             << kernel_properties.requiredGroupSizeX;
    LOG_INFO << "Group Size in Y dim = "
             << kernel_properties.requiredGroupSizeY;
    LOG_INFO << "Group Size in Z dim = "
             << kernel_properties.requiredGroupSizeZ;
    lzt::destroy_function(function);
  }
}

TEST_F(zeKernelCreateTests,
       GivenValidFunctionWhenSettingAttributesThenReturnSuccessful) {

  ze_kernel_handle_t function;

  for (auto mod : module_) {
    function = lzt::create_function(mod, "module_add_constant");
    bool trueValue = true;
    bool falseValue = false;
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetAttribute(function,
                                   ZE_KERNEL_ATTR_INDIRECT_HOST_ACCESS,
                                   sizeof(bool), &trueValue));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetAttribute(function,
                                   ZE_KERNEL_ATTR_INDIRECT_HOST_ACCESS,
                                   sizeof(bool), &falseValue));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetAttribute(function,
                                   ZE_KERNEL_ATTR_INDIRECT_DEVICE_ACCESS,
                                   sizeof(bool), &trueValue));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetAttribute(function,
                                   ZE_KERNEL_ATTR_INDIRECT_DEVICE_ACCESS,
                                   sizeof(bool), &falseValue));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetAttribute(function,
                                   ZE_KERNEL_ATTR_INDIRECT_SHARED_ACCESS,
                                   sizeof(bool), &trueValue));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetAttribute(function,
                                   ZE_KERNEL_ATTR_INDIRECT_SHARED_ACCESS,
                                   sizeof(bool), &falseValue));
    lzt::destroy_function(function);
  }
}

class zeKernelLaunchTests
    : public ::zeKernelCreateTests,
      public ::testing::WithParamInterface<enum TestType> {};

TEST_P(
    zeKernelLaunchTests,
    GivenValidFunctionWhenAppendLaunchKernelThenReturnSuccessfulAndVerifyExecution) {
  ze_device_compute_properties_t dev_compute_properties;
  dev_compute_properties.version = ZE_DEVICE_COMPUTE_PROPERTIES_VERSION_CURRENT;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDeviceGetComputeProperties(device_, &dev_compute_properties));

  uint32_t group_size_x;
  uint32_t group_size_y;
  uint32_t group_size_z;
  ze_group_count_t thread_group_dimensions;

  enum TestType test_type = GetParam();

  std::vector<int> dim = {1, 2, 3};
  std::vector<uint32_t> tg_count = {1, 2, 3, 4};
  std::vector<uint32_t> grp_size = {1, 2, 3, 4};
  std::vector<bool> sig_to_host = {false, true};
  std::vector<bool> sig_from_host = {false, true};
  if (test_type == MULTIPLE_INDIRECT) {
    dim.erase(dim.begin(), dim.begin() + 2);
    tg_count.erase(tg_count.begin(), tg_count.begin() + 3);
    grp_size.erase(grp_size.begin(), grp_size.begin() + 3);
    sig_to_host.erase(sig_to_host.begin(), sig_to_host.begin() + 1);
    sig_from_host.erase(sig_from_host.begin(), sig_from_host.begin() + 1);
  }

  uint32_t count = 0;
  for (auto mod : module_) {
    LOG_INFO << "module count = " << count;
    count++;
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
              run_test(mod, thread_group_dimensions, group_size_x, group_size_y,
                       group_size_z, sig1, sig2, test_type);
            }
          }
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(
    TestFunctionAndFunctionIndirectAndMultipleFunctionsIndirect,
    zeKernelLaunchTests,
    testing::Values(FUNCTION, FUNCTION_INDIRECT, MULTIPLE_INDIRECT));

class ModuleGetKernelNamesTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<uint32_t> {};

TEST_P(
    ModuleGetKernelNamesTests,
    GivenValidModuleWhenGettingKernelNamesThenCorrectKernelNumberAndNamesAreReturned) {
  int num = GetParam();
  uint32_t kernel_count = 0;
  const ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  std::string filename =
      std::to_string(num) + "kernel" + (num == 1 ? "" : "s") + ".spv";
  ze_module_handle_t module = lzt::create_module(device, filename);
  std::vector<const char *> names(num);

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeModuleGetKernelNames(module, &kernel_count, nullptr));
  EXPECT_EQ(kernel_count, num);
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeModuleGetKernelNames(module, &kernel_count, names.data()));

  LOG_DEBUG << kernel_count << " Kernels in Module:";
  for (uint32_t i = 0; i < kernel_count; i++) {
    LOG_DEBUG << "\t" << names[i];
    EXPECT_EQ(names[i], "kernel" + std::to_string(i + 1));
  }

  lzt::destroy_module(module);
}

INSTANTIATE_TEST_CASE_P(ModuleGetKernelNamesParamTests,
                        ModuleGetKernelNamesTests,
                        ::testing::Values(0, 1, 10, 100, 1000));

} // namespace
