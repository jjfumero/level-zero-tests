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

#ifndef level_zero_tests_XE_TEST_HARNESS_CMDLIST_HPP
#define level_zero_tests_XE_TEST_HARNESS_CMDLIST_HPP

#include "xe_test_harness_device.hpp"
#include "xe_cmdlist.h"
#include "gtest/gtest.h"

namespace level_zero_tests {

class xeCommandList {
public:
  xeCommandList();
  ~xeCommandList();

  xe_command_list_handle_t command_list_ = nullptr;
};

class xeCommandListTests : public ::testing::Test {
protected:
  xeCommandList cl;
};

xe_command_list_handle_t create_command_list(xe_device_handle_t device);
xe_command_list_handle_t create_command_list();

void append_memory_set(xe_command_list_handle_t cl, void *dstptr, uint8_t value,
                       size_t size);
void append_memory_set(xe_command_list_handle_t cl, void *dstptr, uint8_t value,
                       size_t size, xe_event_handle_t hSignalEvent);
void append_memory_set(xe_command_list_handle_t cl, void *dstptr, uint8_t value,
                       size_t size, xe_event_handle_t hSignalEvent,
                       uint32_t numWaitEvents, xe_event_handle_t *phWaitEvents);

void append_memory_copy(xe_command_list_handle_t cl, void *dstptr,
                        const void *srcptr, size_t size,
                        xe_event_handle_t hSignalEvent, uint32_t numWaitEvents,
                        xe_event_handle_t *phWaitEvents);
void append_memory_copy_region(xe_command_list_handle_t hCommandList,
                               void *dstptr, const xe_copy_region_t *dstRegion,
                               uint32_t dstPitch, const void *srcptr,
                               const xe_copy_region_t *srcRegion,
                               uint32_t srcPitch,
                               xe_event_handle_t hSignalEvent,
                               uint32_t numWaitEvents,
                               xe_event_handle_t *phWaitEvents);
void append_barrier(xe_command_list_handle_t cl, xe_event_handle_t hSignalEvent,
                    uint32_t numWaitEvents, xe_event_handle_t *phWaitEvents);
void close_command_list(xe_command_list_handle_t cl);
void destroy_command_list(xe_command_list_handle_t cl);

}; // namespace level_zero_tests
#endif
