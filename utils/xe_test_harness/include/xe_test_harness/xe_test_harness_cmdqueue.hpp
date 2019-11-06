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

#ifndef level_zero_tests_ZE_TEST_HARNESS_CMDQUEUE_HPP
#define level_zero_tests_ZE_TEST_HARNESS_CMDQUEUE_HPP

#include "xe_test_harness/xe_test_harness_device.hpp"
#include "ze_api.h"
#include "gtest/gtest.h"

namespace level_zero_tests {

class zeCommandQueue {
public:
  zeCommandQueue();
  ~zeCommandQueue();

  ze_command_queue_handle_t command_queue_ = nullptr;
};

class zeCommandQueueTests : public ::testing::Test {
protected:
  zeCommandQueue cq;
};

ze_command_queue_handle_t create_command_queue();
ze_command_queue_handle_t create_command_queue(ze_device_handle_t device);
ze_command_queue_handle_t create_command_queue(ze_command_queue_mode_t mode);
ze_command_queue_handle_t
create_command_queue(ze_device_handle_t device, ze_command_queue_flag_t flags,
                     ze_command_queue_mode_t mode,
                     ze_command_queue_priority_t priority, uint32_t ordinal);

void execute_command_lists(ze_command_queue_handle_t cq,
                           uint32_t numCommandLists,
                           ze_command_list_handle_t *phCommandLists,
                           ze_fence_handle_t hFence);
void synchronize(ze_command_queue_handle_t cq, uint32_t timeout);

void destroy_command_queue(ze_command_queue_handle_t cq);

}; // namespace level_zero_tests
#endif
