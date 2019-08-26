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

#ifndef level_zero_tests_XE_TEST_HARNESS_DEVICE_HPP
#define level_zero_tests_XE_TEST_HARNESS_DEVICE_HPP

#include "xe_device.h"
#include "gtest/gtest.h"

namespace level_zero_tests {

class xeDevice {
public:
  static xeDevice *get_instance();
  xe_device_handle_t get_device();

private:
  xeDevice();
  static xeDevice *instance_;
  xe_device_handle_t device_ = nullptr;
};

uint32_t get_xe_device_group_count();
std::vector<xe_device_group_handle_t> get_xe_device_groups();
std::vector<xe_device_group_handle_t> get_xe_device_groups(uint32_t count);
uint32_t get_xe_device_count(xe_device_group_handle_t device_group);
std::vector<xe_device_handle_t>
get_xe_devices(xe_device_group_handle_t device_group);
std::vector<xe_device_handle_t>
get_xe_devices(xe_device_group_handle_t device_group, uint32_t count);
uint32_t get_xe_sub_device_count(xe_device_handle_t device);
std::vector<xe_device_handle_t> get_xe_sub_devices(xe_device_handle_t device);
std::vector<xe_device_handle_t> get_xe_sub_devices(xe_device_handle_t device,
                                                   uint32_t count);
xe_api_version_t get_api_version(xe_device_group_handle_t device_group);
std::vector<xe_device_properties_t>
get_device_properties(xe_device_group_handle_t device_group);
xe_device_compute_properties_t
get_compute_properties(xe_device_group_handle_t device_group);
uint32_t get_memory_properties_count(xe_device_group_handle_t device_group);
std::vector<xe_device_memory_properties_t>
get_memory_properties(xe_device_group_handle_t device_group);
std::vector<xe_device_memory_properties_t>
get_memory_properties(xe_device_group_handle_t device_group, uint32_t count);
xe_device_memory_access_properties_t
get_memory_access_properties(xe_device_group_handle_t device_group);
xe_device_cache_properties_t
get_cache_properties(xe_device_group_handle_t device_group);
xe_device_image_properties_t
get_image_properties(xe_device_group_handle_t device_group);
#if 0
TODO: Enable when the xeDeviceGroupGetIPCProperties API is available
xe_device_ipc_properties_t get_ipc_properties (xe_device_group_handle_t device_group);
#endif

xe_device_p2p_properties_t get_p2p_properties(xe_device_handle_t dev1,
                                              xe_device_handle_t dev2);
xe_bool_t can_access_peer(xe_device_handle_t dev1, xe_device_handle_t dev2);

void set_intermediate_cache_config(xe_device_handle_t device,
                                   xe_cache_config_t config);
void set_last_level_cache_config(xe_device_handle_t device,
                                 xe_cache_config_t config);

}; // namespace level_zero_tests
#endif
