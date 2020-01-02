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

#include "gtest/gtest.h"

#include "logging/logging.hpp"
#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"

namespace lzt = level_zero_tests;

#include "ze_api.h"
#include "zet_api.h"

namespace {

TEST(zetSysmanPciGetStateTests,
     GivenSysmanHandleWhenRetrievingStateThenStateIsReturned) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    zet_pci_properties_t PciProps;
    PciProps = lzt::get_pci_properties(device);
    zet_pci_state_t PciState;
    PciState = lzt::get_pci_state(device);
    EXPECT_LE(PciState.speed.gen, PciProps.maxSpeed.gen);
    EXPECT_LE(PciState.speed.width, PciProps.maxSpeed.width);
    EXPECT_LE(PciState.speed.maxBandwidth, PciProps.maxSpeed.maxBandwidth);
    EXPECT_TRUE(
        (PciState.speed.maxPacketSize == PCI_SPEED_MAX_PACKET_SIZE_128) ||
        (PciState.speed.maxPacketSize == PCI_SPEED_MAX_PACKET_SIZE_256));
  };
}

TEST(zetSysmanPciGetPropertiesTests,
     GivenSysmanHandleWhenRetrievingPCIPropertiesThenpropertiesIsReturned) {
  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    zet_pci_properties_t PciProps;
    PciProps = lzt::get_pci_properties(device);
    EXPECT_GT(PciProps.address.domain, 0);
    EXPECT_LT(PciProps.address.domain, MAX_DOMAINs);
    EXPECT_GE(PciProps.address.bus, 0);
    EXPECT_LT(PciProps.address.bus, MAX_BUSES_PER_DOMAIN);
    EXPECT_GE(PciProps.address.device, 0);
    EXPECT_LT(PciProps.address.device, MAX_DEVICES_PER_BUS);
    EXPECT_GE(PciProps.address.function, 0);
    EXPECT_LT(PciProps.address.function, MAX_FUNCTIONS_PER_DEVICE);
    EXPECT_GE(PciProps.maxSpeed.gen, 0);
    EXPECT_LE(PciProps.maxSpeed.gen, PCI_SPEED_MAX_LINK_GEN);
    EXPECT_GT(PciProps.maxSpeed.width, 0);
    EXPECT_LE(PciProps.maxSpeed.width, PCI_SPEED_MAX_LANE_WIDTH);
    EXPECT_GT(PciProps.maxSpeed.width, 0);
    EXPECT_GT(PciProps.maxSpeed.maxBandwidth, 0);
    EXPECT_LE(PciProps.maxSpeed.maxBandwidth, PCI_SPEED_MAX_BANDWIDTH_GT_S);
  };
}

} // namespace
