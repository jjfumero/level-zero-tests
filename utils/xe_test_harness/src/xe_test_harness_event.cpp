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

#include "xe_test_harness/xe_test_harness.hpp"
#include "gtest/gtest.h"

namespace lzt = level_zero_tests;

namespace level_zero_tests {

ze_event_pool_handle_t create_event_pool(uint32_t count,
                                         ze_event_pool_flag_t flags) {
  ze_event_pool_handle_t event_pool;
  ze_event_pool_desc_t descriptor;

  descriptor.version = ZE_EVENT_POOL_DESC_VERSION_CURRENT;
  descriptor.flags = flags;
  descriptor.count = count;
  ze_driver_handle_t driver = lzt::get_default_driver();
  ze_device_handle_t device = zeDevice::get_instance()->get_device();
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventPoolCreate(driver, &descriptor,
                                                 lzt::get_device_count(driver),
                                                 &device, &event_pool));
  EXPECT_NE(nullptr, event_pool);
  return event_pool;
}

void destroy_event_pool(ze_event_pool_handle_t event_pool) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventPoolDestroy(event_pool));
}

void zeEventPool::InitEventPool() { InitEventPool(32); }

void zeEventPool::InitEventPool(uint32_t count) {
  InitEventPool(count, ZE_EVENT_POOL_FLAG_DEFAULT);
}
void zeEventPool::InitEventPool(uint32_t count, ze_event_pool_flag_t flags) {
  if (event_pool_ == nullptr) {
    event_pool_ = create_event_pool(count, flags);
    pool_indexes_available_.resize(count, true);
  }
}

zeEventPool::zeEventPool() {}

zeEventPool::~zeEventPool() {
  // If the event pool was never created, do not attempt to destroy it
  // as that will needlessly cause a test failure.
  if (event_pool_)
    destroy_event_pool(event_pool_);
}

uint32_t find_index(const std::vector<bool> &indexes_available) {
  for (uint32_t i = 0; i < indexes_available.size(); i++)
    if (indexes_available[i])
      return i;
  return -1;
}

void zeEventPool::create_event(ze_event_handle_t &event) {
  create_event(event, ZE_EVENT_SCOPE_FLAG_NONE, ZE_EVENT_SCOPE_FLAG_NONE);
}

void zeEventPool::create_event(ze_event_handle_t &event,
                               ze_event_scope_flag_t signal,
                               ze_event_scope_flag_t wait) {
  // Make sure the event pool is initialized to at least defaults:
  InitEventPool();
  ze_event_desc_t desc;
  memset(&desc, 0, sizeof(desc));
  desc.version = ZE_EVENT_DESC_VERSION_CURRENT;
  desc.signal = signal;
  desc.wait = wait;
  event = nullptr;
  desc.index = find_index(pool_indexes_available_);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventCreate(event_pool_, &desc, &event));
  EXPECT_NE(nullptr, event);
  handle_to_index_map_[event] = desc.index;
  pool_indexes_available_[desc.index] = false;
}

void zeEventPool::create_events(std::vector<ze_event_handle_t> &events,
                                size_t event_count) {
  create_events(events, event_count, ZE_EVENT_SCOPE_FLAG_NONE,
                ZE_EVENT_SCOPE_FLAG_NONE);
}

void zeEventPool::create_events(std::vector<ze_event_handle_t> &events,
                                size_t event_count,
                                ze_event_scope_flag_t signal,
                                ze_event_scope_flag_t wait) {
  events.resize(event_count);
  for (auto &event : events)
    create_event(event, signal, wait);
}

void zeEventPool::destroy_event(ze_event_handle_t event) {
  std::map<ze_event_handle_t, uint32_t>::iterator it =
      handle_to_index_map_.find(event);

  EXPECT_NE(it, handle_to_index_map_.end());
  pool_indexes_available_[(*it).second] = true;
  handle_to_index_map_.erase(it);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(event));
}

void zeEventPool::destroy_events(std::vector<ze_event_handle_t> &events) {
  for (auto &event : events)
    destroy_event(event);
  events.clear();
}

void zeEventPool::get_ipc_handle(ze_ipc_event_pool_handle_t *hIpc) {
  // As of July 10, 2019, zeEventPoolGetIpcHandle() returns UNSUPPORTED
  // and thus the following test fails:
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventPoolGetIpcHandle(event_pool_, hIpc));
}

void zeEventPool::open_ipc_handle(ze_ipc_event_pool_handle_t &hIpc,
                                  ze_event_pool_handle_t *eventPool) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventPoolOpenIpcHandle(
                                   lzt::get_default_driver(), hIpc, eventPool));
}

void zeEventPool::close_ipc_handle_pool(ze_event_pool_handle_t &eventPool) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventPoolCloseIpcHandle(eventPool));
}

}; // namespace level_zero_tests
