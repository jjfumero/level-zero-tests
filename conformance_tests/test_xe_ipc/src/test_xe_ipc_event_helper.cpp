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

#include "xe_utils/xe_utils.hpp"
#include "xe_test_harness/xe_test_harness.hpp"
#include "logging/logging.hpp"
#include "test_xe_ipc_event.hpp"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/process.hpp>

#include "ze_api.h"

namespace lzt = level_zero_tests;
namespace bipc = boost::interprocess;

static const ze_event_desc_t defaultEventDesc = {
    ZE_EVENT_DESC_VERSION_CURRENT, 5, ZE_EVENT_SCOPE_FLAG_NONE,
    ZE_EVENT_SCOPE_FLAG_HOST, // ensure memory coherency across device and Host
                              // after event signalled
};

ze_event_pool_desc_t defaultEventPoolDesc = {
    ZE_EVENT_POOL_DESC_VERSION_CURRENT,
    (ze_event_pool_flag_t)(ZE_EVENT_POOL_FLAG_HOST_VISIBLE |
                           ZE_EVENT_POOL_FLAG_IPC),
    10};

static void child_host_reads(ze_event_pool_handle_t hEventPool) {
  ze_event_handle_t hEvent;
  zeEventCreate(hEventPool, &defaultEventDesc, &hEvent);
  zeEventHostSynchronize(hEvent, UINT32_MAX);
  // cleanup
  zeEventDestroy(hEvent);
}

static void child_device_reads(ze_event_pool_handle_t hEventPool) {
  ze_event_handle_t hEvent;
  zeEventCreate(hEventPool, &defaultEventDesc, &hEvent);

  auto cmdlist = lzt::create_command_list();
  auto cmdqueue = lzt::create_command_queue();
  lzt::append_wait_on_events(cmdlist, 1, &hEvent);
  lzt::execute_command_lists(cmdqueue, 1, &cmdlist, nullptr);
  lzt::synchronize(cmdqueue, UINT32_MAX);

  // cleanup
  lzt::destroy_command_list(cmdlist);
  lzt::destroy_command_queue(cmdqueue);
  zeEventDestroy(hEvent);
}

static void child_device2_reads(ze_event_pool_handle_t hEventPool) {
  ze_event_handle_t hEvent;
  zeEventCreate(hEventPool, &defaultEventDesc, &hEvent);
  auto devices = lzt::get_ze_devices();
  auto cmdlist = lzt::create_command_list(devices[1]);
  auto cmdqueue = lzt::create_command_queue(devices[1]);
  lzt::append_wait_on_events(cmdlist, 1, &hEvent);
  lzt::execute_command_lists(cmdqueue, 1, &cmdlist, nullptr);
  lzt::synchronize(cmdqueue, UINT32_MAX);

  // cleanup
  lzt::destroy_command_list(cmdlist);
  lzt::destroy_command_queue(cmdqueue);
  zeEventDestroy(hEvent);
}
static void child_multi_device_reads(ze_event_pool_handle_t hEventPool) {
  ze_event_handle_t hEvent;
  zeEventCreate(hEventPool, &defaultEventDesc, &hEvent);
  auto devices = lzt::get_ze_devices();
  auto cmdlist1 = lzt::create_command_list(devices[0]);
  auto cmdqueue1 = lzt::create_command_queue(devices[0]);
  lzt::append_wait_on_events(cmdlist1, 1, &hEvent);
  lzt::execute_command_lists(cmdqueue1, 1, &cmdlist1, nullptr);
  lzt::synchronize(cmdqueue1, UINT32_MAX);

  auto cmdlist2 = lzt::create_command_list(devices[1]);
  auto cmdqueue2 = lzt::create_command_queue(devices[1]);
  lzt::append_wait_on_events(cmdlist2, 1, &hEvent);
  lzt::execute_command_lists(cmdqueue2, 1, &cmdlist2, nullptr);
  lzt::synchronize(cmdqueue2, UINT32_MAX);
  // cleanup
  lzt::destroy_command_list(cmdlist1);
  lzt::destroy_command_queue(cmdqueue1);
  lzt::destroy_command_list(cmdlist2);
  lzt::destroy_command_queue(cmdqueue2);
  zeEventDestroy(hEvent);
}

int main() {

  ze_result_t result;
  if (zeInit(ZE_INIT_FLAG_NONE) != ZE_RESULT_SUCCESS)
    exit(1);

  shared_data_t shared_data;
  bipc::shared_memory_object shm(bipc::open_only, "ipc_event_test",
                                 bipc::read_write);
  shm.truncate(sizeof(shared_data_t));
  bipc::mapped_region region(shm, bipc::read_only);
  std::memcpy(&shared_data, region.get_address(), sizeof(shared_data_t));
  ze_event_pool_handle_t hEventPool = 0;
  lzt::open_ipc_event_handle(shared_data.hIpcEventPool, &hEventPool);

  if (!hEventPool)
    exit(1);
  switch (shared_data.child_type) {
  CHILD_TEST_HOST_READS:
    child_host_reads(hEventPool);
    break;
  CHILD_TEST_DEVICE_READS:
    child_device_reads(hEventPool);
    break;
  CHILD_TEST_DEVICE2_READS:
    child_device2_reads(hEventPool);
    break;
  CHILD_TEST_MULTI_DEVICE_READS:
    child_multi_device_reads(hEventPool);
    break;
  default:
    exit(1);
  }
  lzt::close_ipc_event_handle(hEventPool);
  exit(0);
}
