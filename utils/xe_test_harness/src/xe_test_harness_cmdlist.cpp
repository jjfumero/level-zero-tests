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
#include "xe_copy.h"
#include "xe_barrier.h"

namespace lzt = level_zero_tests;

namespace level_zero_tests {

xe_command_list_handle_t create_command_list() {
  return create_command_list(xeDevice::get_instance()->get_device());
}
xe_command_list_handle_t create_command_list(xe_device_handle_t device) {
  xe_command_list_desc_t descriptor;
  descriptor.version = XE_COMMAND_LIST_DESC_VERSION_CURRENT;

  xe_command_list_handle_t command_list = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListCreate(device, &descriptor, &command_list));
  EXPECT_NE(nullptr, command_list);

  return command_list;
}

xeCommandList::xeCommandList() { command_list_ = create_command_list(); }

xeCommandList::~xeCommandList() {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListDestroy(command_list_));
}

void append_memory_set(xe_command_list_handle_t cl, void *dstptr, uint8_t value,
                       size_t size) {
  append_memory_set(cl, dstptr, value, size, nullptr, 0, nullptr);
}

void append_memory_set(xe_command_list_handle_t cl, void *dstptr, uint8_t value,
                       size_t size, xe_event_handle_t hSignalEvent) {
  append_memory_set(cl, dstptr, value, size, hSignalEvent, 0, nullptr);
}

void append_memory_set(xe_command_list_handle_t cl, void *dstptr, uint8_t value,
                       size_t size, xe_event_handle_t hSignalEvent,
                       uint32_t numWaitEvents,
                       xe_event_handle_t *phWaitEvents) {
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemorySet(cl, dstptr, value, size, hSignalEvent,
                                         numWaitEvents, phWaitEvents));
}

void append_memory_copy(xe_command_list_handle_t cl, void *dstptr,
                        const void *srcptr, size_t size,
                        xe_event_handle_t hSignalEvent, uint32_t numWaitEvents,
                        xe_event_handle_t *phWaitEvents) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendMemoryCopy(
                                   cl, dstptr, srcptr, size, hSignalEvent,
                                   numWaitEvents, phWaitEvents));
}

void append_memory_copy_region(xe_command_list_handle_t hCommandList,
                               void *dstptr, const xe_copy_region_t *dstRegion,
                               uint32_t dstPitch, const void *srcptr,
                               const xe_copy_region_t *srcRegion,
                               uint32_t srcPitch,
                               xe_event_handle_t hSignalEvent,
                               uint32_t numWaitEvents,
                               xe_event_handle_t *phWaitEvents) {
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemoryCopyRegion(
                hCommandList, dstptr, dstRegion, dstPitch, srcptr, srcRegion,
                srcPitch, hSignalEvent, numWaitEvents, phWaitEvents));
}

void append_barrier(xe_command_list_handle_t cl, xe_event_handle_t hSignalEvent,
                    uint32_t numWaitEvents, xe_event_handle_t *phWaitEvents) {
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendBarrier(cl, hSignalEvent, numWaitEvents,
                                       phWaitEvents));
}

void append_memory_ranges_barrier(xe_command_list_handle_t hCommandList,
                                  uint32_t numRanges, const size_t *pRangeSizes,
                                  const void **pRanges,
                                  xe_event_handle_t hSignalEvent,
                                  uint32_t numWaitEvents,
                                  xe_event_handle_t *phWaitEvents) {
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemoryRangesBarrier(
                hCommandList, numRanges, pRangeSizes, pRanges, hSignalEvent,
                numWaitEvents, phWaitEvents));
}

void append_launch_function(xe_command_list_handle_t hCommandList,
                            xe_function_handle_t hFunction,
                            const xe_thread_group_dimensions_t *pLaunchFuncArgs,
                            xe_event_handle_t hSignalEvent,
                            uint32_t numWaitEvents,
                            xe_event_handle_t *phWaitEvents) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendLaunchFunction(
                                   hCommandList, hFunction, pLaunchFuncArgs,
                                   hSignalEvent, numWaitEvents, phWaitEvents));
}

void append_signal_event(xe_command_list_handle_t hCommandList,
                         xe_event_handle_t hEvent) {
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendSignalEvent(hCommandList, hEvent));
}

void close_command_list(xe_command_list_handle_t cl) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListClose(cl));
}

void destroy_command_list(xe_command_list_handle_t cl) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListDestroy(cl));
}
}; // namespace level_zero_tests
