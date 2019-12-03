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

#include "../include/common.h"

using namespace std;

void Timer::start() { tick = chrono::high_resolution_clock::now(); }

long double Timer::stopAndTime() {
  tock = chrono::high_resolution_clock::now();
  return std::chrono::duration<long double, std::chrono::microseconds::period>(
             tock - tick)
      .count();
}

uint64_t roundToMultipleOf(uint64_t number, uint64_t base, uint64_t maxValue) {
  uint64_t n = (number > maxValue) ? maxValue : number;
  return (n / base) * base;
}
