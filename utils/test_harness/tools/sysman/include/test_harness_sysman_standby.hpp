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

#ifndef level_zero_tests_ZE_TEST_HARNESS_SYSMAN_STANDBY_HPP
#define level_zero_tests_ZE_TEST_HARNESS_SYSMAN_STANDBY_HPP
#include "ze_api.h"
#include "gtest/gtest.h"
#include "test_harness_sysman_init.hpp"

namespace level_zero_tests {
uint32_t get_standby_handle_count(ze_device_handle_t device,
                                  uint32_t pCount = 0);
std::vector<zet_sysman_standby_handle_t>
get_standby_handles(ze_device_handle_t device);
zet_standby_promo_mode_t get_standby_mode(zet_sysman_standby_handle_t pHnadle);
void set_standby_mode(zet_sysman_standby_handle_t pHnadle,
                      zet_standby_promo_mode_t pMode);
zet_standby_properties_t
get_standby_properties(zet_sysman_standby_handle_t pStandbyHandle);

} // namespace level_zero_tests

#endif
