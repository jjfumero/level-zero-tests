/*
 * INTEL CONFIDENTIAL
 *
 * Copyright 2020 Intel Corporation All Rights Reserved.
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

#ifndef level_zero_tests_ZE_TEST_HARNESS_SYSMAN_POWER_HPP
#define level_zero_tests_ZE_TEST_HARNESS_SYSMAN_POWER_HPP
#include "ze_api.h"
#include "gtest/gtest.h"
#include "test_harness_sysman_init.hpp"

namespace level_zero_tests {
uint32_t get_power_handle_count(ze_device_handle_t device);
std::vector<zet_sysman_pwr_handle_t>
get_power_handles(ze_device_handle_t device, uint32_t &count);
zet_power_properties_t
get_power_properties(zet_sysman_pwr_handle_t pPowerhandle);
void get_power_limits(zet_sysman_pwr_handle_t pPowerHandle,
                      zet_power_sustained_limit_t *pSustained,
                      zet_power_burst_limit_t *pBurst,
                      zet_power_peak_limit_t *pPeak);
void set_power_limits(zet_sysman_pwr_handle_t pPowerHandle,
                      zet_power_sustained_limit_t *pSustained,
                      zet_power_burst_limit_t *pBurst,
                      zet_power_peak_limit_t *pPeak);
void get_power_energy_counter(zet_sysman_pwr_handle_t pPowerHandle,
                              zet_power_energy_counter_t *pEnergy);
zet_energy_threshold_t
get_power_energy_threshold(zet_sysman_pwr_handle_t pPowerHandle);
void set_power_energy_threshold(zet_sysman_pwr_handle_t pPowerHandle,
                                double threshold);
} // namespace level_zero_tests

#endif
