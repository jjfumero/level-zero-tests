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

#include "xe_utils/xe_utils.hpp"
#include "xe_test_harness/xe_test_harness.hpp"
#include "logging/logging.hpp"

#include <chrono>
#include <thread>

namespace cs = compute_samples;

#include "xe_driver.h"

namespace {

TEST(xeDeviceGroupGetTests,
     GivenZeroCountWhenRetrievingDeviceGroupsThenValidCountReturned) {
  cs::get_xe_device_group_count();
}

TEST(
    xeDeviceGroupGetTests,
    GivenValidDeviceGroupPointerWhenRetrievingDeviceGroupsThenNotNullDeviceGroupsIsReturned) {
  uint32_t count = cs::get_xe_device_group_count();
  auto device_groups = cs::get_xe_device_groups(count);

  for (auto device_group : device_groups) {
    EXPECT_NE(nullptr, device_group);
  }
}

TEST(xeDeviceGetTests,
     GivenZeroCountWhenRetrievingDeviceThenValidCountReturned) {

  auto device_groups = cs::get_xe_device_groups();

  ASSERT_GT(device_groups.size(), 0);
  for (auto device_group : device_groups) {
    EXPECT_GT(cs::get_xe_device_count(device_group), 0)
        << "no devices in device group";
  }
}

TEST(xeDeviceGetTests,
     GivenValidCountWhenRetrievingDeviceThenNotNullDeviceIsReturned) {

  auto device_groups = cs::get_xe_device_groups();

  ASSERT_GT(device_groups.size(), 0);
  for (auto device_group : device_groups) {
    auto devices = cs::get_xe_devices(device_group);
    for (auto device : devices) {
      EXPECT_NE(nullptr, device);
    }
  }
}

TEST(xeDeviceGetSubDevicesTest,
     GivenZeroCountWhenRetrievingSubDevicesThenValidCountIsReturned) {
  cs::get_xe_sub_device_count(cs::xeDevice::get_instance()->get_device());
}

TEST(xeDeviceGetSubDevicesTest,
     GivenValidCountWhenRetrievingSubDevicesThenNotNullSubDeviceReturned) {

  std::vector<xe_device_handle_t> sub_devices =
      cs::get_xe_sub_devices(cs::xeDevice::get_instance()->get_device());

  for (auto sub_device : sub_devices) {
    EXPECT_NE(nullptr, sub_device);
  }
}

TEST(xeDeviceGroupGetApiVersionTests,
     GivenValidDeviceWhenRetrievingApiVersionThenValidApiVersionIsReturned) {
  auto device_groups = cs::get_xe_device_groups();
  ASSERT_GT(device_groups.size(), 0);
  for (auto device_group : device_groups) {
    cs::get_api_version(device_group);
  }
}

TEST(xeDeviceGroupGetDevicePropertiesTests,
     GivenValidDeviceWhenRetrievingPropertiesThenValidPropertiesAreReturned) {

  auto device_groups = cs::get_xe_device_groups();
  for (auto device_group : device_groups) {
    auto properties = cs::get_device_properties(device_group);
    EXPECT_EQ(XE_DEVICE_TYPE_GPU, properties[0].type);
  }
}

TEST(
    xeDeviceGroupGetComputePropertiesTests,
    GivenValidDeviceWhenRetrievingComputePropertiesThenValidPropertiesAreReturned) {

  auto device_groups = cs::get_xe_device_groups();
  ASSERT_GT(device_groups.size(), 0);
  for (auto device_group : device_groups) {
    xe_device_compute_properties_t properties =
        cs::get_compute_properties(device_group);

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
    xeDeviceGroupGetMemoryPropertiesTests,
    GivenValidCountPointerWhenRetrievingMemoryPropertiesThenValidCountReturned) {
  // FIXME: VLCLJ-354 - The level zero spec indicates the last argument to this
  // API is optional but this test fails with an
  // XE_RESULT_ERROR_INVALID_PARAMETER
  auto device_groups = cs::get_xe_device_groups();
  ASSERT_GT(device_groups.size(), 0);
  for (auto device_group : device_groups) {
    cs::get_memory_properties_count(device_group);
  }
}

TEST(
    xeDeviceGroupGetMemoryPropertiesTests,
    GivenValidDeviceWhenRetrievingMemoryPropertiesThenValidPropertiesAreReturned) {

  auto device_groups = cs::get_xe_device_groups();
  ASSERT_GT(device_groups.size(), 0);

  for (auto device_group : device_groups) {
    uint32_t count = cs::get_memory_properties_count(device_group);
    uint32_t count_out = count;

    ASSERT_GT(count, 0) << "no memory properties found";
    std::vector<xe_device_memory_properties_t> properties =
        cs::get_memory_properties(device_group);

    for (uint32_t i = 0; i < count_out; ++i) {
      EXPECT_EQ(count_out, count);
      EXPECT_GT(properties[i].maxClockRate, 0);
      EXPECT_GT(properties[i].maxBusWidth, 0);
      EXPECT_GT(properties[i].totalSize, 0);
    }
  }
}

TEST(
    xeDeviceGetMemoryAccessTests,
    GivenValidDeviceGroupWhenRetrievingMemoryAccessPropertiesThenValidPropertiesReturned) {
  auto device_groups = cs::get_xe_device_groups();

  ASSERT_GT(device_groups.size(), 0);
  for (auto device_group : device_groups) {
    cs::get_memory_access_properties(device_group);
  }
}

TEST(
    xeDeviceGroupGetCachePropertiesTests,
    GivenValidDeviceGroupWhenRetrievingCachePropertiesThenValidPropertiesAreReturned) {
  auto device_groups = cs::get_xe_device_groups();

  ASSERT_GT(device_groups.size(), 0);
  for (auto device_group : device_groups) {
    xe_device_cache_properties_t properties =
        cs::get_cache_properties(device_group);
  }
}

// This API is currently not implemented so these tests fail
TEST(
    xeDeviceGroupGetImagePropertiesTests,
    GivenValidDeviceGroupWhenRetrievingImagePropertiesThenValidPropertiesAreReturned) {
  auto device_groups = cs::get_xe_device_groups();

  ASSERT_GT(device_groups.size(), 0);
  for (auto device_group : device_groups) {
    xe_device_image_properties_t properties =
        cs::get_image_properties(device_group);
    EXPECT_TRUE(properties.supported);
    EXPECT_GT(properties.maxImageDims1D, 0);
    EXPECT_GT(properties.maxImageDims2D, 0);
    EXPECT_GT(properties.maxImageDims3D, 0);
    EXPECT_GT(properties.maxImageArraySlices, 0);
  }
}

TEST(
    xeDeviceGroupGetIPCPropertiesTests,
    GivenValidDeviceGroupWhenRetrievingIPCPropertiesThenValidPropertiesAreRetured) {

  FAIL() << "xeDeviceGroupGetIPCProperties API unavailable";
#if 0
 TODO: enable when this API is available
  auto device_groups = cs::get_xe_device_groups();
  ASSERT_GT(device_groups.size(), 0);
  for (auto device_group : device_groups){
    cs::get_ipc_properties(device_group);
  }
#endif
}

TEST(xeDevicGetP2PPropertiesTests,
     GivenValidDevicesWhenRetrievingP2PThenValidPropertiesAreReturned) {
  auto device_groups = cs::get_all_device_groups();

  ASSERT_GT(device_groups.size(), 0)
      << "no device groups found for peer to peer device test";

  std::vector<xe_device_handle_t> devices;
  for (auto device_group : device_groups) {
    devices = cs::get_xe_devices(device_group);

    if (devices.size() >= 2)
      break;
  }
  ASSERT_GE(devices.size(), 2)
      << "less than 2 devices available for peer to peer device test";

  cs::get_p2p_properties(devices[0], devices[1]);
}

TEST(xeDeviceCanAccessPeerTests,
     GivenValidDevicesWhenRetrievingCanAccessPropertyThenCapabilityIsReturned) {
  auto device_groups = cs::get_xe_device_groups();
  ASSERT_GT(device_groups.size(), 0)
      << "no device groups found for peer to peer device test";

  std::vector<xe_device_handle_t> devices;
  for (auto device_group : device_groups) {
    devices = cs::get_xe_devices(device_group);

    if (devices.size() >= 1)
      break;
  }

  ASSERT_GE(devices.size(), 2)
      << "less than 2 devices available for peer to peer device test";

  xe_bool_t a2b, b2a;
  a2b = cs::can_access_peer(devices[0], devices[1]);
  b2a = cs::can_access_peer(devices[1], devices[0]);

  EXPECT_EQ(a2b, b2a);
}

// This feature is not currently implemented so these tests fail
class xeSetCacheConfigTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<xe_cache_config_t> {};

TEST_P(xeSetCacheConfigTests,
       GivenConfigFlagWhenSettingIntermediateCacheConfigThenSuccessIsReturned) {
  cs::set_last_level_cache_config(cs::xeDevice::get_instance()->get_device(),
                                  GetParam());
}

INSTANTIATE_TEST_CASE_P(SetIntermediateCacheConfigParemeterizedTest,
                        xeSetCacheConfigTests,
                        ::testing::Values(XE_CACHE_CONFIG_DEFAULT,
                                          XE_CACHE_CONFIG_LARGE_SLM,
                                          XE_CACHE_CONFIG_LARGE_DATA));

TEST_P(xeSetCacheConfigTests,
       GivenConfigFlagWhenSettingLastLevelCacheConfigThenSuccessIsReturned) {
  cs::set_intermediate_cache_config(cs::xeDevice::get_instance()->get_device(),
                                    GetParam());
}

INSTANTIATE_TEST_CASE_P(SetLastLevelCacheConfigParemeterizedTest,
                        xeSetCacheConfigTests,
                        ::testing::Values(XE_CACHE_CONFIG_DEFAULT,
                                          XE_CACHE_CONFIG_LARGE_SLM,
                                          XE_CACHE_CONFIG_LARGE_DATA));

} // namespace

// TODO: Verify properties in similar way as computeinfo_extended
// TODO: Test all available devices
// TODO: Test P2P
// TODO: Test cache configs
