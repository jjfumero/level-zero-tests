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

namespace cs = compute_samples;

namespace compute_samples {

xe_event_pool_handle_t create_event_pool(uint32_t count,
                                         xe_event_pool_flag_t flags) {
  xe_event_pool_handle_t event_pool;
  xe_event_pool_desc_t descriptor;

  descriptor.version = XE_EVENT_POOL_DESC_VERSION_CURRENT;
  descriptor.flags = flags;
  descriptor.count = count;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeEventPoolCreate(xeDevice::get_instance()->get_device(),
                              &descriptor, &event_pool));
  EXPECT_NE(nullptr, event_pool);
  return event_pool;
}

void destroy_event_pool(xe_event_pool_handle_t event_pool) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventPoolDestroy(event_pool));
}

void xeEventPool::InitEventPool() {
  InitEventPool(32, XE_EVENT_POOL_FLAG_DEFAULT);
}

void xeEventPool::InitEventPool(uint32_t count, xe_event_pool_flag_t flags) {
  if (event_pool_ == nullptr) {
    event_pool_ = create_event_pool(count, flags);
    pool_indexes_available_.resize(count, true);
  }
}

xeEventPool::xeEventPool() {}

xeEventPool::~xeEventPool() {
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

void xeEventPool::create_event(xe_event_handle_t &event) {
  create_event(event, XE_EVENT_SCOPE_FLAG_NONE, XE_EVENT_SCOPE_FLAG_NONE);
}

void xeEventPool::create_event(xe_event_handle_t &event,
                               xe_event_scope_flag_t signal,
                               xe_event_scope_flag_t wait) {
  // Make sure the event pool is initialized to at least defaults:
  InitEventPool();
  xe_event_desc_t desc;
  memset(&desc, 0, sizeof(desc));
  desc.version = XE_EVENT_DESC_VERSION_CURRENT;
  desc.signal = signal;
  desc.wait = wait;
  event = nullptr;
  desc.index = find_index(pool_indexes_available_);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventCreate(event_pool_, &desc, &event));
  EXPECT_NE(nullptr, event);
  handle_to_index_map_[event] = desc.index;
  pool_indexes_available_[desc.index] = false;
}

void xeEventPool::create_events(std::vector<xe_event_handle_t> &events,
                                size_t event_count) {
  create_events(events, event_count, XE_EVENT_SCOPE_FLAG_NONE,
                XE_EVENT_SCOPE_FLAG_NONE);
}

void xeEventPool::create_events(std::vector<xe_event_handle_t> &events,
                                size_t event_count,
                                xe_event_scope_flag_t signal,
                                xe_event_scope_flag_t wait) {
  events.resize(event_count);
  for (auto &event : events)
    create_event(event, signal, wait);
}

void xeEventPool::destroy_event(xe_event_handle_t event) {
  std::map<xe_event_handle_t, uint32_t>::iterator it =
      handle_to_index_map_.find(event);

  EXPECT_NE(it, handle_to_index_map_.end());
  pool_indexes_available_[(*it).second] = true;
  handle_to_index_map_.erase(it);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventDestroy(event));
}

void xeEventPool::destroy_events(std::vector<xe_event_handle_t> &events) {
  for (auto &event : events)
    destroy_event(event);
  events.clear();
}

void xeEventPool::get_ipc_handle(xe_ipc_event_pool_handle_t &hIpc) {
  // As of July 10, 2019, xeEventPoolGetIpcHandle() returns UNSUPPORTED
  // and thus the following test fails:
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventPoolGetIpcHandle(event_pool_, &hIpc));
}

void xeEventPool::open_ipc_handle(xe_ipc_event_pool_handle_t hIpc,
                                  xe_event_pool_handle_t &eventPool) {
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeEventPoolOpenIpcHandle(xeDevice::get_instance()->get_device(),
                                     hIpc, &eventPool));
}

void xeEventPool::close_ipc_handle_pool(xe_event_pool_handle_t &eventPool) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventPoolCloseIpcHandle(eventPool));
}

}; // namespace compute_samples
