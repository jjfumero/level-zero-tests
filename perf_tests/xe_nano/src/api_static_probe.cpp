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

#include "api_static_probe.hpp"

HardwareCounter *hardware_counters = NULL;
static bool static_probe_init = false;

void api_static_probe_init() {
  assert(static_probe_init == false); /* Initialize it only once */
  static_probe_init = true;
  hardware_counters = new HardwareCounter;
}

void api_static_probe_cleanup() {
  /* api static probe needs to be initialized first*/
  assert(static_probe_init == true);
  static_probe_init = false;
  delete hardware_counters;
}

bool api_static_probe_is_init() { return static_probe_init; }
