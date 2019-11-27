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

#include "test_harness/test_harness.hpp"
#include "gtest/gtest.h"
#include "ze_api.h"

namespace lzt = level_zero_tests;

namespace level_zero_tests {

ze_command_list_handle_t create_command_list() {
  return create_command_list(zeDevice::get_instance()->get_device());
}
ze_command_list_handle_t create_command_list(ze_device_handle_t device) {
  return create_command_list(device, ZE_COMMAND_LIST_FLAG_NONE);
}

ze_command_list_handle_t create_command_list(ze_device_handle_t device,
                                             ze_command_list_flag_t flags) {
  ze_command_list_desc_t descriptor;
  descriptor.version = ZE_COMMAND_LIST_DESC_VERSION_CURRENT;
  descriptor.flags = flags;
  ze_command_list_handle_t command_list = nullptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListCreate(device, &descriptor, &command_list));
  EXPECT_NE(nullptr, command_list);

  return command_list;
}

ze_command_list_handle_t create_immediate_command_list() {
  return create_immediate_command_list(zeDevice::get_instance()->get_device());
}

ze_command_list_handle_t
create_immediate_command_list(ze_device_handle_t device) {
  return create_immediate_command_list(device, ZE_COMMAND_QUEUE_FLAG_NONE,
                                       ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                       ZE_COMMAND_QUEUE_PRIORITY_NORMAL, 0);
}

ze_command_list_handle_t create_immediate_command_list(
    ze_device_handle_t device, ze_command_queue_flag_t flags,
    ze_command_queue_mode_t mode, ze_command_queue_priority_t priority,
    uint32_t ordinal) {
  ze_command_queue_desc_t descriptor;
  descriptor.version = ZE_COMMAND_QUEUE_DESC_VERSION_CURRENT;
  descriptor.flags = flags;
  descriptor.mode = mode;
  descriptor.priority = priority;
  descriptor.ordinal = ordinal;
  ze_command_list_handle_t command_list = nullptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListCreateImmediate(device, &descriptor, &command_list));
  EXPECT_NE(nullptr, command_list);
  return command_list;
}

zeCommandList::zeCommandList() { command_list_ = create_command_list(); }

zeCommandList::~zeCommandList() {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListDestroy(command_list_));
}

void append_memory_set(ze_command_list_handle_t cl, void *dstptr, uint8_t value,
                       size_t size) {
  append_memory_set(cl, dstptr, value, size, nullptr);
}

void append_memory_set(ze_command_list_handle_t cl, void *dstptr, uint8_t value,
                       size_t size, ze_event_handle_t hSignalEvent) {
  append_memory_fill(cl, dstptr, &value, 1, size, hSignalEvent);
}

void append_memory_fill(ze_command_list_handle_t cl, void *dstptr,
                        const void *pattern, size_t pattern_size, size_t size,
                        ze_event_handle_t hSignalEvent) {
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendMemoryFill(cl, dstptr, pattern, pattern_size,
                                          size, hSignalEvent));
}

void append_memory_copy(ze_command_list_handle_t cl, void *dstptr,
                        const void *srcptr, size_t size,
                        ze_event_handle_t hSignalEvent) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendMemoryCopy(
                                   cl, dstptr, srcptr, size, hSignalEvent));
}

void append_memory_copy(ze_command_list_handle_t cl, void *dstptr,
                        const void *srcptr, size_t size) {
  append_memory_copy(cl, dstptr, srcptr, size, nullptr);
}

void append_memory_copy_region(ze_command_list_handle_t hCommandList,
                               void *dstptr, const ze_copy_region_t *dstRegion,
                               uint32_t dstPitch, const void *srcptr,
                               const ze_copy_region_t *srcRegion,
                               uint32_t srcPitch,
                               ze_event_handle_t hSignalEvent) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendMemoryCopyRegion(
                                   hCommandList, dstptr, dstRegion, dstPitch,
                                   srcptr, srcRegion, srcPitch, hSignalEvent));
}

void append_barrier(ze_command_list_handle_t cl, ze_event_handle_t hSignalEvent,
                    uint32_t numWaitEvents, ze_event_handle_t *phWaitEvents) {
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendBarrier(cl, hSignalEvent, numWaitEvents,
                                       phWaitEvents));
}

void append_barrier(ze_command_list_handle_t cl,
                    ze_event_handle_t hSignalEvent) {
  append_barrier(cl, hSignalEvent, 0, nullptr);
}

void append_barrier(ze_command_list_handle_t cl) {
  append_barrier(cl, nullptr, 0, nullptr);
}

void append_memory_ranges_barrier(ze_command_list_handle_t hCommandList,
                                  uint32_t numRanges, const size_t *pRangeSizes,
                                  const void **pRanges,
                                  ze_event_handle_t hSignalEvent,
                                  uint32_t numWaitEvents,
                                  ze_event_handle_t *phWaitEvents) {
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendMemoryRangesBarrier(
                hCommandList, numRanges, pRangeSizes, pRanges, hSignalEvent,
                numWaitEvents, phWaitEvents));
}

void append_launch_function(ze_command_list_handle_t hCommandList,
                            ze_kernel_handle_t hFunction,
                            const ze_thread_group_dimensions_t *pLaunchFuncArgs,
                            ze_event_handle_t hSignalEvent,
                            uint32_t numWaitEvents,
                            ze_event_handle_t *phWaitEvents) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendLaunchKernel(
                                   hCommandList, hFunction, pLaunchFuncArgs,
                                   hSignalEvent, numWaitEvents, phWaitEvents));
}

void append_signal_event(ze_command_list_handle_t hCommandList,
                         ze_event_handle_t hEvent) {
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendSignalEvent(hCommandList, hEvent));
}

void append_wait_on_events(ze_command_list_handle_t hCommandList,
                           uint32_t numEvents, ze_event_handle_t *phEvents) {
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendWaitOnEvents(hCommandList, numEvents, phEvents));
}

void append_reset_event(ze_command_list_handle_t hCommandList,
                        ze_event_handle_t hEvent) {
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendEventReset(hCommandList, hEvent));
}

void append_image_copy(ze_command_list_handle_t hCommandList,
                       ze_image_handle_t dst, ze_image_handle_t src,
                       ze_event_handle_t hEvent) {

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendImageCopy(hCommandList, dst, src, hEvent));
}

void append_image_copy_to_mem(ze_command_list_handle_t hCommandList, void *dst,
                              ze_image_handle_t src, ze_event_handle_t hEvent) {

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyToMemory(
                                   hCommandList, dst, src, nullptr, hEvent));
}

void append_image_copy_from_mem(ze_command_list_handle_t hCommandList,
                                ze_image_handle_t dst, void *src,
                                ze_event_handle_t hEvent) {

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyFromMemory(
                                   hCommandList, dst, src, nullptr, hEvent));
}

void append_image_copy_region(ze_command_list_handle_t hCommandList,
                              ze_image_handle_t dst, ze_image_handle_t src,
                              ze_image_region_t *dst_region,
                              ze_image_region_t *src_region,
                              ze_event_handle_t hEvent) {
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendImageCopyRegion(hCommandList, dst, src,
                                               dst_region, src_region, hEvent));
}

void close_command_list(ze_command_list_handle_t cl) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(cl));
}

void reset_command_list(ze_command_list_handle_t cl) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListReset(cl));
}

void destroy_command_list(ze_command_list_handle_t cl) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListDestroy(cl));
}
}; // namespace level_zero_tests
