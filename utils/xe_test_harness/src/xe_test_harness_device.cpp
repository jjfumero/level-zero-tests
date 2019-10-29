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

#include "xe_test_harness/xe_test_harness_device.hpp"
#include "ze_api.h"
#include "xe_utils/xe_utils.hpp"

namespace lzt = level_zero_tests;

namespace level_zero_tests {

zeDevice *zeDevice::instance_ = nullptr;

zeDevice *zeDevice::get_instance() {
  if (instance_)
    return instance_;
  instance_ = new zeDevice;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeInit(ZE_INIT_FLAG_NONE));

  instance_->device_ = lzt::get_default_device();
  return instance_;
}

ze_device_handle_t zeDevice::get_device() { return get_instance()->device_; }

zeDevice::zeDevice() { device_ = nullptr; }

uint32_t get_ze_device_count() {
  return get_ze_device_count(lzt::get_default_driver());
}

uint32_t get_ze_device_count(ze_driver_handle_t driver) {
  uint32_t count = 0;
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceGet(driver, &count, nullptr));

  return count;
}

std::vector<ze_device_handle_t> get_ze_devices() {
  return get_ze_devices(get_ze_device_count());
}

std::vector<ze_device_handle_t> get_ze_devices(uint32_t count) {
  return get_ze_devices(count, lzt::get_default_driver());
}

std::vector<ze_device_handle_t> get_ze_devices(ze_driver_handle_t driver) {
  return get_ze_devices(get_ze_device_count(), driver);
}

std::vector<ze_device_handle_t> get_ze_devices(uint32_t count,
                                               ze_driver_handle_t driver) {
  uint32_t count_out = count;
  std::vector<ze_device_handle_t> devices(count);

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceGet(driver, &count_out, devices.data()));
  if (count == get_ze_device_count())
    EXPECT_EQ(count_out, count);

  return devices;
}

uint32_t get_ze_sub_device_count(ze_device_handle_t device) {
  uint32_t count = 0;

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceGetSubDevices(device, &count, nullptr));
  return count;
}

std::vector<ze_device_handle_t> get_ze_sub_devices(ze_device_handle_t device) {
  return get_ze_sub_devices(device, get_ze_sub_device_count(device));
}

std::vector<ze_device_handle_t> get_ze_sub_devices(ze_device_handle_t device,
                                                   uint32_t count) {
  std::vector<ze_device_handle_t> sub_devices(count);

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDeviceGetSubDevices(device, &count, sub_devices.data()));
  return sub_devices;
}

ze_device_properties_t get_device_properties(ze_device_handle_t device) {
  ze_device_properties_t properties;

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceGetProperties(device, &properties));
  return properties;
}

ze_device_compute_properties_t
get_compute_properties(ze_device_handle_t device) {
  ze_device_compute_properties_t properties = {
      ZE_DEVICE_COMPUTE_PROPERTIES_VERSION_CURRENT};

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDeviceGetComputeProperties(device, &properties));
  return properties;
}

uint32_t get_memory_properties_count(ze_device_handle_t device) {
  uint32_t count = 0;

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDeviceGetMemoryProperties(device, &count, nullptr));
  return count;
}

std::vector<ze_device_memory_properties_t>
get_memory_properties(ze_device_handle_t device) {
  return get_memory_properties(device, get_memory_properties_count(device));
}

std::vector<ze_device_memory_properties_t>
get_memory_properties(ze_device_handle_t device, uint32_t count) {
  std::vector<ze_device_memory_properties_t> properties(
      count, {ZE_DEVICE_MEMORY_PROPERTIES_VERSION_CURRENT});

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDeviceGetMemoryProperties(device, &count, properties.data()));
  return properties;
}

ze_device_memory_access_properties_t
get_memory_access_properties(ze_device_handle_t device) {
  ze_device_memory_access_properties_t properties = {
      ZE_DEVICE_MEMORY_ACCESS_PROPERTIES_VERSION_CURRENT};

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDeviceGetMemoryAccessProperties(device, &properties));
  return properties;
}

ze_device_cache_properties_t get_cache_properties(ze_device_handle_t device) {
  ze_device_cache_properties_t properties = {
      ZE_DEVICE_CACHE_PROPERTIES_VERSION_CURRENT};

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceGetCacheProperties(device, &properties));
  return properties;
}

ze_device_image_properties_t get_image_properties(ze_device_handle_t device) {
  ze_device_image_properties_t properties = {
      ZE_DEVICE_IMAGE_PROPERTIES_VERSION_CURRENT};

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceGetImageProperties(device, &properties));
  return properties;
}

ze_device_p2p_properties_t get_p2p_properties(ze_device_handle_t dev1,
                                              ze_device_handle_t dev2) {
  ze_device_p2p_properties_t properties = {
      ZE_DEVICE_P2P_PROPERTIES_VERSION_CURRENT};

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDeviceGetP2PProperties(dev1, dev2, &properties));
  return properties;
}

ze_bool_t can_access_peer(ze_device_handle_t dev1, ze_device_handle_t dev2) {
  ze_bool_t can_access;

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceCanAccessPeer(dev1, dev2, &can_access));
  return can_access;
}

void set_intermediate_cache_config(ze_kernel_handle_t kernel,
                                   ze_cache_config_t config) {
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeKernelSetIntermediateCacheConfig(kernel, config));
}

void set_last_level_cache_config(ze_device_handle_t device,
                                 ze_cache_config_t config) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeDeviceSetLastLevelCacheConfig(device, config));
}

}; // namespace level_zero_tests
