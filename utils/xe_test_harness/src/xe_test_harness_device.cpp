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
#include "xe_driver.h"
#include "xe_utils/xe_utils.hpp"

namespace lzt = level_zero_tests;

namespace level_zero_tests {

xeDevice *xeDevice::instance_ = nullptr;

xeDevice *xeDevice::get_instance() {
  if (instance_)
    return instance_;
  instance_ = new xeDevice;
  EXPECT_EQ(XE_RESULT_SUCCESS, xeInit(XE_INIT_FLAG_NONE));
  instance_->device_ = level_zero_tests::get_default_device();
  return instance_;
}

xe_device_handle_t xeDevice::get_device() { return get_instance()->device_; }

xeDevice::xeDevice() { device_ = nullptr; }

uint32_t get_xe_device_group_count() {
  uint32_t count = 0;
  EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupGet(&count, nullptr));

  return count;
}

std::vector<xe_device_group_handle_t> get_xe_device_groups() {
  return get_xe_device_groups(get_xe_device_group_count());
}

std::vector<xe_device_group_handle_t> get_xe_device_groups(uint32_t count) {
  uint32_t count_out = count;
  std::vector<xe_device_group_handle_t> device_groups(count);

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGet(&count_out, device_groups.data()));
  if (count == get_xe_device_group_count())
    EXPECT_EQ(count_out, count);

  return device_groups;
}

uint32_t get_xe_device_count(xe_device_group_handle_t device_group) {
  uint32_t count = 0;

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetDevices(device_group, &count, nullptr));
  return count;
}

std::vector<xe_device_handle_t>
get_xe_devices(xe_device_group_handle_t device_group) {
  return get_xe_devices(device_group, get_xe_device_count(device_group));
}

std::vector<xe_device_handle_t>
get_xe_devices(xe_device_group_handle_t device_group, uint32_t count) {
  uint32_t count_out = count;
  std::vector<xe_device_handle_t> devices(count);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetDevices(device_group, &count_out, devices.data()));

  if (count == get_xe_device_count(device_group))
    EXPECT_EQ(count_out, count);
  return devices;
}

uint32_t get_xe_sub_device_count(xe_device_handle_t device) {
  uint32_t count = 0;

  EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGetSubDevices(device, &count, nullptr));
  return count;
}

std::vector<xe_device_handle_t> get_xe_sub_devices(xe_device_handle_t device) {
  return get_xe_sub_devices(device, get_xe_sub_device_count(device));
}

std::vector<xe_device_handle_t> get_xe_sub_devices(xe_device_handle_t device,
                                                   uint32_t count) {
  std::vector<xe_device_handle_t> sub_devices(count);

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGetSubDevices(device, &count, sub_devices.data()));
  return sub_devices;
}

xe_api_version_t get_api_version(xe_device_group_handle_t device_group) {
  xe_api_version_t api_version;

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetApiVersion(device_group, &api_version));
  return api_version;
}

std::vector<xe_device_properties_t>
get_device_properties(xe_device_group_handle_t device_group) {
  std::vector<xe_device_properties_t> properties(
      lzt::get_xe_device_count(device_group));

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetDeviceProperties(device_group, properties.data()));
  return properties;
}

xe_device_compute_properties_t
get_compute_properties(xe_device_group_handle_t device_group) {
  xe_device_compute_properties_t properties = {
      XE_DEVICE_COMPUTE_PROPERTIES_VERSION_CURRENT};

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetComputeProperties(device_group, &properties));
  return properties;
}

uint32_t get_memory_properties_count(xe_device_group_handle_t device_group) {
  uint32_t count = 0;

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetMemoryProperties(device_group, &count, nullptr));
  return count;
}

std::vector<xe_device_memory_properties_t>
get_memory_properties(xe_device_group_handle_t device_group) {
  auto count = get_memory_properties_count(device_group);
  std::vector<xe_device_memory_properties_t> properties(
      count, {XE_DEVICE_MEMORY_PROPERTIES_VERSION_CURRENT});

  EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupGetMemoryProperties(
                                   device_group, &count, properties.data()));
  return properties;
}

std::vector<xe_device_memory_properties_t>
get_memory_properties(xe_device_group_handle_t device_group, uint32_t count) {
  std::vector<xe_device_memory_properties_t> properties(
      count, {XE_DEVICE_MEMORY_PROPERTIES_VERSION_CURRENT});

  EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupGetMemoryProperties(
                                   device_group, &count, properties.data()));
  return properties;
}

xe_device_memory_access_properties_t
get_memory_access_properties(xe_device_group_handle_t device_group) {
  xe_device_memory_access_properties_t properties = {
      XE_DEVICE_MEMORY_ACCESS_PROPERTIES_VERSION_CURRENT};

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetMemoryAccessProperties(device_group, &properties));
  return properties;
}

xe_device_cache_properties_t
get_cache_properties(xe_device_group_handle_t device_group) {
  xe_device_cache_properties_t properties = {
      XE_DEVICE_CACHE_PROPERTIES_VERSION_CURRENT};

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetCacheProperties(device_group, &properties));
  return properties;
}

xe_device_image_properties_t
get_image_properties(xe_device_group_handle_t device_group) {
  xe_device_image_properties_t properties = {
      XE_DEVICE_IMAGE_PROPERTIES_VERSION_CURRENT};

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetImageProperties(device_group, &properties));
  return properties;
}

#if 0
 TODO: enable when this API is available
xe_device_ipc_properties_t get_ipc_properties (xe_device_group_handle_t device_group)
{
  xe_device_ipc_properties_t properties =
     {XE_DEVICE_IPC_PROPERTIES_VERSION_CURRENT};

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupGetIPCProperties(device_group, &properties));

  return properties;
}
#endif

xe_device_p2p_properties_t get_p2p_properties(xe_device_handle_t dev1,
                                              xe_device_handle_t dev2) {
  xe_device_p2p_properties_t properties = {
      XE_DEVICE_P2P_PROPERTIES_VERSION_CURRENT};

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGetP2PProperties(dev1, dev2, &properties));
  return properties;
}

xe_bool_t can_access_peer(xe_device_handle_t dev1, xe_device_handle_t dev2) {
  xe_bool_t can_access;

  EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceCanAccessPeer(dev1, dev2, &can_access));
  return can_access;
}

void set_intermediate_cache_config(xe_device_handle_t device,
                                   xe_cache_config_t config) {
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceSetIntermediateCacheConfig(device, config));
}

void set_last_level_cache_config(xe_device_handle_t device,
                                 xe_cache_config_t config) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceSetLastLevelCacheConfig(device, config));
}

}; // namespace level_zero_tests
