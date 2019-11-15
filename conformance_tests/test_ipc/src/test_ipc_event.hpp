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

#ifndef TEST_IPC_EVENT_HPP
#define TEST_IPC_EVENT_HPP

typedef enum {
  PARENT_TEST_HOST_SIGNALS,
  PARENT_TEST_DEVICE_SIGNALS
} parent_test_t;

typedef enum {
  CHILD_TEST_HOST_READS,
  CHILD_TEST_DEVICE_READS,
  CHILD_TEST_DEVICE2_READS,
  CHILD_TEST_MULTI_DEVICE_READS
} child_test_t;

typedef struct {
  parent_test_t parent_type;
  child_test_t child_type;
  ze_ipc_event_pool_handle_t hIpcEventPool;
  bool multi_device;
} shared_data_t;

#endif