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

#ifndef level_zero_tests_ZE_TEST_HARNESS_CMDLIST_HPP
#define level_zero_tests_ZE_TEST_HARNESS_CMDLIST_HPP

#include "test_harness_device.hpp"
#include "ze_api.h"
#include "gtest/gtest.h"

namespace level_zero_tests {

class zeCommandList {
public:
  zeCommandList();
  ~zeCommandList();

  ze_command_list_handle_t command_list_ = nullptr;
};

class zeCommandListTests : public ::testing::Test {
protected:
  zeCommandList cl;
};

ze_command_list_handle_t create_command_list(ze_device_handle_t device,
                                             ze_command_list_flag_t flags);
ze_command_list_handle_t create_command_list(ze_device_handle_t device);
ze_command_list_handle_t create_command_list();

ze_command_list_handle_t create_immediate_command_list(
    ze_device_handle_t device, ze_command_queue_flag_t flags,
    ze_command_queue_mode_t, ze_command_queue_priority_t priority,
    uint32_t ordinal);
ze_command_list_handle_t
create_immediate_command_list(ze_device_handle_t device);
ze_command_list_handle_t create_immediate_command_list();

void append_memory_set(ze_command_list_handle_t cl, void *dstptr, uint8_t value,
                       size_t size);
void append_memory_set(ze_command_list_handle_t cl, void *dstptr, uint8_t value,
                       size_t size, ze_event_handle_t hSignalEvent);

void append_memory_copy(ze_command_list_handle_t cl, void *dstptr,
                        const void *srcptr, size_t size,
                        ze_event_handle_t hSignalEvent);
void append_memory_copy_region(ze_command_list_handle_t hCommandList,
                               void *dstptr, const ze_copy_region_t *dstRegion,
                               uint32_t dstPitch, const void *srcptr,
                               const ze_copy_region_t *srcRegion,
                               uint32_t srcPitch,
                               ze_event_handle_t hSignalEvent);
void append_barrier(ze_command_list_handle_t cl, ze_event_handle_t hSignalEvent,
                    uint32_t numWaitEvents, ze_event_handle_t *phWaitEvents);
void append_memory_ranges_barrier(ze_command_list_handle_t hCommandList,
                                  uint32_t numRanges, const size_t *pRangeSizes,
                                  const void **pRanges,
                                  ze_event_handle_t hSignalEvent,
                                  uint32_t numWaitEvents,
                                  ze_event_handle_t *phWaitEvents);
void append_launch_function(ze_command_list_handle_t hCommandList,
                            ze_kernel_handle_t hFunction,
                            const ze_thread_group_dimensions_t *pLaunchFuncArgs,
                            ze_event_handle_t hSignalEvent,
                            uint32_t numWaitEvents,
                            ze_event_handle_t *phWaitEvents);
void append_signal_event(ze_command_list_handle_t hCommandList,
                         ze_event_handle_t hEvent);
void append_wait_on_events(ze_command_list_handle_t hCommandList,
                           uint32_t numEvents, ze_event_handle_t *phEvents);
void append_reset_event(ze_command_list_handle_t hCommandList,
                        ze_event_handle_t hEvent);

void append_image_copy(ze_command_list_handle_t hCommandList,
                       ze_image_handle_t dst, ze_image_handle_t src,
                       ze_event_handle_t hEvent);
void append_image_copy_to_mem(ze_command_list_handle_t hCommandList, void *dst,
                              ze_image_handle_t src, ze_event_handle_t hEvent);
void append_image_copy_from_mem(ze_command_list_handle_t hCommandList,
                                ze_image_handle_t dst, void *src,
                                ze_event_handle_t hEvent);
void append_image_copy_region(ze_command_list_handle_t hCommandList,
                              ze_image_handle_t dst, ze_image_handle_t src,
                              ze_image_region_t *dst_region,
                              ze_image_region_t *src_region,
                              ze_event_handle_t hEvent);

void close_command_list(ze_command_list_handle_t cl);
void reset_command_list(ze_command_list_handle_t cl);
void destroy_command_list(ze_command_list_handle_t cl);

}; // namespace level_zero_tests
#endif
