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

#ifndef level_zero_tests_ZE_TEST_HARNESS_DEVICE_HPP
#define level_zero_tests_ZE_TEST_HARNESS_DEVICE_HPP

#include "ze_api.h"
#include "gtest/gtest.h"

namespace level_zero_tests {

class zeDevice {
public:
  static zeDevice *get_instance();
  ze_device_handle_t get_device();
  ze_driver_handle_t get_driver();

private:
  zeDevice();
  static zeDevice *instance_;
  ze_device_handle_t device_ = nullptr;
  ze_driver_handle_t driver_ = nullptr;
};

uint32_t get_ze_device_count();
uint32_t get_ze_device_count(ze_driver_handle_t driver);
std::vector<ze_device_handle_t> get_ze_devices();
std::vector<ze_device_handle_t> get_ze_devices(uint32_t count);
std::vector<ze_device_handle_t> get_ze_devices(ze_driver_handle_t driver);
std::vector<ze_device_handle_t> get_ze_devices(uint32_t count,
                                               ze_driver_handle_t driver);
uint32_t get_ze_sub_device_count(ze_device_handle_t device);
std::vector<ze_device_handle_t> get_ze_sub_devices(ze_device_handle_t device);
std::vector<ze_device_handle_t> get_ze_sub_devices(ze_device_handle_t device,
                                                   uint32_t count);

ze_device_properties_t get_device_properties(ze_device_handle_t device);
ze_device_compute_properties_t
get_compute_properties(ze_device_handle_t device);
uint32_t get_memory_properties_count(ze_device_handle_t device);
std::vector<ze_device_memory_properties_t>
get_memory_properties(ze_device_handle_t device);
std::vector<ze_device_memory_properties_t>
get_memory_properties(ze_device_handle_t device, uint32_t count);
ze_device_memory_access_properties_t
get_memory_access_properties(ze_device_handle_t device);
ze_device_cache_properties_t get_cache_properties(ze_device_handle_t device);
ze_device_image_properties_t get_image_properties(ze_device_handle_t device);

ze_device_p2p_properties_t get_p2p_properties(ze_device_handle_t dev1,
                                              ze_device_handle_t dev2);
ze_bool_t can_access_peer(ze_device_handle_t dev1, ze_device_handle_t dev2);

void set_intermediate_cache_config(ze_kernel_handle_t kernel,
                                   ze_cache_config_t config);
void set_last_level_cache_config(ze_device_handle_t device,
                                 ze_cache_config_t config);

}; // namespace level_zero_tests
#endif
