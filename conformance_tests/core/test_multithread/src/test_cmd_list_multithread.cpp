/*
 *
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gtest/gtest.h"

#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"
#include "logging/logging.hpp"
#include <thread>
#include <array>

namespace lzt = level_zero_tests;

#include <level_zero/ze_api.h>

namespace {

constexpr uint32_t thread_iters = 2000;
constexpr size_t num_threads = 16;
constexpr size_t num_events = 16;
constexpr size_t size = 4096;

void thread_create_destroy_command_list_default() {
  std::thread::id thread_id = std::this_thread::get_id();

  LOG_DEBUG << "child thread spawned with ID :: " << thread_id;

  for (uint32_t i = 0; i < thread_iters; i++) {
    ze_command_list_handle_t cmd_list = lzt::create_command_list();
    lzt::close_command_list(cmd_list);
    lzt::destroy_command_list(cmd_list);
  }

  LOG_DEBUG << "child thread done with ID ::" << std::this_thread::get_id();
}

void thread_create_destroy_command_list_valid_flags(
    ze_device_handle_t device, ze_command_list_flag_t flags) {
  std::thread::id thread_id = std::this_thread::get_id();

  LOG_DEBUG << "child thread spawned with ID :: " << thread_id;

  for (uint32_t i = 0; i < thread_iters; i++) {
    ze_command_list_handle_t cmd_list = lzt::create_command_list(device, flags);
    lzt::close_command_list(cmd_list);
    lzt::destroy_command_list(cmd_list);
  }

  LOG_DEBUG << "child thread done with ID ::" << std::this_thread::get_id();
}

void thread_append_memory_copy(const ze_command_queue_handle_t cq) {
  std::thread::id thread_id = std::this_thread::get_id();

  LOG_DEBUG << "child thread spawned with ID :: " << thread_id;

  // Each thread creates event pool,events and uses them to sync
  // and later destroy events

  ze_event_pool_desc_t eventPoolDesc = {};
  eventPoolDesc.stype = ZE_STRUCTURE_TYPE_EVENT_POOL_DESC;
  eventPoolDesc.flags = ZE_EVENT_POOL_FLAG_HOST_VISIBLE;
  eventPoolDesc.count = num_events;

  for (uint32_t i = 0; i < thread_iters; i++) {
    ze_event_pool_handle_t event_pool = lzt::create_event_pool(eventPoolDesc);
    std::array<ze_event_handle_t, num_events> events;
    std::array<ze_event_desc_t, num_events> eventDesc;
    std::array<ze_command_list_handle_t, num_events> cmd_list;
    std::array<uint8_t, size> host_memory1, host_memory2;
    for (uint32_t j = 0; j < num_events; j++) {
      eventDesc[j].stype = ZE_STRUCTURE_TYPE_EVENT_DESC;
      eventDesc[j].pNext = nullptr;
      eventDesc[j].index = j;
      eventDesc[j].signal = ZE_EVENT_SCOPE_FLAG_HOST;
      eventDesc[j].wait = ZE_EVENT_SCOPE_FLAG_HOST;
      events[j] = lzt::create_event(event_pool, eventDesc[j]);
      cmd_list[j] = lzt::create_command_list();
    }

    for (auto event : events) {
      EXPECT_EQ(ZE_RESULT_NOT_READY, zeEventQueryStatus(event));
    }

    for (uint32_t j = 0; j < num_events; j++) {
      void *device_memory = lzt::allocate_device_memory(size);
      ze_fence_handle_t fence_ = lzt::create_fence(cq);
      lzt::write_data_pattern(host_memory1.data(), size, 1);
      EXPECT_EQ(ZE_RESULT_NOT_READY, zeFenceQueryStatus(fence_));

      lzt::append_wait_on_events(cmd_list[j], 1, &events[j]);
      lzt::append_memory_copy(cmd_list[j], device_memory, host_memory1.data(),
                              size, nullptr);
      lzt::close_command_list(cmd_list[j]);
      lzt::execute_command_lists(cq, 1, &cmd_list[j], fence_);
      lzt::signal_event_from_host(events[j]);
      lzt::query_event(events[j]);
      lzt::sync_fence(fence_, UINT64_MAX);
      lzt::query_fence(fence_);

      lzt::reset_fence(fence_);
      EXPECT_EQ(ZE_RESULT_NOT_READY, zeFenceQueryStatus(fence_));
      lzt::event_host_reset(events[j]);
      lzt::reset_command_list(cmd_list[j]);

      lzt::append_wait_on_events(cmd_list[j], 1, &events[j]);
      lzt::append_memory_copy(cmd_list[j], host_memory2.data(), device_memory,
                              size, nullptr);
      lzt::close_command_list(cmd_list[j]);
      lzt::execute_command_lists(cq, 1, &cmd_list[j], fence_);
      lzt::signal_event_from_host(events[j]);
      lzt::query_event(events[j]);
      lzt::sync_fence(fence_, UINT64_MAX);
      lzt::query_fence(fence_);

      lzt::validate_data_pattern(host_memory2.data(), size, 1);

      lzt::destroy_fence(fence_);
      lzt::free_memory(device_memory);
    }
    for (auto event : events) {
      lzt::destroy_event(event);
    }
    for (auto cl : cmd_list) {
      lzt::destroy_command_list(cl);
    }
    lzt::destroy_event_pool(event_pool);
  }
}

class zeCommandListMultithreadTest : public ::testing::Test {};

TEST(
    zeCommandListMultithreadTest,
    GivenMultipleThreadsWhenCreateCloseAndDestroyCommandListsThenReturnSuccess) {
  LOG_DEBUG << "Total number of threads spawned ::" << num_threads;

  std::array<std::unique_ptr<std::thread>, num_threads> threads;

  for (uint32_t i = 0; i < num_threads; i++) {
    threads[i] = std::make_unique<std::thread>(
        thread_create_destroy_command_list_default);
  }

  for (uint32_t i = 0; i < num_threads; i++) {
    threads[i]->join();
  }
}

TEST(zeCommandListMultithreadTest,
     GivenMultipleThreadsWhenUsingCommandListsWithFenceThenReturnSuccess) {
  LOG_DEBUG << "Total number of threads spawned ::" << num_threads;

  ze_command_queue_handle_t cq = lzt::create_command_queue();

  std::array<std::unique_ptr<std::thread>, num_threads> threads;

  for (uint32_t i = 0; i < num_threads; i++) {
    threads[i] = std::make_unique<std::thread>(thread_append_memory_copy, cq);
  }

  for (uint32_t i = 0; i < num_threads; i++) {
    threads[i]->join();
  }

  lzt::destroy_command_queue(cq);
}

class zeCommandListCreateDestroyValidFlagsMultithreadTest
    : public ::testing::Test,
      public ::testing::WithParamInterface<ze_command_list_flag_t> {};

TEST_P(
    zeCommandListCreateDestroyValidFlagsMultithreadTest,
    GivenMultipleThreadsWhenCreateCloseAndDestroyCommandListWithValidFlagValuesThenReturnSuccess) {
  LOG_DEBUG << "Total number of threads spawned ::" << num_threads;

  std::array<std::unique_ptr<std::thread>, num_threads> threads;

  ze_command_list_flag_t flags = GetParam(); // flags

  ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();

  for (uint32_t i = 0; i < num_threads; i++) {
    threads[i] = std::make_unique<std::thread>(
        thread_create_destroy_command_list_valid_flags, device, flags);
  }

  for (uint32_t i = 0; i < num_threads; i++) {
    threads[i]->join();
  }
}

INSTANTIATE_TEST_CASE_P(
    TestCasesforCommandListCreateCloseAndDestroyWithValidFlagValues,
    zeCommandListCreateDestroyValidFlagsMultithreadTest,
    testing::Values(ZE_COMMAND_LIST_FLAG_RELAXED_ORDERING,
                    ZE_COMMAND_LIST_FLAG_MAXIMIZE_THROUGHPUT,
                    ZE_COMMAND_LIST_FLAG_EXPLICIT_ONLY));

} // namespace
