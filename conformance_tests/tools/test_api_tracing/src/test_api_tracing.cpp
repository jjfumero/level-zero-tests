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
#include "test_harness/test_harness.hpp"
#include "ze_api.h"
#include "zet_api.h"

namespace lzt = level_zero_tests;

namespace {

TEST(
    TracingCreateTests,
    GivenValidDeviceAndTracerDescriptionWhenCreatingTracerThenTracerIsNotNull) {
  uint32_t user_data;

  zet_tracer_desc_t tracer_desc;
  tracer_desc.version = ZET_TRACER_DESC_VERSION_CURRENT;
  tracer_desc.pUserData = &user_data;

  zet_tracer_handle_t tracer_handle = lzt::create_tracer_handle(tracer_desc);
  EXPECT_NE(tracer_handle, nullptr);

  lzt::destroy_tracer_handle(tracer_handle);
}

class TracingCreateMultipleTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<uint32_t> {};

TEST_P(TracingCreateMultipleTests,
       GivenExistingTracersWhenCreatingNewTracerThenSuccesIsReturned) {

  std::vector<zet_tracer_handle_t> tracers(GetParam());
  std::vector<zet_tracer_desc_t> descs(GetParam());
  uint32_t *user_data = new uint32_t[GetParam()];

  for (uint32_t i = 0; i < GetParam(); ++i) {
    descs[i].version = ZET_TRACER_DESC_VERSION_CURRENT;
    descs[i].pUserData = &user_data[i];

    tracers[i] = lzt::create_tracer_handle(descs[i]);
    EXPECT_NE(tracers[i], nullptr);
  }

  for (auto tracer : tracers) {
    lzt::destroy_tracer_handle(tracer);
  }
  delete[] user_data;
}

INSTANTIATE_TEST_CASE_P(CreateMultipleTracerTest, TracingCreateMultipleTests,
                        ::testing::Values(1, 10, 100, 1000));

TEST(TracingDestroyTests,
     GivenSingleDisabledTracerWhenDestroyingTracerThenSuccessIsReturned) {
  uint32_t user_data;

  zet_tracer_desc_t tracer_desc;
  tracer_desc.version = ZET_TRACER_DESC_VERSION_CURRENT;
  tracer_desc.pUserData = &user_data;
  zet_tracer_handle_t tracer_handle = lzt::create_tracer_handle(tracer_desc);

  lzt::destroy_tracer_handle(tracer_handle);
}

class TracingPrologueEpilogueTests : public ::testing::Test {
protected:
  void SetUp() override {
    driver = lzt::get_default_driver();
    device = lzt::zeDevice::get_instance()->get_device();

    compute_properties.version = ZE_DEVICE_COMPUTE_PROPERTIES_VERSION_CURRENT;
    memory_properties.version = ZE_DEVICE_MEMORY_PROPERTIES_VERSION_CURRENT;
    memory_access_properties.version =
        ZE_DEVICE_MEMORY_ACCESS_PROPERTIES_VERSION_CURRENT;
    cache_properties.version = ZE_DEVICE_CACHE_PROPERTIES_VERSION_CURRENT;
    device_image_properties.version =
        ZE_DEVICE_IMAGE_PROPERTIES_VERSION_CURRENT;
    p2p_properties.version = ZE_DEVICE_P2P_PROPERTIES_VERSION_CURRENT;

    command_queue_desc = {
        ZE_COMMAND_QUEUE_DESC_VERSION_CURRENT, // version
        ZE_COMMAND_QUEUE_FLAG_NONE,            // flags
        ZE_COMMAND_QUEUE_MODE_DEFAULT,         // mode
        ZE_COMMAND_QUEUE_PRIORITY_NORMAL       // priority
    };

    memory = lzt::allocate_device_memory(100);

    ze_image_format_desc_t format_descriptor = {
        ZE_IMAGE_FORMAT_LAYOUT_8,  // layout
        ZE_IMAGE_FORMAT_TYPE_UINT, // type
        ZE_IMAGE_FORMAT_SWIZZLE_X, // x
        ZE_IMAGE_FORMAT_SWIZZLE_X, // y
        ZE_IMAGE_FORMAT_SWIZZLE_X, // z
        ZE_IMAGE_FORMAT_SWIZZLE_X  // w
    };
    image_desc = {ZE_IMAGE_DESC_VERSION_CURRENT, // version
                  ZE_IMAGE_FLAG_PROGRAM_WRITE,   // flags
                  ZE_IMAGE_TYPE_1D,              // type
                  format_descriptor,             // format
                  1920,                          // width
                  1,                             // height
                  1,                             // depth
                  0,                             // arraylevels
                  0};                            // miplevels
    ASSERT_EQ(ZE_RESULT_SUCCESS, zeImageCreate(device, &image_desc, &image));

    ASSERT_EQ(
        ZE_RESULT_SUCCESS,
        zeCommandQueueCreate(device, &command_queue_desc, &command_queue));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListCreate(device, &command_list_desc, &command_list));

    ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventPoolCreate(driver, &event_pool_desc, 1,
                                                   &device, &event_pool));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeEventCreate(event_pool, &event_desc, &event));

    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeFenceCreate(command_queue, &fence_desc, &fence));

    binary_file = level_zero_tests::load_binary_file("module_add.spv");
    module_desc = {ZE_MODULE_DESC_VERSION_CURRENT,
                   ZE_MODULE_FORMAT_IL_SPIRV,
                   static_cast<uint32_t>(binary_file.size()),
                   binary_file.data(),
                   "",
                   nullptr};
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeModuleCreate(device, &module_desc, &module, &build_log));

    kernel_desc = {ZE_KERNEL_DESC_VERSION_CURRENT, ZE_KERNEL_FLAG_NONE,
                   "module_add_constant"};
    ASSERT_EQ(ZE_RESULT_SUCCESS, zeKernelCreate(module, &kernel_desc, &kernel));

    zet_tracer_desc_t tracer_desc;
    tracer_desc.version = ZET_TRACER_DESC_VERSION_CURRENT;
    tracer_desc.pUserData = &user_data;
    tracer_handle = lzt::create_tracer_handle(tracer_desc);
  }

  void TearDown() override {
    EXPECT_TRUE(user_data.prologue_called);
    EXPECT_TRUE(user_data.epilogue_called);

    if (fence)
      zeFenceDestroy(fence);
    if (event)
      zeEventDestroy(event);
    if (event_pool)
      zeEventPoolDestroy(event_pool);
    if (command_list)
      zeCommandListDestroy(command_list);
    if (command_queue)
      zeCommandQueueDestroy(command_queue);
    if (kernel)
      zeKernelDestroy(kernel);
    if (build_log)
      zeModuleBuildLogDestroy(build_log);
    if (module)
      zeModuleDestroy(module);
    if (image)
      zeImageDestroy(image);
    lzt::free_memory(memory);
    lzt::disable_tracer(tracer_handle);
    lzt::destroy_tracer_handle(tracer_handle);
  }

  zet_tracer_handle_t tracer_handle;
  lzt::test_api_tracing_user_data user_data = {};
  ze_callbacks_t prologues = {};
  ze_callbacks_t epilogues = {};

  ze_driver_handle_t driver;

  ze_device_handle_t device;
  ze_device_properties_t properties;
  ze_device_compute_properties_t compute_properties;

  ze_device_kernel_properties_t device_kernel_properties;
  ze_device_memory_properties_t memory_properties;

  ze_device_memory_access_properties_t memory_access_properties;

  ze_device_cache_properties_t cache_properties;
  ze_device_image_properties_t device_image_properties;
  ze_device_p2p_properties_t p2p_properties;

  ze_api_version_t api_version;
  ze_driver_ipc_properties_t ipc_properties;

  ze_command_queue_desc_t command_queue_desc;
  ze_command_queue_handle_t command_queue;

  ze_command_list_desc_t command_list_desc = {
      ZE_COMMAND_LIST_DESC_VERSION_CURRENT, ZE_COMMAND_LIST_FLAG_NONE};
  ze_command_list_handle_t command_list;

  ze_copy_region_t copy_region;
  ze_image_region_t image_region;

  ze_event_pool_handle_t event_pool;
  ze_event_pool_desc_t event_pool_desc = {ZE_EVENT_POOL_DESC_VERSION_CURRENT,
                                          ZE_EVENT_POOL_FLAG_HOST_VISIBLE, 1};
  ze_event_handle_t event;
  ze_event_desc_t event_desc = {ZE_EVENT_DESC_VERSION_CURRENT, 0,
                                ZE_EVENT_SCOPE_FLAG_NONE,
                                ZE_EVENT_SCOPE_FLAG_NONE};
  ze_ipc_event_pool_handle_t ipc_event;
  ze_ipc_mem_handle_t ipc_handle;

  ze_fence_handle_t fence;
  ze_fence_desc_t fence_desc = {ZE_FENCE_DESC_VERSION_CURRENT,
                                ZE_FENCE_FLAG_NONE};
  ze_image_desc_t image_desc = {};
  ze_image_handle_t image;
  ze_image_properties_t image_properties;

  void *device_memory, *host_memory, *shared_memory, *memory;

  ze_memory_allocation_properties_t mem_alloc_properties;

  std::vector<uint8_t> binary_file;
  ze_module_desc_t module_desc;
  ze_module_handle_t module;
  ze_module_build_log_handle_t build_log;

  ze_kernel_desc_t kernel_desc;
  ze_kernel_handle_t kernel;

  ze_kernel_attribute_t set_attribute;

  ze_cache_config_t cache_config;

  ze_sampler_desc_t sampler_desc = {ZE_SAMPLER_DESC_VERSION_CURRENT,
                                    ZE_SAMPLER_ADDRESS_MODE_NONE,
                                    ZE_SAMPLER_FILTER_MODE_NEAREST, true};
  ze_sampler_handle_t sampler;

  uint32_t num = 0, version;
  ze_driver_properties_t driver_properties;
  ze_bool_t can_access;
};

static void ready_tracer(zet_tracer_handle_t tracer, ze_callbacks_t prologues,
                         ze_callbacks_t epilogues) {
  lzt::set_tracer_prologues(tracer, prologues);
  lzt::set_tracer_epilogues(tracer, epilogues);
  lzt::enable_tracer(tracer);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeInitCallbacksWhenCallingzeInitThenUserDataIsSet) {
  prologues.Global.pfnInitCb = lzt::prologue_callback;
  epilogues.Global.pfnInitCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeInit(ZE_INIT_FLAG_NONE);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceGetCallbacksWhenCallingzeDeviceGetThenUserDataIsSet) {
  prologues.Device.pfnGetCb = lzt::prologue_callback;
  epilogues.Device.pfnGetCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceGet(driver, &num, nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithZeDeviceGetSubDevicesCallbacksWhenCallingzeDeviceGetSubDevicesThenUserDataIsSet) {
  prologues.Device.pfnGetSubDevicesCb = lzt::prologue_callback;
  epilogues.Device.pfnGetSubDevicesCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceGetSubDevices(device, &num, nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceGetPropertiesCallbacksWhenCallingzeDeviceGetPropertiesThenUserDataIsSet) {
  prologues.Device.pfnGetPropertiesCb = lzt::prologue_callback;
  epilogues.Device.pfnGetPropertiesCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceGetProperties(device, &properties);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceGetComputePropertiesCallbacksWhenCallingzeDeviceGetComputePropertiesThenUserDataIsSet) {
  prologues.Device.pfnGetComputePropertiesCb = lzt::prologue_callback;
  epilogues.Device.pfnGetComputePropertiesCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceGetComputeProperties(device, &compute_properties);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceGetMemoryPropertiesCallbacksWhenCallingzeDeviceGetMemoryPropertiesThenUserDataIsSet) {
  prologues.Device.pfnGetMemoryPropertiesCb = lzt::prologue_callback;
  epilogues.Device.pfnGetMemoryPropertiesCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceGetMemoryProperties(device, &num, nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceGetKernelPropertiesCallbacksWhenCallingzeDeviceGetKernelPropertiesThenUserDataIsSet) {
  prologues.Device.pfnGetKernelPropertiesCb = lzt::prologue_callback;
  epilogues.Device.pfnGetKernelPropertiesCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceGetKernelProperties(device, &device_kernel_properties);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceGetMemoryAccessPropertiesCallbacksWhenCallingzeDeviceGetMemoryAccessPropertiesThenUserDataIsSet) {
  prologues.Device.pfnGetMemoryAccessPropertiesCb = lzt::prologue_callback;
  epilogues.Device.pfnGetMemoryAccessPropertiesCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceGetMemoryAccessProperties(device, &memory_access_properties);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceGetCachePropertiesCallbacksWhenCallingzeDeviceGetCachePropertiesThenUserDataIsSet) {
  prologues.Device.pfnGetCachePropertiesCb = lzt::prologue_callback;
  epilogues.Device.pfnGetCachePropertiesCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceGetCacheProperties(device, &cache_properties);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceGetImagePropertiesCallbacksWhenCallingzeDeviceGetImagePropertiesThenUserDataIsSet) {
  prologues.Device.pfnGetImagePropertiesCb = lzt::prologue_callback;
  epilogues.Device.pfnGetImagePropertiesCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceGetImageProperties(device, &device_image_properties);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceGetP2PPropertiesCallbacksWhenCallingzeDeviceGetP2PPropertiesThenUserDataIsSet) {
  prologues.Device.pfnGetP2PPropertiesCb = lzt::prologue_callback;
  epilogues.Device.pfnGetP2PPropertiesCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceGetP2PProperties(device, device, &p2p_properties);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceCanAccessPeerCallbacksWhenCallingzeDeviceCanAccessPeerThenUserDataIsSet) {
  prologues.Device.pfnCanAccessPeerCb = lzt::prologue_callback;
  epilogues.Device.pfnCanAccessPeerCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceCanAccessPeer(device, device, &can_access);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceSetLastLevelCacheConfigCallbacksWhenCallingzeDeviceSetLastLevelCacheConfigThenUserDataIsSet) {
  prologues.Device.pfnSetLastLevelCacheConfigCb = lzt::prologue_callback;
  epilogues.Device.pfnSetLastLevelCacheConfigCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceSetLastLevelCacheConfig(device, ZE_CACHE_CONFIG_DEFAULT);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceSystemBarrierCallbacksWhenCallingzeDeviceSystemBarrierThenUserDataIsSet) {
  prologues.Device.pfnSystemBarrierCb = lzt::prologue_callback;
  epilogues.Device.pfnSystemBarrierCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceSystemBarrier(device);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceMakeMemoryResidentCallbacksWhenCallingzeDeviceMakeMemoryResidentThenUserDataIsSet) {
  prologues.Device.pfnMakeMemoryResidentCb = lzt::prologue_callback;
  epilogues.Device.pfnMakeMemoryResidentCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceMakeMemoryResident(device, memory, 0);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceEvictMemoryCallbacksWhenCallingzeDeviceEvictMemoryThenUserDataIsSet) {
  prologues.Device.pfnEvictMemoryCb = lzt::prologue_callback;
  epilogues.Device.pfnEvictMemoryCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceEvictMemory(device, memory, 0);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceMakeImageResidentCallbacksWhenCallingzeDeviceMakeImageResidentThenUserDataIsSet) {
  prologues.Device.pfnMakeImageResidentCb = lzt::prologue_callback;
  epilogues.Device.pfnMakeImageResidentCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceMakeImageResident(device, image);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDeviceEvictImageCallbacksWhenCallingzeDeviceEvictImageThenUserDataIsSet) {
  prologues.Device.pfnEvictImageCb = lzt::prologue_callback;
  epilogues.Device.pfnEvictImageCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDeviceEvictImage(device, image);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDriverGetCallbacksWhenCallingzeDriverGetThenUserDataIsSet) {
  prologues.Driver.pfnGetCb = lzt::prologue_callback;
  epilogues.Driver.pfnGetCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDriverGet(&num, nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDriverGetPropertiesCallbacksWhenCallingzeDriverGetPropertiesThenUserDataIsSet) {
  prologues.Driver.pfnGetPropertiesCb = lzt::prologue_callback;
  epilogues.Driver.pfnGetPropertiesCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDriverGetProperties(driver, &driver_properties);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDriverGetApiVersionCallbacksWhenCallingzeDriverGetApiVersionThenUserDataIsSet) {
  prologues.Driver.pfnGetApiVersionCb = lzt::prologue_callback;
  epilogues.Driver.pfnGetApiVersionCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDriverGetApiVersion(driver, &api_version);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDriverGetIPCPropertiesCallbacksWhenCallingzeDriverGetIPCPropertiesThenUserDataIsSet) {
  prologues.Driver.pfnGetIPCPropertiesCb = lzt::prologue_callback;
  epilogues.Driver.pfnGetIPCPropertiesCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDriverGetIPCProperties(driver, &ipc_properties);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDriverAllocSharedMemCallbacksWhenCallingzeDriverAllocSharedMemThenUserDataIsSet) {
  prologues.Driver.pfnAllocSharedMemCb = lzt::prologue_callback;
  epilogues.Driver.pfnAllocSharedMemCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ze_device_mem_alloc_desc_t device_desc;
  device_desc.flags = ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT;
  device_desc.ordinal = 0;
  ze_host_mem_alloc_desc_t host_desc;
  host_desc.flags = ZE_HOST_MEM_ALLOC_FLAG_DEFAULT;
  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeDriverAllocSharedMem(driver, &device_desc, &host_desc, 0, 0,
                                   device, &shared_memory));

  zeDriverFreeMem(driver, shared_memory);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDriverAllocDeviceMemCallbacksWhenCallingzeDriverAllocDeviceMemThenUserDataIsSet) {
  prologues.Driver.pfnAllocDeviceMemCb = lzt::prologue_callback;
  epilogues.Driver.pfnAllocDeviceMemCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ze_device_mem_alloc_desc_t device_desc;
  device_desc.flags = ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT;
  device_desc.ordinal = 1;
  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeDriverAllocDeviceMem(driver, &device_desc, 1, 1, device,
                                   &device_memory));

  zeDriverFreeMem(driver, device_memory);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDriverAllocHostMemCallbacksWhenCallingzeDriverAllocHostMemThenUserDataIsSet) {
  prologues.Driver.pfnAllocHostMemCb = lzt::prologue_callback;
  epilogues.Driver.pfnAllocHostMemCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ze_host_mem_alloc_desc_t host_desc;
  host_desc.flags = ZE_HOST_MEM_ALLOC_FLAG_DEFAULT;
  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeDriverAllocHostMem(driver, &host_desc, 1, 1, &host_memory));

  zeDriverFreeMem(driver, host_memory);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDriverFreeMemCallbacksWhenCallingzeDriverFreeMemThenUserDataIsSet) {
  prologues.Driver.pfnFreeMemCb = lzt::prologue_callback;
  epilogues.Driver.pfnFreeMemCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ze_host_mem_alloc_desc_t host_desc;
  host_desc.flags = ZE_HOST_MEM_ALLOC_FLAG_DEFAULT;
  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeDriverAllocHostMem(driver, &host_desc, 1, 0, &host_memory));

  zeDriverFreeMem(driver, host_memory);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDriverGetMemAllocPropertiesCallbacksWhenCallingzeDriverGetMemAllocPropertiesThenUserDataIsSet) {
  prologues.Driver.pfnGetMemAllocPropertiesCb = lzt::prologue_callback;
  epilogues.Driver.pfnGetMemAllocPropertiesCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDriverGetMemAllocProperties(driver, memory, &mem_alloc_properties, nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDriverGetMemAddressRangeCallbacksWhenCallingzeDriverGetMemAddressRangeThenUserDataIsSet) {

  prologues.Driver.pfnGetMemAddressRangeCb = lzt::prologue_callback;
  epilogues.Driver.pfnGetMemAddressRangeCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDriverGetMemAddressRange(driver, memory, nullptr, nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDriverGetMemIpcHandleCallbacksWhenCallingzeDriverGetMemIpcHandleThenUserDataIsSet) {
  prologues.Driver.pfnGetMemIpcHandleCb = lzt::prologue_callback;
  epilogues.Driver.pfnGetMemIpcHandleCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeDriverGetMemIpcHandle(driver, memory, &ipc_handle);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDriverOpenMemIpcHandleCallbacksWhenCallingzeDriverOpenMemIpcHandleThenUserDataIsSet) {
  prologues.Driver.pfnOpenMemIpcHandleCb = lzt::prologue_callback;
  epilogues.Driver.pfnOpenMemIpcHandleCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeDriverGetMemIpcHandle(driver, memory, &ipc_handle));

  void *mem = nullptr;
  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeDriverOpenMemIpcHandle(driver, device, ipc_handle,
                                     ZE_IPC_MEMORY_FLAG_NONE, &mem));
  zeDriverCloseMemIpcHandle(driver, memory);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeDriverCloseMemIpcHandleCallbacksWhenCallingzeDriverCloseMemIpcHandleThenUserDataIsSet) {
  prologues.Driver.pfnCloseMemIpcHandleCb = lzt::prologue_callback;
  epilogues.Driver.pfnCloseMemIpcHandleCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeDriverGetMemIpcHandle(driver, memory, &ipc_handle));

  void *mem = nullptr;
  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeDriverOpenMemIpcHandle(driver, device, ipc_handle,
                                     ZE_IPC_MEMORY_FLAG_NONE, &mem));
  zeDriverCloseMemIpcHandle(driver, memory);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandQueueCreateCallbacksWhenCallingzeCommandQueueCreateThenUserDataIsSet) {
  prologues.CommandQueue.pfnCreateCb = lzt::prologue_callback;
  epilogues.CommandQueue.pfnCreateCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandQueueDestroy(command_queue);
  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeCommandQueueCreate(device, &command_queue_desc, &command_queue));
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandQueueDestroyCallbacksWhenCallingzeCommandQueueDestroyThenUserDataIsSet) {
  prologues.CommandQueue.pfnDestroyCb = lzt::prologue_callback;
  epilogues.CommandQueue.pfnDestroyCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandQueueDestroy(command_queue);
  command_queue = nullptr;
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandQueueExecuteCommandListsCallbacksWhenCallingzeCommandQueueExecuteCommandListsThenUserDataIsSet) {
  prologues.CommandQueue.pfnExecuteCommandListsCb = lzt::prologue_callback;
  epilogues.CommandQueue.pfnExecuteCommandListsCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandListClose(command_list);
  zeCommandQueueExecuteCommandLists(command_queue, 1, &command_list, nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandQueueSynchronizeCallbacksWhenCallingzeCommandQueueSynchronizeThenUserDataIsSet) {
  prologues.CommandQueue.pfnSynchronizeCb = lzt::prologue_callback;
  epilogues.CommandQueue.pfnSynchronizeCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandQueueSynchronize(command_queue, 0);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListCreateCallbacksWhenCallingzeCommandListCreateThenUserDataIsSet) {
  prologues.CommandList.pfnCreateCb = lzt::prologue_callback;
  epilogues.CommandList.pfnCreateCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandListDestroy(command_list);
  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListCreate(device, &command_list_desc, &command_list));
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListResetCallbacksWhenCallingzeCommandListResetThenUserDataIsSet) {
  prologues.CommandList.pfnResetCb = lzt::prologue_callback;
  epilogues.CommandList.pfnResetCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandListReset(command_list);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListCloseCallbacksWhenCallingzeCommandListCloseThenUserDataIsSet) {
  prologues.CommandList.pfnCloseCb = lzt::prologue_callback;
  epilogues.CommandList.pfnCloseCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandListClose(command_list);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListDestroyCallbacksWhenCallingzeCommandListDestroyThenUserDataIsSet) {

  prologues.CommandList.pfnDestroyCb = lzt::prologue_callback;
  epilogues.CommandList.pfnDestroyCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandListDestroy(command_list);
  command_list = nullptr;
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListCreateImmediateCallbacksWhenCallingzeCommandListCreateImmediateThenUserDataIsSet) {
  prologues.CommandList.pfnCreateImmediateCb = lzt::prologue_callback;
  epilogues.CommandList.pfnCreateImmediateCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ASSERT_EQ(ZE_RESULT_SUCCESS, zeCommandListCreateImmediate(
                                   device, &command_queue_desc, &command_list));
  zeCommandListDestroy(command_list);
  command_list = nullptr;
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendBarrierCallbacksWhenCallingzeCommandListAppendBarrierThenUserDataIsSet) {
  prologues.CommandList.pfnAppendBarrierCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendBarrierCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandListAppendBarrier(command_list, nullptr, 0, nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendMemoryRangesBarrierCallbacksWhenCallingzeCommandListAppendMemoryRangesBarrierThenUserDataIsSet) {
  prologues.CommandList.pfnAppendMemoryRangesBarrierCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendMemoryRangesBarrierCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  const std::vector<size_t> range_sizes{4096, 8192};
  std::vector<const void *> ranges{
      lzt::allocate_device_memory(range_sizes[0] * 2),
      lzt::allocate_device_memory(range_sizes[1] * 2)};

  zeCommandListAppendMemoryRangesBarrier(command_list, ranges.size(),
                                         range_sizes.data(), ranges.data(),
                                         nullptr, 0, nullptr);

  for (auto &range : ranges)
    lzt::free_memory(range);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendMemoryCopyCallbacksWhenCallingzeCommandListAppendMemoryCopyThenUserDataIsSet) {
  prologues.CommandList.pfnAppendMemoryCopyCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendMemoryCopyCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  void *src = lzt::allocate_device_memory(10);
  void *dst = lzt::allocate_device_memory(10);

  zeCommandListAppendMemoryCopy(command_list, dst, src, 0, nullptr);
  zeCommandListDestroy(command_list);
  command_list = nullptr;
  lzt::free_memory(src);
  lzt::free_memory(dst);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendMemoryFillCallbacksWhenCallingzeCommandListAppendMemoryFillThenUserDataIsSet) {
  prologues.CommandList.pfnAppendMemoryFillCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendMemoryFillCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  uint32_t val = 1;
  zeCommandListAppendMemoryFill(command_list, memory, &val, sizeof(uint32_t), 1,
                                nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendMemoryCopyRegionCallbacksWhenCallingzeCommandListAppendMemoryCopyRegionThenUserDataIsSet) {
  prologues.CommandList.pfnAppendMemoryCopyRegionCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendMemoryCopyRegionCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ze_copy_region_t src_region = {0, 0, 0, 1, 1, 0};
  ze_copy_region_t dst_region = {0, 0, 0, 1, 1, 0};

  void *src = lzt::allocate_device_memory(10);
  void *dst = lzt::allocate_device_memory(10);

  zeCommandListAppendMemoryCopyRegion(command_list, dst, &dst_region, 0, 0, src,
                                      &src_region, 0, 0, nullptr);
  zeCommandListDestroy(command_list);
  command_list = nullptr;
  lzt::free_memory(src);
  lzt::free_memory(dst);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendImageCopyCallbacksWhenCallingzeCommandListAppendImageCopyThenUserDataIsSet) {
  prologues.CommandList.pfnAppendImageCopyCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendImageCopyCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ze_image_handle_t src_image;
  ze_image_handle_t dst_image;
  ASSERT_EQ(ZE_RESULT_SUCCESS, zeImageCreate(device, &image_desc, &src_image));
  ASSERT_EQ(ZE_RESULT_SUCCESS, zeImageCreate(device, &image_desc, &dst_image));

  zeCommandListAppendImageCopy(command_list, dst_image, src_image, nullptr);

  zeImageDestroy(src_image);
  zeImageDestroy(dst_image);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendImageCopyRegionCallbacksWhenCallingzeCommandListAppendImageCopyRegionThenUserDataIsSet) {
  prologues.CommandList.pfnAppendImageCopyRegionCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendImageCopyRegionCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ze_image_handle_t src_image;
  ze_image_handle_t dst_image;
  ASSERT_EQ(ZE_RESULT_SUCCESS, zeImageCreate(device, &image_desc, &src_image));
  ASSERT_EQ(ZE_RESULT_SUCCESS, zeImageCreate(device, &image_desc, &dst_image));

  zeCommandListAppendImageCopyRegion(command_list, dst_image, src_image,
                                     nullptr, nullptr, nullptr);

  zeImageDestroy(src_image);
  zeImageDestroy(dst_image);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendImageCopyFromMemoryCallbacksWhenCallingzeCommandListAppendImageCopyFromMemoryThenUserDataIsSet) {
  prologues.CommandList.pfnAppendImageCopyFromMemoryCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendImageCopyFromMemoryCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandListAppendImageCopyFromMemory(command_list, image, memory, nullptr,
                                         nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendImageCopyToMemoryCallbacksWhenCallingzeCommandListAppendImageCopyToMemoryThenUserDataIsSet) {
  prologues.CommandList.pfnAppendImageCopyToMemoryCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendImageCopyToMemoryCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandListAppendImageCopyToMemory(command_list, memory, image, nullptr,
                                       nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendMemoryPrefetchCallbacksWhenCallingzeCommandListAppendMemoryPrefetchThenUserDataIsSet) {
  prologues.CommandList.pfnAppendMemoryPrefetchCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendMemoryPrefetchCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandListAppendMemoryPrefetch(command_list, memory, 0);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendMemAdviseCallbacksWhenCallingzeCommandListAppendMemAdviseThenUserDataIsSet) {
  prologues.CommandList.pfnAppendMemAdviseCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendMemAdviseCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandListAppendMemAdvise(command_list, device, memory, 10,
                               ZE_MEMORY_ADVICE_SET_READ_MOSTLY);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendSignalEventCallbacksWhenCallingzeCommandListAppendSignalEventThenUserDataIsSet) {
  prologues.CommandList.pfnAppendSignalEventCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendSignalEventCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandListAppendSignalEvent(command_list, event);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendWaitOnEventsCallbacksWhenCallingzeCommandListAppendWaitOnEventsThenUserDataIsSet) {
  prologues.CommandList.pfnAppendWaitOnEventsCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendWaitOnEventsCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandListAppendWaitOnEvents(command_list, 1, &event);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendEventResetCallbacksWhenCallingzeCommandListAppendEventResetThenUserDataIsSet) {
  prologues.CommandList.pfnAppendEventResetCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendEventResetCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeCommandListAppendEventReset(command_list, event);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendLaunchKernelCallbacksWhenCallingzeCommandListAppendLaunchKernelThenUserDataIsSet) {
  prologues.CommandList.pfnAppendLaunchKernelCb = lzt::prologue_callback;
  epilogues.CommandList.pfnAppendLaunchKernelCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ze_group_count_t args = {1, 0, 0};

  zeCommandListAppendLaunchKernel(command_list, kernel, &args, nullptr, 0,
                                  nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendLaunchMultipleKernelsIndirectCallbacksWhenCallingzeCommandListAppendLaunchMultipleKernelsIndirectThenUserDataIsSet) {
  prologues.CommandList.pfnAppendLaunchMultipleKernelsIndirectCb =
      lzt::prologue_callback;
  epilogues.CommandList.pfnAppendLaunchMultipleKernelsIndirectCb =
      lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  uint32_t count = 1;
  ze_group_count_t args = {1, 0, 0};

  zeCommandListAppendLaunchMultipleKernelsIndirect(
      command_list, 1, &kernel, &count, &args, nullptr, 0, nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeFenceCreateCallbacksWhenCallingzeFenceCreateThenUserDataIsSet) {
  prologues.Fence.pfnCreateCb = lzt::prologue_callback;
  epilogues.Fence.pfnCreateCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeFenceDestroy(fence);
  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeFenceCreate(command_queue, &fence_desc, &fence));
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeFenceDestroyCallbacksWhenCallingzeFenceDestroyThenUserDataIsSet) {
  prologues.Fence.pfnDestroyCb = lzt::prologue_callback;
  epilogues.Fence.pfnDestroyCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeFenceDestroy(fence);
  fence = nullptr;
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeFenceHostSynchronizeCallbacksWhenCallingzeFenceHostSynchronizeThenUserDataIsSet) {
  prologues.Fence.pfnHostSynchronizeCb = lzt::prologue_callback;
  epilogues.Fence.pfnHostSynchronizeCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeFenceHostSynchronize(fence, 0);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeFenceQueryStatusCallbacksWhenCallingzeFenceQueryStatusThenUserDataIsSet) {
  prologues.Fence.pfnQueryStatusCb = lzt::prologue_callback;
  epilogues.Fence.pfnQueryStatusCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeFenceQueryStatus(fence);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeFenceResetCallbacksWhenCallingzeFenceResetThenUserDataIsSet) {
  prologues.Fence.pfnResetCb = lzt::prologue_callback;
  epilogues.Fence.pfnResetCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeFenceReset(fence);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeEventPoolCreateCallbacksWhenCallingzeEventPoolCreateThenUserDataIsSet) {
  prologues.EventPool.pfnCreateCb = lzt::prologue_callback;
  epilogues.EventPool.pfnCreateCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeEventPoolDestroy(event_pool);
  ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventPoolCreate(driver, &event_pool_desc, 1,
                                                 &device, &event_pool));
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeEventPoolDestroyCallbacksWhenCallingzeEventPoolDestroyThenUserDataIsSet) {
  prologues.EventPool.pfnDestroyCb = lzt::prologue_callback;
  epilogues.EventPool.pfnDestroyCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeEventPoolDestroy(event_pool);
  event_pool = nullptr;
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeEventCreateCallbacksWhenCallingzeEventCreateThenUserDataIsSet) {
  prologues.Event.pfnCreateCb = lzt::prologue_callback;
  epilogues.Event.pfnCreateCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeEventDestroy(event);
  ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventCreate(event_pool, &event_desc, &event));
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeEventDestroyCallbacksWhenCallingzeEventDestroyThenUserDataIsSet) {
  prologues.Event.pfnDestroyCb = lzt::prologue_callback;
  epilogues.Event.pfnDestroyCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeEventDestroy(event);
  event = nullptr;
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeEventPoolGetIpcHandleCallbacksWhenCallingzeEventPoolGetIpcHandleThenUserDataIsSet) {
  prologues.EventPool.pfnGetIpcHandleCb = lzt::prologue_callback;
  epilogues.EventPool.pfnGetIpcHandleCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeEventPoolGetIpcHandle(event_pool, &ipc_event);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeEventPoolOpenIpcHandleCallbacksWhenCallingzeEventPoolOpenIpcHandleThenUserDataIsSet) {
  prologues.EventPool.pfnOpenIpcHandleCb = lzt::prologue_callback;
  epilogues.EventPool.pfnOpenIpcHandleCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ze_ipc_event_pool_handle_t handle;
  ze_event_pool_handle_t event_pool2;
  ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventPoolGetIpcHandle(event_pool, &ipc_event));
  zeEventPoolOpenIpcHandle(driver, handle, &event_pool2);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeEventPoolCloseIpcHandleCallbacksWhenCallingzeEventPoolCloseIpcHandleThenUserDataIsSet) {
  prologues.EventPool.pfnCloseIpcHandleCb = lzt::prologue_callback;
  epilogues.EventPool.pfnCloseIpcHandleCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeEventPoolCloseIpcHandle(event_pool);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeEventHostSignalCallbacksWhenCallingzeEventHostSignalThenUserDataIsSet) {
  prologues.Event.pfnHostSignalCb = lzt::prologue_callback;
  epilogues.Event.pfnHostSignalCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeEventHostSignal(event);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeEventHostSynchronizeCallbacksWhenCallingzeEventHostSynchronizeThenUserDataIsSet) {
  prologues.Event.pfnHostSynchronizeCb = lzt::prologue_callback;
  epilogues.Event.pfnHostSynchronizeCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeEventHostSynchronize(event, 0);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeEventQueryStatusCallbacksWhenCallingzeEventQueryStatusThenUserDataIsSet) {
  prologues.Event.pfnQueryStatusCb = lzt::prologue_callback;
  epilogues.Event.pfnQueryStatusCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeEventQueryStatus(nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeEventHostResetCallbacksWhenCallingzeEventHostResetThenUserDataIsSet) {
  prologues.Event.pfnHostResetCb = lzt::prologue_callback;
  epilogues.Event.pfnHostResetCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeEventHostReset(event);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeImageCreateCallbacksWhenCallingzeImageCreateThenUserDataIsSet) {
  prologues.Image.pfnCreateCb = lzt::prologue_callback;
  epilogues.Image.pfnCreateCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeImageDestroy(image);
  ASSERT_EQ(ZE_RESULT_SUCCESS, zeImageCreate(device, &image_desc, &image));
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeImageGetPropertiesCallbacksWhenCallingzeImageGetPropertiesThenUserDataIsSet) {
  prologues.Image.pfnGetPropertiesCb = lzt::prologue_callback;
  epilogues.Image.pfnGetPropertiesCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeImageGetProperties(device, &image_desc, &image_properties);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeImageDestroyCallbacksWhenCallingzeImageDestroyThenUserDataIsSet) {
  prologues.Image.pfnDestroyCb = lzt::prologue_callback;
  epilogues.Image.pfnDestroyCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeImageDestroy(image);
  image = nullptr;
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeModuleCreateCallbacksWhenCallingzeModuleCreateThenUserDataIsSet) {
  prologues.Module.pfnCreateCb = lzt::prologue_callback;
  epilogues.Module.pfnCreateCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeModuleDestroy(module);
  zeModuleCreate(device, &module_desc, &module, nullptr);
  kernel = nullptr;
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeModuleDestroyCallbacksWhenCallingzeModuleDestroyThenUserDataIsSet) {
  prologues.Module.pfnDestroyCb = lzt::prologue_callback;
  epilogues.Module.pfnDestroyCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeModuleDestroy(module);
  module = nullptr;
  kernel = nullptr;
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeModuleGetNativeBinaryCallbacksWhenCallingzeModuleGetNativeBinaryThenUserDataIsSet) {
  prologues.Module.pfnGetNativeBinaryCb = lzt::prologue_callback;
  epilogues.Module.pfnGetNativeBinaryCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  size_t binary_size;
  zeModuleGetNativeBinary(module, &binary_size, nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeModuleGetGlobalPointerCallbacksWhenCallingzeModuleGetGlobalPointerThenUserDataIsSet) {
  prologues.Module.pfnGetGlobalPointerCb = lzt::prologue_callback;
  epilogues.Module.pfnGetGlobalPointerCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  void *pointer;
  zeModuleGetGlobalPointer(module, "xid", &pointer);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeModuleGetFunctionPointerCallbacksWhenCallingzeModuleGetFunctionPointerThenUserDataIsSet) {
  prologues.Module.pfnGetFunctionPointerCb = lzt::prologue_callback;
  epilogues.Module.pfnGetFunctionPointerCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  void *function_pointer;
  zeModuleGetFunctionPointer(module, "module_add_constant", &function_pointer);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeModuleBuildLogDestroyCallbacksWhenCallingzeModuleBuildLogDestroyThenUserDataIsSet) {
  prologues.ModuleBuildLog.pfnDestroyCb = lzt::prologue_callback;
  epilogues.ModuleBuildLog.pfnDestroyCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeModuleBuildLogDestroy(build_log);
  build_log = nullptr;
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeModuleBuildLogGetStringCallbacksWhenCallingzeModuleBuildLogGetStringThenUserDataIsSet) {
  prologues.ModuleBuildLog.pfnGetStringCb = lzt::prologue_callback;
  epilogues.ModuleBuildLog.pfnGetStringCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  size_t log_size;
  zeModuleBuildLogGetString(build_log, &log_size, nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeKernelCreateCallbacksWhenCallingzeKernelCreateThenUserDataIsSet) {
  prologues.Kernel.pfnCreateCb = lzt::prologue_callback;
  epilogues.Kernel.pfnCreateCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeKernelDestroy(kernel);
  zeKernelCreate(module, &kernel_desc, &kernel);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeKernelDestroyCallbacksWhenCallingzeKernelDestroyThenUserDataIsSet) {
  prologues.Kernel.pfnDestroyCb = lzt::prologue_callback;
  epilogues.Kernel.pfnDestroyCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeKernelDestroy(kernel);
  kernel = nullptr;
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeCommandListAppendLaunchKernelIndirectCallbacksWhenCallingzeCommandListAppendLaunchKernelIndirectThenUserDataIsSet) {
  prologues.CommandList.pfnAppendLaunchKernelIndirectCb =
      lzt::prologue_callback;
  epilogues.CommandList.pfnAppendLaunchKernelIndirectCb =
      lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ze_group_count_t args = {1, 0, 0};

  zeCommandListAppendLaunchKernelIndirect(command_list, kernel, &args, nullptr,
                                          0, nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeKernelSetIntermediateCacheConfigCallbacksWhenCallingzeKernelSetIntermediateCacheConfigThenUserDataIsSet) {
  prologues.Kernel.pfnSetIntermediateCacheConfigCb = lzt::prologue_callback;
  epilogues.Kernel.pfnSetIntermediateCacheConfigCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeKernelSetIntermediateCacheConfig(kernel, ZE_CACHE_CONFIG_DEFAULT);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeKernelSetGroupSizeCallbacksWhenCallingzeKernelSetGroupSizeThenUserDataIsSet) {
  prologues.Kernel.pfnSetGroupSizeCb = lzt::prologue_callback;
  epilogues.Kernel.pfnSetGroupSizeCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeKernelSetGroupSize(kernel, 1, 1, 1);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeKernelSuggestGroupSizeCallbacksWhenCallingzeKernelSuggestGroupSizeThenUserDataIsSet) {
  prologues.Kernel.pfnSuggestGroupSizeCb = lzt::prologue_callback;
  epilogues.Kernel.pfnSuggestGroupSizeCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeKernelSuggestGroupSize(kernel, 1, 1, 1, nullptr, nullptr, nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeKernelSetArgumentValueCallbacksWhenCallingzeKernelSetArgumentValueThenUserDataIsSet) {
  prologues.Kernel.pfnSetArgumentValueCb = lzt::prologue_callback;
  epilogues.Kernel.pfnSetArgumentValueCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  zeKernelSetArgumentValue(kernel, 0, sizeof(int), nullptr);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeKernelSetAttributeCallbacksWhenCallingzeKernelSetAttributeThenUserDataIsSet) {
  prologues.Kernel.pfnSetAttributeCb = lzt::prologue_callback;
  epilogues.Kernel.pfnSetAttributeCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  bool value = false;
  zeKernelSetAttribute(kernel, ZE_KERNEL_ATTR_INDIRECT_HOST_ACCESS,
                       sizeof(bool), &value);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeKernelGetPropertiesCallbacksWhenCallingzeKernelGetPropertiesThenUserDataIsSet) {
  prologues.Kernel.pfnGetPropertiesCb = lzt::prologue_callback;
  epilogues.Kernel.pfnGetPropertiesCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ze_kernel_properties_t kernel_properties;
  zeKernelGetProperties(kernel, &kernel_properties);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeSamplerCreateCallbacksWhenCallingzeSamplerCreateThenUserDataIsSet) {
  prologues.Sampler.pfnCreateCb = lzt::prologue_callback;
  epilogues.Sampler.pfnCreateCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeSamplerCreate(device, &sampler_desc, &sampler));
  zeSamplerDestroy(sampler);
}

TEST_F(
    TracingPrologueEpilogueTests,
    GivenEnabledTracerWithzeSamplerDestroyCallbacksWhenCallingzeSamplerDestroyThenUserDataIsSet) {
  prologues.Sampler.pfnDestroyCb = lzt::prologue_callback;
  epilogues.Sampler.pfnDestroyCb = lzt::epilogue_callback;
  ready_tracer(tracer_handle, prologues, epilogues);

  ASSERT_EQ(ZE_RESULT_SUCCESS,
            zeSamplerCreate(device, &sampler_desc, &sampler));
  zeSamplerDestroy(sampler);
}

} // namespace
