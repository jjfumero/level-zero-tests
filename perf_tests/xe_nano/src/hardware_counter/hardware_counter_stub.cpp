/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2016 - 2019 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 */

#include "hardware_counter.hpp"

#include <assert.h>

HardwareCounter::HardwareCounter() {}

HardwareCounter::~HardwareCounter() {}

void HardwareCounter::start(void) { assert(0); }

void HardwareCounter::end(void) { assert(0); }

void HardwareCounter::counter_asserts(void) { assert(0); }

long long HardwareCounter::counter_instructions(void) {
  assert(0);
  return -1;
}

long long HardwareCounter::counter_cycles(void) {
  assert(0);
  return -1;
}

bool HardwareCounter::is_supported(void) { return false; }

std::string HardwareCounter::support_warning(void) {
  return "Hardware counters are not supported. Compile benchmark with the PAPI "
         "library on Unix system";
}
