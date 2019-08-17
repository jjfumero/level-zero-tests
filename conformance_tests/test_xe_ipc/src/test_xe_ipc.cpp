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

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <chrono>
#include <thread>

#include "gtest/gtest.h"

#include "xe_utils/xe_utils.hpp"
#include "xe_test_harness/xe_test_harness.hpp"
#include "logging/logging.hpp"

namespace cs = compute_samples;

#include "xe_driver.h"
#include "xe_memory.h"

namespace {

// Workaround for using ipc memory handles
// Should be removed when
// https://gitlab.devtools.intel.com/one-api/level_zero/issues/198
// is addressed
struct _xe_ipc_mem_handle_t {
  size_t alignment;
  size_t size;
  char shmFileName[255];
};

class xeIpcMemHandleTests : public ::testing::Test {
protected:
  void *memory_ = nullptr;
  xe_ipc_mem_handle_t ipc_mem_handle_;
};

TEST_F(
    xeIpcMemHandleTests,
    GivenDeviceMemoryAllocationWhenGettingIpcMemHandleThenSuccessIsReturned) {
  memory_ = cs::allocate_device_memory(1);

  const xe_device_group_handle_t device_group = cs::get_default_device_group();
  EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupGetMemIpcHandle(
                                   device_group, memory_, &ipc_mem_handle_));

  cs::free_memory(memory_);
}

#ifdef __linux__ // Disabled on Windows until fixed in VLCLJ-285
TEST_F(xeIpcMemHandleTests,
       GivenValidIpcMemoryHandleWhenOpeningIpcMemHandleThenSuccessIsReturned) {
  namespace bi = boost::interprocess;

  bi::shared_memory_object::remove("SharedMem");

  int pid = fork();

  if (pid < 0) {
    throw std::runtime_error("Failed to fork child process");
  } else if (0 == pid) { // Create the IPC handle in a child process

    int status = 0;
    bi::managed_shared_memory shm(bi::create_only, "SharedMem", 65536);

    struct _xe_ipc_mem_handle_t *ipc_handle_ =
        (shm.construct<struct _xe_ipc_mem_handle_t>("MemHandle")());
    xe_ipc_mem_handle_t temp_handle;
    cs::allocate_mem_and_get_ipc_handle(&temp_handle, &memory_,
                                        XE_MEMORY_TYPE_DEVICE);

    memcpy(ipc_handle_, temp_handle, sizeof(struct _xe_ipc_mem_handle_t));

    if (XE_RESULT_SUCCESS != xeDeviceGroupOpenMemIpcHandle(
                                 cs::get_default_device_group(),
                                 cs::xeDevice::get_instance()->get_device(),
                                 (xe_ipc_mem_handle_t)ipc_handle_,
                                 XE_IPC_MEMORY_FLAG_NONE, &memory_)) {
      status = 1;
    }

    // free memory after parent process is done
    std::chrono::milliseconds ms(500);
    std::this_thread::sleep_for(ms);

    cs::free_memory(memory_);
    bi::shared_memory_object::remove("SharedMem");

    // Don't produce test result for child process
    exit(status);
  } else {
    // wait for shared memory object to be created
    std::chrono::milliseconds ms(100);
    std::this_thread::sleep_for(ms);
    bi::managed_shared_memory shm(bi::open_only, "SharedMem");

    std::pair<struct _xe_ipc_mem_handle_t *,
              bi::managed_shared_memory::size_type>
        res;
    res = shm.find<struct _xe_ipc_mem_handle_t>("MemHandle");

    struct _xe_ipc_mem_handle_t *ipc_mem_handle = res.first;

    ipc_mem_handle_ = reinterpret_cast<xe_ipc_mem_handle_t>(ipc_mem_handle);
    EXPECT_EQ(XE_RESULT_SUCCESS,
              xeDeviceGroupOpenMemIpcHandle(
                  cs::get_default_device_group(),
                  cs::xeDevice::get_instance()->get_device(), ipc_mem_handle_,
                  XE_IPC_MEMORY_FLAG_NONE, &memory_));

    EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupCloseMemIpcHandle(
                                     cs::get_default_device_group(), memory_));

    int status;
    if (waitpid(pid, &status, 0) < 0) {
      ADD_FAILURE() << "Error waiting for ipc test child process";
    } else {
      if (WIFEXITED(status)) {
        status = WEXITSTATUS(status);
        EXPECT_EQ(0, status)
            << "ipc test child process exited with error: " << status;
      } else {
        ADD_FAILURE() << "ipc test child process exited abnormally";
      }
    }
  }
}
#endif

class xeIpcMemHandleCloseTests : public xeIpcMemHandleTests {
protected:
  void SetUp() override {
    cs::allocate_mem_and_get_ipc_handle(&ipc_mem_handle_, &memory_,
                                        XE_MEMORY_TYPE_DEVICE);

    xe_ipc_memory_flag_t flags = XE_IPC_MEMORY_FLAG_NONE;
    EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupOpenMemIpcHandle(
                                     cs::get_default_device_group(),
                                     cs::xeDevice::get_instance()->get_device(),
                                     ipc_mem_handle_, flags, &memory_));
  }

  void TearDown() { cs::free_memory(memory_); }
};

TEST_F(
    xeIpcMemHandleCloseTests,
    GivenValidPointerToDeviceMemoryAllocationWhenClosingIpcHandleThenSuccessIsReturned) {
  EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupCloseMemIpcHandle(
                                   cs::get_default_device_group(), memory_));
}

} // namespace
