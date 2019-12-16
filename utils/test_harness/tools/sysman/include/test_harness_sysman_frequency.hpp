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

#ifndef level_zero_tests_ZE_TEST_HARNESS_SYSMAN_FREQUENCY_HPP
#define level_zero_tests_ZE_TEST_HARNESS_SYSMAN_FREQUENCY_HPP

#include "ze_api.h"
#include "gtest/gtest.h"
#include "test_harness_sysman_init.hpp"

#define IDLE_WAIT_TIMESTEP_MSEC 250
#define IDLE_WAIT_TIMEOUT_MSEC 2500
namespace level_zero_tests {

void idle_check(zet_sysman_freq_handle_t pFreqHandle);
void validate_freq_state(zet_sysman_freq_handle_t pFreqHandle,
                         zet_freq_state_t pState);
uint32_t get_freq_handle_count(ze_device_handle_t device, uint32_t pCount = 0);
std::vector<zet_sysman_freq_handle_t>
get_freq_handles(ze_device_handle_t device);
zet_freq_state_t get_freq_state(zet_sysman_freq_handle_t pFreqHandle);
zet_freq_range_t get_freq_range(zet_sysman_freq_handle_t pFreqHandle);
void set_freq_range(zet_sysman_freq_handle_t pFreqHandle,
                    zet_freq_range_t &pLimits);
zet_freq_properties_t get_freq_properties(zet_sysman_freq_handle_t pFreqHandle);
zet_freq_range_t
get_and_validate_freq_range(zet_sysman_freq_handle_t pFreqHandle);
uint32_t get_available_clock_count(zet_sysman_freq_handle_t pFreqHandle);
std::vector<double> get_available_clocks(zet_sysman_freq_handle_t pFreqHandle,
                                         uint32_t &count);

} // namespace level_zero_tests

#endif
