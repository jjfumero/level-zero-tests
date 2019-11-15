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

#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"
#include "logging/logging.hpp"

#include <chrono>
#include <thread>

namespace lzt = level_zero_tests;

#include "ze_api.h"

namespace {

TEST(zeDeviceGetTests,
     GivenZeroCountWhenRetrievingDevicesThenValidCountReturned) {
  lzt::get_ze_device_count();
}

TEST(zeDeviceGetTests,
     GivenValidCountWhenRetrievingDevicesThenNotNullDevicesAreReturned) {

  auto device_count = lzt::get_ze_device_count();

  ASSERT_GT(device_count, 0);

  auto devices = lzt::get_ze_devices(device_count);
  for (auto device : devices) {
    EXPECT_NE(nullptr, device);
  }
}

TEST(zeDeviceGetSubDevicesTest,
     GivenZeroCountWhenRetrievingSubDevicesThenValidCountIsReturned) {
  lzt::get_ze_sub_device_count(lzt::zeDevice::get_instance()->get_device());
}

TEST(zeDeviceGetSubDevicesTest,
     GivenValidCountWhenRetrievingSubDevicesThenNotNullSubDeviceReturned) {

  std::vector<ze_device_handle_t> sub_devices =
      lzt::get_ze_sub_devices(lzt::zeDevice::get_instance()->get_device());

  for (auto sub_device : sub_devices) {
    EXPECT_NE(nullptr, sub_device);
  }
}

TEST(zeDeviceGetDevicePropertiesTests,
     GivenValidDeviceWhenRetrievingPropertiesThenValidPropertiesAreReturned) {

  auto devices = lzt::get_ze_devices();
  for (auto device : devices) {
    auto properties = lzt::get_device_properties(device);
    EXPECT_EQ(ZE_DEVICE_TYPE_GPU, properties.type);
  }
}

TEST(
    zeDeviceGetComputePropertiesTests,
    GivenValidDeviceWhenRetrievingComputePropertiesThenValidPropertiesAreReturned) {

  auto devices = lzt::get_ze_devices();
  ASSERT_GT(devices.size(), 0);
  for (auto device : devices) {
    ze_device_compute_properties_t properties =
        lzt::get_compute_properties(device);

    EXPECT_GT(properties.maxTotalGroupSize, 0);
    EXPECT_GT(properties.maxGroupSizeX, 0);
    EXPECT_GT(properties.maxGroupSizeY, 0);
    EXPECT_GT(properties.maxGroupSizeZ, 0);
    EXPECT_GT(properties.maxGroupCountX, 0);
    EXPECT_GT(properties.maxGroupCountY, 0);
    EXPECT_GT(properties.maxGroupCountZ, 0);
    EXPECT_GT(properties.maxSharedLocalMemory, 0);
    EXPECT_GT(properties.numSubGroupSizes, 0);
    for (uint32_t i = 0; i < properties.numSubGroupSizes; ++i) {
      EXPECT_NE(0, properties.subGroupSizes[i]);
    }
  }
}

TEST(
    zeDeviceGetMemoryPropertiesTests,
    GivenValidCountPointerWhenRetrievingMemoryPropertiesThenValidCountReturned) {
  // FIXME: VLCLJ-354 - The level zero spec indicates the last argument to this
  // API is optional but this test fails with an
  // ZE_RESULT_ERROR_INVALID_PARAMETER
  auto devices = lzt::get_ze_devices();
  ASSERT_GT(devices.size(), 0);
  for (auto device : devices) {
    lzt::get_memory_properties_count(device);
  }
}

TEST(
    zeDeviceGetMemoryPropertiesTests,
    GivenValidDeviceWhenRetrievingMemoryPropertiesThenValidPropertiesAreReturned) {

  auto devices = lzt::get_ze_devices();
  ASSERT_GT(devices.size(), 0);

  for (auto device : devices) {
    uint32_t count = lzt::get_memory_properties_count(device);
    uint32_t count_out = count;

    ASSERT_GT(count, 0) << "no memory properties found";
    std::vector<ze_device_memory_properties_t> properties =
        lzt::get_memory_properties(device);

    for (uint32_t i = 0; i < count_out; ++i) {
      EXPECT_EQ(count_out, count);
      EXPECT_GT(properties[i].maxClockRate, 0);
      EXPECT_GT(properties[i].maxBusWidth, 0);
      EXPECT_GT(properties[i].totalSize, 0);
    }
  }
}

TEST(
    zeDeviceGetMemoryAccessTests,
    GivenValidDeviceWhenRetrievingMemoryAccessPropertiesThenValidPropertiesReturned) {
  auto devices = lzt::get_ze_devices();

  ASSERT_GT(devices.size(), 0);
  for (auto device : devices) {
    lzt::get_memory_access_properties(device);
  }
}

TEST(
    zeDeviceGetCachePropertiesTests,
    GivenValidDeviceWhenRetrievingCachePropertiesThenValidPropertiesAreReturned) {
  auto devices = lzt::get_ze_devices();

  ASSERT_GT(devices.size(), 0);
  for (auto device : devices) {
    ze_device_cache_properties_t properties = lzt::get_cache_properties(device);
  }
}

// This API is currently not implemented so these tests fail
TEST(
    zeDeviceGetImagePropertiesTests,
    GivenValidDeviceWhenRetrievingImagePropertiesThenValidPropertiesAreReturned) {
  auto devices = lzt::get_ze_devices();

  ASSERT_GT(devices.size(), 0);
  for (auto device : devices) {
    ze_device_image_properties_t properties = lzt::get_image_properties(device);
    EXPECT_TRUE(properties.supported);
    EXPECT_GT(properties.maxImageDims1D, 0);
    EXPECT_GT(properties.maxImageDims2D, 0);
    EXPECT_GT(properties.maxImageDims3D, 0);
    EXPECT_GT(properties.maxImageArraySlices, 0);
  }
}

TEST(xeDevicGetP2PPropertiesTests,
     GivenValidDevicesWhenRetrievingP2PThenValidPropertiesAreReturned) {
  auto drivers = lzt::get_all_driver_handles();

  ASSERT_GT(drivers.size(), 0)
      << "no drivers found for peer to peer device test";

  std::vector<ze_device_handle_t> devices;
  for (auto driver : drivers) {
    devices = lzt::get_ze_devices(driver);

    if (devices.size() >= 2)
      break;
  }
  ASSERT_GE(devices.size(), 2)
      << "less than 2 devices available for peer to peer device test";

  lzt::get_p2p_properties(devices[0], devices[1]);
}

TEST(zeDeviceCanAccessPeerTests,
     GivenValidDevicesWhenRetrievingCanAccessPropertyThenCapabilityIsReturned) {
  auto drivers = lzt::get_all_driver_handles();
  ASSERT_GT(drivers.size(), 0)
      << "no drivers found for peer to peer device test";

  std::vector<ze_device_handle_t> devices;
  for (auto driver : drivers) {
    devices = lzt::get_ze_devices(driver);

    if (devices.size() >= 1)
      break;
  }

  ASSERT_GE(devices.size(), 2)
      << "less than 2 devices available for peer to peer device test";

  ze_bool_t a2b, b2a;
  a2b = lzt::can_access_peer(devices[0], devices[1]);
  b2a = lzt::can_access_peer(devices[1], devices[0]);

  EXPECT_EQ(a2b, b2a);
}

// This feature is not currently implemented so these tests fail
class xeSetCacheConfigTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<ze_cache_config_t> {};

TEST_P(xeSetCacheConfigTests,
       GivenConfigFlagWhenSettingIntermediateCacheConfigThenSuccessIsReturned) {
  lzt::set_last_level_cache_config(lzt::zeDevice::get_instance()->get_device(),
                                   GetParam());
}

// TODO: To move to kernel tests, VLCLJ-529
#if 0
INSTANTIATE_TEST_CASE_P(SetIntermediateCacheConfigParemeterizedTest,
                        xeSetCacheConfigTests,
                        ::testing::Values(ZE_CACHE_CONFIG_DEFAULT,
                                          ZE_CACHE_CONFIG_LARGE_SLM,
                                          ZE_CACHE_CONFIG_LARGE_DATA));

TEST_P(xeSetCacheConfigTests,
       GivenConfigFlagWhenSettingLastLevelCacheConfigThenSuccessIsReturned) {
  lzt::set_intermediate_cache_config(
      lzt::zeDevice::get_instance()->get_device(), GetParam());
}
#endif

INSTANTIATE_TEST_CASE_P(SetLastLevelCacheConfigParemeterizedTest,
                        xeSetCacheConfigTests,
                        ::testing::Values(ZE_CACHE_CONFIG_DEFAULT,
                                          ZE_CACHE_CONFIG_LARGE_SLM,
                                          ZE_CACHE_CONFIG_LARGE_DATA));

} // namespace

// TODO: Verify properties in similar way as computeinfo_extended
// TODO: Test all available devices
// TODO: Test P2P
// TODO: Test cache configs
