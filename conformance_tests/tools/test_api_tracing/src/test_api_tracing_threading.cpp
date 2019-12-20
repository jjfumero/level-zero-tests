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

#include <thread>
#include <atomic>

#include "gtest/gtest.h"

#include "logging/logging.hpp"
#include "test_harness/test_harness.hpp"
#include "ze_api.h"
#include "zet_api.h"

namespace lzt = level_zero_tests;

namespace {

std::atomic_bool ready(false);
std::atomic_bool signaling_thread(false);
std::atomic_int callback_enter_invokations(0);
std::atomic_int callback_exit_invokations(0);

void OnEnterDriverAllocDeviceMem(
    ze_driver_alloc_device_mem_params_t *tracer_params, ze_result_t result,
    void *pTraceUserData, void **ppTracerInstanceUserData) {

  callback_enter_invokations++;
  while (!signaling_thread && !ready) {
  }
}

void OnExitDriverAllocDeviceMem(
    ze_driver_alloc_device_mem_params_t *tracer_params, ze_result_t result,
    void *pTraceUserData, void **ppTracerInstanceUserData) {

  callback_exit_invokations++;
}

void OnEnterDriverAllocHostMem(ze_driver_alloc_host_mem_params_t *tracer_params,
                               ze_result_t result, void *pTraceUserData,
                               void **ppTracerInstanceUserData) {}

void OnExitDriverAllocHostMem(ze_driver_alloc_host_mem_params_t *tracer_params,
                              ze_result_t result, void *pTraceUserData,
                              void **ppTracerInstanceUserData) {}

void allocate_then_deallocate_device_memory() {
  void *memory = lzt::allocate_device_memory(1);
  lzt::free_memory(memory);
}

class TracingThreadTests : public ::testing::Test {
protected:
  void SetUp() override {
    callback_exit_invokations = 0;
    callback_enter_invokations = 0;
    ready = false;
    signaling_thread = false;
    zet_tracer_desc_t tracer_desc;
    tracer_desc.version = ZET_TRACER_DESC_VERSION_CURRENT;
    tracer_desc.pUserData = nullptr;
    tracer = lzt::create_tracer_handle(tracer_desc);

    prologues.Driver.pfnAllocDeviceMemCb = OnEnterDriverAllocDeviceMem;
    epilogues.Driver.pfnAllocDeviceMemCb = OnExitDriverAllocDeviceMem;

    lzt::set_tracer_prologues(tracer, prologues);
    lzt::set_tracer_epilogues(tracer, epilogues);
    lzt::enable_tracer(tracer);
  }

  void TearDown() {
    lzt::disable_tracer(tracer);
    lzt::destroy_tracer_handle(tracer);
  }

  zet_tracer_handle_t tracer;
  ze_callbacks_t prologues = {};
  ze_callbacks_t epilogues = {};
};

TEST_F(
    TracingThreadTests,
    GivenSingleTracingEnabledThreadWhenCallingDifferentAPIFunctionThenCallbacksCalledOnce) {

  std::thread child_thread(allocate_then_deallocate_device_memory);
  void *memory = lzt::allocate_host_memory(100);
  ready = true;

  lzt::free_memory(memory);
  child_thread.join();

  EXPECT_EQ(callback_enter_invokations, 1);
  EXPECT_EQ(callback_exit_invokations, 1);
}

TEST_F(
    TracingThreadTests,
    GivenSingleTracingEnabledThreadWhenCallingSameAPIFunctionThenCallbackCalledTwice) {

  std::thread child_thread(allocate_then_deallocate_device_memory);
  signaling_thread = true;
  void *memory = lzt::allocate_device_memory(100);
  ready = true;

  lzt::free_memory(memory);
  child_thread.join();

  EXPECT_EQ(2, callback_enter_invokations);
  EXPECT_EQ(2, callback_exit_invokations);
}

void trace_memory_allocation_then_deallocate(ze_memory_type_t memory_type) {

  zet_tracer_handle_t tracer;
  ze_callbacks_t prologues = {};
  ze_callbacks_t epilogues = {};

  zet_tracer_desc_t tracer_desc = {};
  tracer_desc.version = ZET_TRACER_DESC_VERSION_CURRENT;
  tracer_desc.pUserData = nullptr;
  tracer = lzt::create_tracer_handle(tracer_desc);

  if (memory_type == ZE_MEMORY_TYPE_DEVICE) {
    prologues.Driver.pfnAllocDeviceMemCb = OnEnterDriverAllocDeviceMem;
    epilogues.Driver.pfnAllocDeviceMemCb = OnExitDriverAllocDeviceMem;
  } else if (memory_type == ZE_MEMORY_TYPE_HOST) {
    prologues.Driver.pfnAllocHostMemCb = OnEnterDriverAllocHostMem;
    epilogues.Driver.pfnAllocHostMemCb = OnExitDriverAllocHostMem;
  }

  lzt::set_tracer_prologues(tracer, prologues);
  lzt::set_tracer_epilogues(tracer, epilogues);
  lzt::enable_tracer(tracer);

  void *memory;
  if (memory_type == ZE_MEMORY_TYPE_DEVICE) {
    memory = lzt::allocate_device_memory(1);
  } else if (memory_type == ZE_MEMORY_TYPE_HOST) {
    memory = lzt::allocate_host_memory(1);
  }

  lzt::free_memory(memory);
}

TEST_F(
    TracingThreadTests,
    GivenTwoTracingEnabledThreadsWhenCallingDifferentAPIFunctionThenCallbackCalledOnce) {

  std::thread second_trace_thread(trace_memory_allocation_then_deallocate,
                                  ZE_MEMORY_TYPE_HOST);
  signaling_thread = true;
  void *memory = lzt::allocate_device_memory(100);
  ready = true;

  lzt::free_memory(memory);
  second_trace_thread.join();

  EXPECT_EQ(1, callback_enter_invokations);
  EXPECT_EQ(1, callback_exit_invokations);
}

TEST_F(
    TracingThreadTests,
    GivenTwoTracingEnabledThreadsWhenCallingSameAPIFunctionThenCallbacksCalledTwice) {

  std::thread second_child_thread(trace_memory_allocation_then_deallocate,
                                  ZE_MEMORY_TYPE_DEVICE);
  signaling_thread = true;
  void *memory = lzt::allocate_device_memory(100);
  ready = true;

  lzt::free_memory(memory);
  second_child_thread.join();

  EXPECT_EQ(2, callback_enter_invokations);
  EXPECT_EQ(2, callback_exit_invokations);
}

class TracingThreadTestsDisabling : public TracingThreadTests {
protected:
  void TearDown() override {}
};

TEST_F(
    TracingThreadTestsDisabling,
    GivenInvokedPrologueWhenDisablingTracerInSeparateThreadThenEpilogueIsCalled) {

  std::thread child_thread(allocate_then_deallocate_device_memory);
  lzt::disable_tracer(tracer);
  ready = true;
  child_thread.join();
  EXPECT_EQ(callback_exit_invokations, 1);
  lzt::destroy_tracer_handle(tracer);
}

} // namespace
