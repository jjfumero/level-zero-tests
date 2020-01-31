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
#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"
#include "logging/logging.hpp"

namespace lzt = level_zero_tests;

#include "ze_api.h"

namespace {

class zeDriverMemoryOvercommitTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<
          std::tuple<uint32_t, uint32_t, uint32_t>> {
protected:
  ze_module_handle_t create_module(const ze_device_handle_t device,
                                   const std::string path) {
    const std::vector<uint8_t> binary_file = lzt::load_binary_file(path);

    LOG_INFO << "set up module description for path " << path;
    ze_module_desc_t module_description;
    module_description.version = ZE_MODULE_DESC_VERSION_CURRENT;
    module_description.format = ZE_MODULE_FORMAT_IL_SPIRV;
    module_description.inputSize = static_cast<uint32_t>(binary_file.size());
    module_description.pInputModule = binary_file.data();
    module_description.pBuildFlags = nullptr;

    ze_module_handle_t module = nullptr;
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeModuleCreate(device, &module_description, &module, nullptr));

    LOG_INFO << "return module";
    return module;
  }

  void run_functions(const ze_device_handle_t device, ze_module_handle_t module,
                     void *pattern_memory, size_t pattern_memory_count,
                     uint16_t sub_pattern,
                     uint64_t *host_expected_output_buffer,
                     uint64_t *gpu_expected_output_buffer,
                     uint64_t *host_found_output_buffer,
                     uint64_t *gpu_found_output_buffer, size_t output_count) {
    ze_kernel_desc_t fill_function_description;
    fill_function_description.version = ZE_KERNEL_DESC_VERSION_CURRENT;
    fill_function_description.flags = ZE_KERNEL_FLAG_NONE;
    fill_function_description.pKernelName = "fill_device_memory";

    /* Prepare the fill function */
    ze_kernel_handle_t fill_function = nullptr;
    EXPECT_EQ(
        ZE_RESULT_SUCCESS,
        zeKernelCreate(module, &fill_function_description, &fill_function));

    EXPECT_EQ(ZE_RESULT_SUCCESS, zeKernelSetGroupSize(fill_function, 1, 1, 1));

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(fill_function, 0, sizeof(pattern_memory),
                                       &pattern_memory));

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(fill_function, 1,
                                       sizeof(pattern_memory_count),
                                       &pattern_memory_count));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(fill_function, 2, sizeof(sub_pattern),
                                       &sub_pattern));

    ze_kernel_desc_t test_function_description;
    test_function_description.version = ZE_KERNEL_DESC_VERSION_CURRENT;
    test_function_description.flags = ZE_KERNEL_FLAG_NONE;
    test_function_description.pKernelName = "test_device_memory";

    /* Prepare the test function */
    ze_kernel_handle_t test_function = nullptr;
    EXPECT_EQ(
        ZE_RESULT_SUCCESS,
        zeKernelCreate(module, &test_function_description, &test_function));

    EXPECT_EQ(ZE_RESULT_SUCCESS, zeKernelSetGroupSize(test_function, 1, 1, 1));

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(test_function, 0, sizeof(pattern_memory),
                                       &pattern_memory));

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(test_function, 1,
                                       sizeof(pattern_memory_count),
                                       &pattern_memory_count));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(test_function, 2, sizeof(sub_pattern),
                                       &sub_pattern));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(test_function, 3,
                                       sizeof(gpu_expected_output_buffer),
                                       &gpu_expected_output_buffer));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(test_function, 4,
                                       sizeof(gpu_found_output_buffer),
                                       &gpu_found_output_buffer));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeKernelSetArgumentValue(test_function, 5, sizeof(output_count),
                                       &output_count));

    ze_command_list_desc_t command_list_description;
    command_list_description.version = ZE_COMMAND_LIST_DESC_VERSION_CURRENT;

    ze_command_list_handle_t command_list = nullptr;
    EXPECT_EQ(
        ZE_RESULT_SUCCESS,
        zeCommandListCreate(device, &command_list_description, &command_list));

    ze_group_count_t thread_group_dimensions = {1, 1, 1};

    lzt::append_memory_copy(command_list, gpu_expected_output_buffer,
                            host_expected_output_buffer,
                            output_count * sizeof(uint64_t), nullptr);
    lzt::append_memory_copy(command_list, gpu_found_output_buffer,
                            host_found_output_buffer,
                            output_count * sizeof(uint64_t), nullptr);

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendLaunchKernel(command_list, fill_function,
                                              &thread_group_dimensions, nullptr,
                                              0, nullptr));

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendBarrier(command_list, nullptr, 0, nullptr));

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendLaunchKernel(command_list, test_function,
                                              &thread_group_dimensions, nullptr,
                                              0, nullptr));

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendBarrier(command_list, nullptr, 0, nullptr));

    lzt::append_memory_copy(command_list, host_expected_output_buffer,
                            gpu_expected_output_buffer,
                            output_count * sizeof(uint64_t), nullptr);

    lzt::append_memory_copy(command_list, host_found_output_buffer,
                            gpu_found_output_buffer,
                            output_count * sizeof(uint64_t), nullptr);

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendBarrier(command_list, nullptr, 0, nullptr));

    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(command_list));

    const uint32_t command_queue_id = 0;
    ze_command_queue_desc_t command_queue_description;
    command_queue_description.version = ZE_COMMAND_QUEUE_DESC_VERSION_CURRENT;
    command_queue_description.ordinal = command_queue_id;
    command_queue_description.mode = ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS;

    ze_command_queue_handle_t command_queue = nullptr;
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandQueueCreate(device, &command_queue_description,
                                   &command_queue));

    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandQueueExecuteCommandLists(
                                     command_queue, 1, &command_list, nullptr));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandQueueSynchronize(command_queue, UINT32_MAX));

    lzt::destroy_command_queue(command_queue);
    lzt::destroy_command_list(command_list);
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeKernelDestroy(fill_function));
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeKernelDestroy(test_function));
  }

  void collect_drivers_info() {

    uint32_t driver_count = 0;

    LOG_INFO << "collect driver information";

    ze_driver_handle_t *driver_handles;
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeDriverGet(&driver_count, NULL));
    EXPECT_NE(0, driver_count);

    driver_handles = new ze_driver_handle_t[driver_count];
    ASSERT_NE(nullptr, driver_handles);
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeDriverGet(&driver_count, driver_handles));

    LOG_INFO << "number of drivers " << driver_count;

    struct DriverInfo *driver_info;
    driver_info = new struct DriverInfo[driver_count];

    for (uint32_t i = 0; i < driver_count; i++) {

      driver_info[i].driver_handle = driver_handles[i];

      uint32_t device_count = 0;
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeDeviceGet(driver_handles[i], &device_count, NULL));
      EXPECT_NE(0, device_count);
      driver_info[i].device_handles = new ze_device_handle_t[device_count];
      ASSERT_NE(nullptr, driver_info[i].device_handles);
      EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceGet(driver_handles[i], &device_count,
                                               driver_info[i].device_handles));
      driver_info[i].number_device_handles = device_count;

      LOG_INFO << "there are " << device_count << " devices in driver " << i;

      /* one ze_device_properties_t for each device in the driver */
      driver_info[i].device_properties =
          new ze_device_properties_t[device_count];
      ASSERT_NE(nullptr, driver_info[i].device_properties);
      for (uint32_t j; j < device_count; j++) {
        driver_info[i].device_properties[j].version =
            ZE_DEVICE_PROPERTIES_VERSION_CURRENT;
      }

      for (uint32_t j = 0; j < device_count; j++) {
        /* The information in ze_device_properties_t is not complete yet */
        EXPECT_EQ(ZE_RESULT_SUCCESS,
                  zeDeviceGetProperties(driver_info[i].device_handles[j],
                                        &driver_info[i].device_properties[j]));
        LOG_INFO << "zeDeviceGetProperties device " << j;
        LOG_INFO << " name " << driver_info[i].device_properties[j].name
                 << " numTiles " << driver_info[i].device_properties[j].numTiles
                 << " type "
                 << ((driver_info[i].device_properties[j].type ==
                      ZE_DEVICE_TYPE_GPU)
                         ? " GPU "
                         : " FPGA ");
      }

      /* one ze_device_compute_properties_t for device in the driver */
      driver_info[i].device_compute_properties =
          new ze_device_compute_properties_t[device_count];
      ASSERT_NE(nullptr, driver_info[i].device_compute_properties);
      for (uint32_t j = 0; j < device_count; j++) {
        driver_info[i].device_compute_properties[j].version =
            ZE_DEVICE_COMPUTE_PROPERTIES_VERSION_CURRENT;
      }

      for (uint32_t j = 0; j < device_count; j++) {
        EXPECT_EQ(ZE_RESULT_SUCCESS,
                  zeDeviceGetComputeProperties(
                      driver_info[i].device_handles[j],
                      &driver_info[i].device_compute_properties[j]));
        LOG_INFO << "zeDeviceGetComputeProperties device " << j;
        LOG_INFO
            << "maxSharedLocalMemory "
            << driver_info[i].device_compute_properties[j].maxSharedLocalMemory;
      }

#ifdef BUG
      /* one ze_device_memory_properties_t per TILE in the driver?*/
      for (uint32_t j = 0; j < device_count; j++) {
        uint32_t device_memory_properties_count = 0;
        EXPECT_EQ(ZE_RESULT_SUCCESS,
                  zeDeviceGetMemoryProperties(driver_info[i].device_handles[j],
                                              &device_memory_properties_count,
                                              NULL));
        EXPECT_NE(0, device_memory_properties_count);
        driver_info[i].device_memory_properties =
            new ze_device_memory_properties_t[device_memory_properties_count];
        ASSERT_NE(nullptr, driver_info[i].device_memory_properties);
        for (uint32_t k = 0; k < device_memory_properties_count; k++) {
          driver_info[i].device_memory_properties[k].version =
              ZE_DEVICE_MEMORY_PROPERTIES_VERSION_CURRENT;
        }
        EXPECT_EQ(ZE_RESULT_SUCCESS,
                  zeDeviceGetMemoryProperties(
                      driver_info[i].device_handles[j],
                      &device_memory_properties_count,
                      &driver_info[i].device_memory_properties[j]));
        driver_info[i].number_device_memory_properties =
            device_memory_properties_count;
      }
#else  /* BUG */
      /*
       * This interface doesn't work properly yet, and it returns
       * incorrect information (e.g. totalSize)
       */
      uint32_t device_memory_properties_count = 1;
      driver_info[i].device_memory_properties =
          new ze_device_memory_properties_t[device_memory_properties_count];
      ASSERT_NE(nullptr, driver_info[i].device_memory_properties);
      for (uint32_t j = 0; j < device_memory_properties_count; j++) {
        driver_info[i].device_memory_properties[j].version =
            ZE_DEVICE_MEMORY_PROPERTIES_VERSION_CURRENT;
        EXPECT_EQ(ZE_RESULT_SUCCESS,
                  zeDeviceGetMemoryProperties(
                      driver_info[i].device_handles[j],
                      &device_memory_properties_count,
                      &driver_info[i].device_memory_properties[j]));
        LOG_INFO << "ze_device_memory_properties_t device " << j;
        LOG_INFO << "totalSize "
                 << driver_info[i].device_memory_properties[j].totalSize;
      }
#endif /* BUG */

#ifdef UNSUPPORTED

      /* one ze_device_memory_access_properties_t per device */
      driver_info[i].device_memory_access_properties =
          new ze_device_memory_access_properties_t[device_count];
      ASSERT_NE(nullptr, driver_info[i].device_memory_access_properties);
      for (uint32_t j = 0; j < device_count; j++) {
        driver_info[i].device_memory_access_properties[j].version =
            ZE_DEVICE_MEMORY_ACCESS_PROPERTIES_VERSION_CURRENT;
        EXPECT_EQ(ZE_RESULT_SUCCESS,
                  zeDeviceGetMemoryAccessProperties(
                      driver_info[i].device_handles[j],
                      driver_info[i].device_memory_access_properties[j]));
      }
#endif /* UNSUPPORTED */
    }

    DriverInfo_ = driver_info;
    DriverInfoCount_ = driver_count;

    delete[] driver_handles;
  }

  void free_drivers_info() {

    LOG_INFO << "free driver information";

    for (uint32_t i = 0; i < DriverInfoCount_; i++) {
      delete[] DriverInfo_[i].device_handles;
      delete[] DriverInfo_[i].device_properties;
      delete[] DriverInfo_[i].device_compute_properties;
      delete[] DriverInfo_[i].device_memory_properties;
#ifdef UNSUPPORTED
      delete[] DriverInfo_[i].device_memory_access_properties;
#endif /* UNSUPPORTED */
    }

    delete[] DriverInfo_;
  }

  typedef struct DriverInfo {
    ze_driver_handle_t driver_handle;

    uint32_t number_device_handles;
    ze_device_handle_t *device_handles;

    uint32_t number_device_properties;
    ze_device_properties_t *device_properties;

    uint32_t number_device_compute_properties;
    ze_device_compute_properties_t *device_compute_properties;

    /*
     * uint64_t totalSize
     * Uncertain how many ze_device_memory_properties_t there are.
     */
    uint32_t number_device_memory_properties;
    ze_device_memory_properties_t *device_memory_properties;

    /*
     * sharedSystemAllocCapabilities
     * 	ZE_MEMORY_ACCESS_NONE, ZE_MEMORY_ACCESS, ZE_MEMORY_ATOMIC_ACCESS
     * 	ZE_MEMORY_CONCURENT_ACCESS, ZE_MEMORY_CONCURRENT_ATOMIC_ACCESS
     *
     * 	There is one ze_device_memory_access_properties_t per device handle
     */
    ze_device_memory_access_properties_t *device_memory_access_properties;
  } DriverInfo_t;

  DriverInfo_t *DriverInfo_;
  uint32_t DriverInfoCount_;

  typedef struct MemoryTestArguments {
    /* The index into the driver array to test */
    uint32_t driver_index;
    /* The index array of devices in selected driver */
    uint32_t device_in_driver_index;
    /*
     * number of multiples of maxSharedLocalMemory
     * in zeDriverGetComputeProperties_t
     * Will always be rounded down to a uint64_t
     * multiple.
     */
    uint32_t memory_size_multiple;
  } MemoryTestArguments_t;

  uint32_t use_this_ordinal_on_device_ = 0;
  uint32_t output_count_ = 64;
  size_t output_size_ = output_count_ * sizeof(uint64_t);
};

TEST_P(
    zeDriverMemoryOvercommitTests,
    GivenDeviceMemoryWhenAllocationSizeLargerThanDeviceMaxMemoryThenMemoryIsPagedOffAndOnTheDevice) {

  MemoryTestArguments_t test_arguments = {
      std::get<0>(GetParam()), // driver index
      std::get<1>(GetParam()), // device index within driver
      std::get<2>(GetParam())  // memory size multiple, rounded up to uint16_t
  };

  uint32_t driver_index = test_arguments.driver_index;
  uint32_t device_in_driver_index = test_arguments.device_in_driver_index;
  uint32_t memory_size_multiple = test_arguments.memory_size_multiple;

  LOG_INFO << "TEST ARGUMENTS "
           << "driver_index " << driver_index << " device_in_driver_index "
           << device_in_driver_index << " memory_size_multiple "
           << memory_size_multiple;

  collect_drivers_info();

  EXPECT_LT(driver_index, DriverInfoCount_);
  EXPECT_LT(device_in_driver_index,
            DriverInfo_[driver_index].number_device_handles);

  DriverInfo_t *driver_info = &DriverInfo_[driver_index];

  ze_driver_handle_t driver_handle = driver_info->driver_handle;

  ze_device_handle_t device_handle =
      driver_info->device_handles[device_in_driver_index];

  uint32_t maxSharedLocalMemory =
      driver_info->device_compute_properties[0].maxSharedLocalMemory;
  uint64_t totalSize = driver_info->device_memory_properties[0].totalSize;

  size_t pattern_memory_size = memory_size_multiple * maxSharedLocalMemory;
  size_t pattern_memory_count = pattern_memory_size >> 3; // array of uint64_t

  uint64_t *gpu_pattern_buffer;
  gpu_pattern_buffer = (uint64_t *)level_zero_tests::allocate_device_memory(
      pattern_memory_size, 8, ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
      use_this_ordinal_on_device_, device_handle, driver_handle);

  uint64_t *gpu_expected_output_buffer;
  gpu_expected_output_buffer =
      (uint64_t *)level_zero_tests::allocate_device_memory(
          output_size_, 8, ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
          use_this_ordinal_on_device_, device_handle, driver_handle);
  uint64_t *host_expected_output_buffer = new uint64_t[output_count_];
  std::fill(host_expected_output_buffer,
            host_expected_output_buffer + output_count_, 0);

  uint64_t *gpu_found_output_buffer;
  gpu_found_output_buffer =
      (uint64_t *)level_zero_tests::allocate_device_memory(
          output_size_, 8, ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT,
          use_this_ordinal_on_device_, device_handle, driver_handle);
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
  ze_module_handle_t module_handle =
      create_module(device_handle, "test_fill_device_memory.spv");

  LOG_INFO << "call run_functions";
  run_functions(device_handle, module_handle, gpu_pattern_buffer,
                pattern_memory_count, pattern_base, host_expected_output_buffer,
                gpu_expected_output_buffer, host_found_output_buffer,
                gpu_found_output_buffer, output_count_);

  LOG_INFO << "call free memory";
  level_zero_tests::free_memory(driver_handle, gpu_pattern_buffer);
  level_zero_tests::free_memory(driver_handle, gpu_expected_output_buffer);
  level_zero_tests::free_memory(driver_handle, gpu_found_output_buffer);

  LOG_INFO << "call destroy module";
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeModuleDestroy(module_handle));

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

  free_drivers_info();
  delete host_expected_output_buffer;
  delete host_found_output_buffer;
}

INSTANTIATE_TEST_CASE_P(TestAllInputPermuntations,
                        zeDriverMemoryOvercommitTests,
                        ::testing::Combine(::testing::Values(0),
                                           ::testing::Values(0),
                                           ::testing::Values(1, 2, 4)));

} // namespace

// TODO: Probably replace maxSharedLocalMemory with totalSize
