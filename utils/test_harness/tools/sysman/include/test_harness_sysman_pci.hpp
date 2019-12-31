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

#ifndef level_zero_tests_ZE_TEST_HARNESS_SYSMAN_PCI_HPP
#define level_zero_tests_ZE_TEST_HARNESS_SYSMAN_PCI_HPP

#include "ze_api.h"
#include "gtest/gtest.h"
#include "test_harness_sysman_init.hpp"

#define MAX_DOMAINs 65536
#define MAX_BUSES_PER_DOMAIN 256
#define MAX_DEVICES_PER_BUS 32
#define MAX_FUNCTIONS_PER_DEVICE 8

#define PCI_SPEED_MAX_LINK_GEN 5
#define PCI_SPEED_MAX_LANE_WIDTH 16
#define PCI_SPEED_MAX_BANDWIDTH_GT_S 32 // TODO: confirm for PCIe Gen 5,
#define PCI_SPEED_MAX_PACKET_SIZE_128 128
#define PCI_SPEED_MAX_PACKET_SIZE_256 256

namespace level_zero_tests {
zet_pci_properties_t get_pci_properties(ze_device_handle_t device);
}
#endif
