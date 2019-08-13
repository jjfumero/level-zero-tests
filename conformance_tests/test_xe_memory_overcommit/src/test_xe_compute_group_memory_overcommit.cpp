/*
 * Copyright(c) 2019 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "gtest/gtest.h"

#include "utils/utils.hpp"
#include "xe_utils/xe_utils.hpp"
#include "xe_test_harness/xe_test_harness.hpp"
#include "logging/logging.hpp"

namespace cs = compute_samples;

#include "xe_driver.h"
#include "xe_module.h"
#include "xe_copy.h"
#include "xe_barrier.h"

namespace {

class xeDeviceGroupMemoryOvercommitTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<
          std::tuple<uint32_t, uint32_t, uint32_t>> {
protected:
  xe_module_handle_t create_module(const xe_device_handle_t device,
                                   const std::string path) {
    const std::vector<uint8_t> binary_file = cs::load_binary_file(path);

    LOG_INFO << "set up module description for path " << path;
    xe_module_desc_t module_description;
    module_description.version = XE_MODULE_DESC_VERSION_CURRENT;
    module_description.format = XE_MODULE_FORMAT_IL_SPIRV;
    module_description.inputSize = static_cast<uint32_t>(binary_file.size());
    module_description.pInputModule = binary_file.data();
    module_description.pBuildFlags = nullptr;

    xe_module_handle_t module = nullptr;
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeModuleCreate(device, &module_description, &module, nullptr));

    LOG_INFO << "return module";
    return module;
  }

  void run_functions(const xe_device_handle_t device, xe_module_handle_t module,
                     void *pattern_memory, size_t pattern_memory_count,
                     uint16_t sub_pattern,
                     uint64_t *host_expected_output_buffer,
                     uint64_t *gpu_expected_output_buffer,
                     uint64_t *host_found_output_buffer,
                     uint64_t *gpu_found_output_buffer, size_t output_count) {
    xe_function_desc_t fill_function_description;
    fill_function_description.version = XE_FUNCTION_DESC_VERSION_CURRENT;
    fill_function_description.flags = XE_FUNCTION_FLAG_NONE;
    fill_function_description.pFunctionName = "fill_device_memory";

    /* Prepare the fill function */
    xe_function_handle_t fill_function = nullptr;
    EXPECT_EQ(
        XE_RESULT_SUCCESS,
        xeFunctionCreate(module, &fill_function_description, &fill_function));

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetGroupSize(fill_function, 1, 1, 1));

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetArgumentValue(
                  fill_function, 0, sizeof(pattern_memory), &pattern_memory));

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetArgumentValue(fill_function, 1,
                                         sizeof(pattern_memory_count),
                                         &pattern_memory_count));
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetArgumentValue(fill_function, 2, sizeof(sub_pattern),
                                         &sub_pattern));

    xe_function_desc_t test_function_description;
    test_function_description.version = XE_FUNCTION_DESC_VERSION_CURRENT;
    test_function_description.flags = XE_FUNCTION_FLAG_NONE;
    test_function_description.pFunctionName = "test_device_memory";

    /* Prepare the test function */
    xe_function_handle_t test_function = nullptr;
    EXPECT_EQ(
        XE_RESULT_SUCCESS,
        xeFunctionCreate(module, &test_function_description, &test_function));

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetGroupSize(test_function, 1, 1, 1));

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetArgumentValue(
                  test_function, 0, sizeof(pattern_memory), &pattern_memory));

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetArgumentValue(test_function, 1,
                                         sizeof(pattern_memory_count),
                                         &pattern_memory_count));
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetArgumentValue(test_function, 2, sizeof(sub_pattern),
                                         &sub_pattern));
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetArgumentValue(test_function, 3,
                                         sizeof(gpu_expected_output_buffer),
                                         &gpu_expected_output_buffer));
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetArgumentValue(test_function, 4,
                                         sizeof(gpu_found_output_buffer),
                                         &gpu_found_output_buffer));
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeFunctionSetArgumentValue(test_function, 5, sizeof(output_count),
                                         &output_count));

    xe_command_list_desc_t command_list_description;
    command_list_description.version = XE_COMMAND_LIST_DESC_VERSION_CURRENT;

    xe_command_list_handle_t command_list = nullptr;
    EXPECT_EQ(
        XE_RESULT_SUCCESS,
        xeCommandListCreate(device, &command_list_description, &command_list));

    xe_thread_group_dimensions_t thread_group_dimensions = {1, 1, 1};

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandListAppendMemoryCopy(
                  command_list, gpu_expected_output_buffer,
                  host_expected_output_buffer, output_count * sizeof(uint64_t),
                  nullptr, 0, nullptr));
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandListAppendMemoryCopy(
                  command_list, gpu_found_output_buffer,
                  host_found_output_buffer, output_count * sizeof(uint64_t),
                  nullptr, 0, nullptr));

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandListAppendLaunchFunction(command_list, fill_function,
                                                &thread_group_dimensions,
                                                nullptr, 0, nullptr));

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandListAppendBarrier(command_list, nullptr, 0, nullptr));

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandListAppendLaunchFunction(command_list, test_function,
                                                &thread_group_dimensions,
                                                nullptr, 0, nullptr));

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandListAppendBarrier(command_list, nullptr, 0, nullptr));

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandListAppendMemoryCopy(
                  command_list, host_expected_output_buffer,
                  gpu_expected_output_buffer, output_count * sizeof(uint64_t),
                  nullptr, 0, nullptr));

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandListAppendMemoryCopy(
                  command_list, host_found_output_buffer,
                  gpu_found_output_buffer, output_count * sizeof(uint64_t),
                  nullptr, 0, nullptr));

    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandListAppendBarrier(command_list, nullptr, 0, nullptr));

    EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListClose(command_list));

    const uint32_t command_queue_id = 0;
    xe_command_queue_desc_t command_queue_description;
    command_queue_description.version = XE_COMMAND_QUEUE_DESC_VERSION_CURRENT;
    command_queue_description.ordinal = command_queue_id;
    command_queue_description.mode = XE_COMMAND_QUEUE_MODE_ASYNCHRONOUS;

    xe_command_queue_handle_t command_queue = nullptr;
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandQueueCreate(device, &command_queue_description,
                                   &command_queue));

    EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandQueueExecuteCommandLists(
                                     command_queue, 1, &command_list, nullptr));
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeCommandQueueSynchronize(command_queue, UINT32_MAX));

    EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandQueueDestroy(command_queue));
    EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListDestroy(command_list));
    EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionDestroy(fill_function));
    EXPECT_EQ(XE_RESULT_SUCCESS, xeFunctionDestroy(test_function));
  }

  void collect_device_groups_info() {

    uint32_t group_count = 0;

    LOG_INFO << "collect device group information";

    xe_device_group_handle_t *device_group_handles;
    EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupGet(&group_count, NULL));
    EXPECT_NE(0, group_count);

    device_group_handles = new xe_device_group_handle_t[group_count];
    ASSERT_NE(nullptr, device_group_handles);
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeDeviceGroupGet(&group_count, device_group_handles));

    LOG_INFO << "number of device groups " << group_count;

    struct DeviceGroupInfo *device_group_info;
    device_group_info = new struct DeviceGroupInfo[group_count];

    for (uint32_t i = 0; i < group_count; i++) {

      device_group_info[i].device_group_handle = device_group_handles[i];

      uint32_t device_count = 0;
      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeDeviceGroupGetDevices(device_group_handles[i], &device_count,
                                        NULL));
      EXPECT_NE(0, device_count);
      device_group_info[i].device_handles =
          new xe_device_handle_t[device_count];
      ASSERT_NE(nullptr, device_group_info[i].device_handles);
      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeDeviceGroupGetDevices(device_group_handles[i], &device_count,
                                        device_group_info[i].device_handles));
      device_group_info[i].number_device_handles = device_count;

      LOG_INFO << "there are " << device_count << " devices in device group "
               << i;

      /* one xe_device_properties_t for each device in the device group */
      device_group_info[i].device_properties =
          new xe_device_properties_t[device_count];
      ASSERT_NE(nullptr, device_group_info[i].device_properties);
      for (uint32_t j; j < device_count; j++) {
        device_group_info[i].device_properties[j].version =
            XE_DEVICE_PROPERTIES_VERSION_CURRENT;
      }

      /* The information in xe_device_properties_t is not complete yet */
      EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupGetDeviceProperties(
                                       device_group_handles[i],
                                       device_group_info[i].device_properties));
      for (uint32_t j = 0; j < device_count; j++) {
        LOG_INFO << "xeDevicegroupGetDeviceProperties device " << j;
        LOG_INFO << " name " << device_group_info[i].device_properties[j].name
                 << " numTiles "
                 << device_group_info[i].device_properties[j].numTiles
                 << " type "
                 << ((device_group_info[i].device_properties[j].type ==
                      XE_DEVICE_TYPE_GPU)
                         ? " GPU "
                         : " FPGA ");
      }

      /* one xe_device_compute_properties_t for device in the device group */
      device_group_info[i].device_compute_properties =
          new xe_device_compute_properties_t[device_count];
      ASSERT_NE(nullptr, device_group_info[i].device_compute_properties);
      for (uint32_t j = 0; j < device_count; j++) {
        device_group_info[i].device_compute_properties[j].version =
            XE_DEVICE_COMPUTE_PROPERTIES_VERSION_CURRENT;
      }
      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeDeviceGroupGetComputeProperties(
                    device_group_handles[i],
                    device_group_info[i].device_compute_properties));

      for (uint32_t j = 0; j < device_count; j++) {
        LOG_INFO << "xeDeviceGroupGetComputeProperties device " << j;
        LOG_INFO << "maxSharedLocalMemory "
                 << device_group_info[i]
                        .device_compute_properties[j]
                        .maxSharedLocalMemory;
      }

#ifdef BUG
      /* one xe_device_memory_properties_t per TILE in the device group?*/
      uint32_t device_memory_properties_count = 0;
      EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupGetMemoryProperties(
                                       device_group_handles[i],
                                       &device_memory_properties_count, NULL));
      EXPECT_NE(0, device_memory_properties_count);
      device_group_info[i].device_memory_properties =
          new xe_device_memory_properties_t[device_memory_properties_count];
      ASSERT_NE(nullptr, device_group_info[i].device_memory_properties);
      for (uint32_t j = 0; j < device_memory_properties_count; j++)
        device_group_info[i].device_memory_properties[j].version =
            XE_DEVICE_MEMORY_PROPERTIES_VERSION_CURRENT;
      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeDeviceGroupGetMemoryProperties(
                    device_group_handles[i], &device_memory_properties_count,
                    device_group_info[i].device_memory_properties));
      device_group_info[i].number_device_memory_properties =
          device_memory_properties_count;
#else  /* BUG */
      /*
       * This interface doesn't work properly yet, and it returns
       * incorrect information (e.g. totalSize)
       */
      uint32_t device_memory_properties_count = 1;
      device_group_info[i].device_memory_properties =
          new xe_device_memory_properties_t[device_memory_properties_count];
      ASSERT_NE(nullptr, device_group_info[i].device_memory_properties);
      for (uint32_t j = 0; j < device_memory_properties_count; j++)
        device_group_info[i].device_memory_properties[j].version =
            XE_DEVICE_MEMORY_PROPERTIES_VERSION_CURRENT;
      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeDeviceGroupGetMemoryProperties(
                    device_group_handles[i], &device_memory_properties_count,
                    device_group_info[i].device_memory_properties));
      device_group_info[i].number_device_memory_properties =
          device_memory_properties_count;
      for (uint32_t j = 0; j < device_memory_properties_count; j++) {
        LOG_INFO << "xe_device_memory_properties_t device " << j;
        LOG_INFO << "totalSize "
                 << device_group_info[i].device_memory_properties[j].totalSize;
      }
#endif /* BUG */

#ifdef UNSUPPORTED

      /* one xe_device_memory_access_properties_t per device */
      device_group_info[i].device_memory_access_properties =
          new xe_device_memory_access_properties_t[device_count];
      ASSERT_NE(nullptr, device_group_info[i].device_memory_access_properties);
      for (uint32_t j = 0; j < device_count; j++)
        device_group_info[i].device_memory_access_properties[j].version =
            XE_DEVICE_MEMORY_ACCESS_PROPERTIES_VERSION_CURRENT;
      EXPECT_EQ(XE_RESULT_SUCCESS,
                xeDeviceGroupGetMemoryAccessProperties(
                    device_group_handles[i],
                    device_group_info[i].device_memory_access_properties));
#endif /* UNSUPPORTED */
    }

    DeviceGroupInfo_ = device_group_info;
    DeviceGroupInfoCount_ = group_count;

    delete[] device_group_handles;
  }

  void free_device_groups_info() {

    LOG_INFO << "free device group information";

    for (uint32_t i = 0; i < DeviceGroupInfoCount_; i++) {
      delete[] DeviceGroupInfo_[i].device_handles;
      delete[] DeviceGroupInfo_[i].device_properties;
      delete[] DeviceGroupInfo_[i].device_compute_properties;
      delete[] DeviceGroupInfo_[i].device_memory_properties;
#ifdef UNSUPPORTED
      delete[] DeviceGroupInfo_[i].device_memory_access_properties;
#endif /* UNSUPPORTED */
    }

    delete[] DeviceGroupInfo_;
  }

  typedef struct DeviceGroupInfo {
    xe_device_group_handle_t device_group_handle;

    uint32_t number_device_handles;
    xe_device_handle_t *device_handles;

    uint32_t number_device_properties;
    xe_device_properties_t *device_properties;

    uint32_t number_device_compute_properties;
    xe_device_compute_properties_t *device_compute_properties;

    /*
     * uint64_t totalSize
     * Uncertain how many xe_device_memory_properties_t there are.
     */
    uint32_t number_device_memory_properties;
    xe_device_memory_properties_t *device_memory_properties;

    /*
     * sharedSystemAllocCapabilities
     * 	XE_MEMORY_ACCESS_NONE, XE_MEMORY_ACCESS, XE_MEMORY_ATOMIC_ACCESS
     * 	XE_MEMORY_CONCURENT_ACCESS, XE_MEMORY_CONCURRENT_ATOMIC_ACCESS
     *
     * 	There is one xe_device_memory_access_properties_t per device handle
     */
    xe_device_memory_access_properties_t *device_memory_access_properties;
  } DeviceGroupInfo_t;

  DeviceGroupInfo_t *DeviceGroupInfo_;
  uint32_t DeviceGroupInfoCount_;

  typedef struct MemoryTestArguments {
    /* The index into the device group array to test */
    uint32_t device_group_index;
    /* The index array of devices in selected device group */
    uint32_t device_in_group_index;
    /*
     * number of multiples of maxSharedLocalMemory
     * in xeDeviceGroupGetComputeProperties_t
     * Will always be arounded down to a uint64_t
     * multiple.
     */
    uint32_t memory_size_multiple;
  } MemoryTestArguments_t;

  uint32_t use_this_ordinal_on_device_ = 0;
  uint32_t output_count_ = 64;
  size_t output_size_ = output_count_ * sizeof(uint64_t);
};

TEST_P(
    xeDeviceGroupMemoryOvercommitTests,
    GivenDeviceMemoryWhenAllocationSizeLargerThanDeviceMaxMemoryThenMemoryIsPagedOffAndOnTheDevice) {

  MemoryTestArguments_t test_arguments = {
      std::get<0>(GetParam()), // device group index
      std::get<1>(GetParam()), // device index within device group
      std::get<2>(GetParam())  // memory size multiple, rounded up to uint16_t
  };

  uint32_t device_group_index = test_arguments.device_group_index;
  uint32_t device_in_group_index = test_arguments.device_in_group_index;
  uint32_t memory_size_multiple = test_arguments.memory_size_multiple;

  LOG_INFO << "TEST ARGUMENTS "
           << "device_group_index " << device_group_index
           << " device_in_group_index " << device_in_group_index
           << " memory_size_multiple " << memory_size_multiple;

  collect_device_groups_info();

  EXPECT_LT(device_group_index, DeviceGroupInfoCount_);
  EXPECT_LT(device_in_group_index,
            DeviceGroupInfo_[device_group_index].number_device_handles);

  DeviceGroupInfo_t *device_group_info = &DeviceGroupInfo_[device_group_index];

  xe_device_group_handle_t device_group_handle =
      device_group_info->device_group_handle;

  xe_device_handle_t device_handle =
      device_group_info->device_handles[device_in_group_index];

  uint32_t maxSharedLocalMemory =
      device_group_info->device_compute_properties[0].maxSharedLocalMemory;
  uint64_t totalSize = device_group_info->device_memory_properties[0].totalSize;

  size_t pattern_memory_size = memory_size_multiple * maxSharedLocalMemory;
  size_t pattern_memory_count = pattern_memory_size >> 3; // array of uint64_t

  uint64_t *gpu_pattern_buffer;
  gpu_pattern_buffer =
      (uint64_t *)compute_samples::allocate_device_group_device_memory(
          device_group_handle, device_handle, XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
          use_this_ordinal_on_device_, pattern_memory_size, 8);

  uint64_t *gpu_expected_output_buffer;
  gpu_expected_output_buffer =
      (uint64_t *)compute_samples::allocate_device_group_device_memory(
          device_group_handle, device_handle, XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
          use_this_ordinal_on_device_, output_size_, 8);
  uint64_t *host_expected_output_buffer = new uint64_t[output_count_];
  std::fill(host_expected_output_buffer,
            host_expected_output_buffer + output_count_, 0);

  uint64_t *gpu_found_output_buffer;
  gpu_found_output_buffer =
      (uint64_t *)compute_samples::allocate_device_group_device_memory(
          device_group_handle, device_handle, XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
          use_this_ordinal_on_device_, output_size_, 8);
  uint64_t *host_found_output_buffer = new uint64_t[output_size_];
  std::fill(host_found_output_buffer, host_found_output_buffer + output_count_,
            0);

  uint16_t pattern_base = std::rand() % 0xFFFF;

  LOG_INFO << "PREPARE TO RUN START";
  LOG_INFO << "totalSize " << totalSize;
  LOG_INFO << "maxSharedLocalMemory " << maxSharedLocalMemory;
  LOG_INFO << "gpu_pattern_buffer " << gpu_pattern_buffer;
  LOG_INFO << "pattern_memory_count " << pattern_memory_count;
  LOG_INFO << "pattern_memory_size " << pattern_memory_size;
  LOG_INFO << "pattern_base " << pattern_base;
  LOG_INFO << "gpu_expected_output_buffer " << gpu_expected_output_buffer;
  LOG_INFO << "host_expected_output_buffer " << host_expected_output_buffer;
  LOG_INFO << "gpu_found_output_buffer " << gpu_found_output_buffer;
  LOG_INFO << "host_found_output_buffer " << host_found_output_buffer;
  LOG_INFO << "output count " << output_count_;
  LOG_INFO << "output size " << output_size_;
  LOG_INFO << "PREPARE TO RUN END";

  LOG_INFO << "call create module";
  xe_module_handle_t module_handle =
      create_module(device_handle, "test_xe_fill_device_memory.spv");

  LOG_INFO << "call run_functions";
  run_functions(device_handle, module_handle, gpu_pattern_buffer,
                pattern_memory_count, pattern_base, host_expected_output_buffer,
                gpu_expected_output_buffer, host_found_output_buffer,
                gpu_found_output_buffer, output_count_);

  LOG_INFO << "call free memory";
  compute_samples::free_device_group_memory(device_group_handle,
                                            gpu_pattern_buffer);
  compute_samples::free_device_group_memory(device_group_handle,
                                            gpu_expected_output_buffer);
  compute_samples::free_device_group_memory(device_group_handle,
                                            gpu_found_output_buffer);

  LOG_INFO << "call destroy module";
  EXPECT_EQ(XE_RESULT_SUCCESS, xeModuleDestroy(module_handle));

  LOG_INFO << "check output buffer";
  bool memory_test_failure = false;
  for (uint32_t i = 0; i < output_count_; i++) {
    if (host_expected_output_buffer[i] || host_found_output_buffer[i]) {
      LOG_INFO << "Index of difference " << i << " found "
               << host_found_output_buffer[i] << " expected "
               << host_expected_output_buffer[i];
      memory_test_failure = true;
    }
  }

  EXPECT_EQ(false, memory_test_failure);

  free_device_groups_info();
  delete host_expected_output_buffer;
  delete host_found_output_buffer;
}

INSTANTIATE_TEST_CASE_P(TestAllInputPermuntations,
                        xeDeviceGroupMemoryOvercommitTests,
                        ::testing::Combine(::testing::Values(0),
                                           ::testing::Values(0),
                                           ::testing::Values(1, 2, 4)));

} // namespace

// TODO: Probably replace maxSharedLocalMemory with totalSize
