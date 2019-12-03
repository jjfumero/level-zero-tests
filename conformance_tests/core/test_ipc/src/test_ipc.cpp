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

#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <boost/process.hpp>
namespace bp = boost::process;

#include "gtest/gtest.h"
#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"
#include "logging/logging.hpp"
#include "test_ipc_comm.hpp"

#include "ze_api.h"

namespace lzt = level_zero_tests;

namespace {

class xeIpcMemHandleTests : public ::testing::Test {
protected:
  void *memory_ = nullptr;
  ze_ipc_mem_handle_t ipc_mem_handle_;
};

TEST_F(
    xeIpcMemHandleTests,
    GivenDeviceMemoryAllocationWhenGettingIpcMemHandleThenSuccessIsReturned) {
  memory_ = lzt::allocate_device_memory(1);

  const ze_driver_handle_t driver = lzt::get_default_driver();
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverGetMemIpcHandle(driver, memory_, &ipc_mem_handle_));

  lzt::free_memory(memory_);
}

class xeIpcMemHandleCloseTests : public xeIpcMemHandleTests {
protected:
  void SetUp() override {
    lzt::allocate_mem_and_get_ipc_handle(&ipc_mem_handle_, &memory_,
                                         ZE_MEMORY_TYPE_DEVICE);

    ze_ipc_memory_flag_t flags = ZE_IPC_MEMORY_FLAG_NONE;
    EXPECT_EQ(
        ZE_RESULT_SUCCESS,
        zeDriverOpenMemIpcHandle(lzt::get_default_driver(),
                                 lzt::zeDevice::get_instance()->get_device(),
                                 ipc_mem_handle_, flags, &memory_));
  }

  void TearDown() { lzt::free_memory(memory_); }
};

TEST_F(
    xeIpcMemHandleCloseTests,
    GivenValidPointerToDeviceMemoryAllocationWhenClosingIpcHandleThenSuccessIsReturned) {
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverCloseMemIpcHandle(lzt::get_default_driver(), memory_));
}

class xeIpcMemAccessTests
    : public ::testing::TestWithParam<::testing::tuple<
          ze_memory_type_t /*source_type*/, size_t /*size*/,
          int /*data_pattern*/, ze_memory_type_t /*destination_type*/>> {};

TEST_P(
    xeIpcMemAccessTests,
    GivenTwoProcessesUsingWriteRndvProtocolWithIpcMemoryHandleMessageIsCorrect) {
  ze_memory_type_t source_type;
  size_t size;
  int data_pattern;
  ze_memory_type_t destination_type;
  std::tie(source_type, size, data_pattern, destination_type) = GetParam();
  lzt::ipc_test_parameters test_parameters;
  test_parameters.source_type = source_type;
  test_parameters.size = size;
  test_parameters.data_pattern = data_pattern;
  test_parameters.destination_type = destination_type;
  lzt::write_rndv_comm_context ctx;

  init_comm(ctx);
  fs::path p = bp::search_path("./ipc/test_ipc_helper");
  if (p.empty()) {
    FAIL() << "Cannot find test_ipc_helper on PATH";
  } else {
    bp::child c(p);
    run_sender_sockets(test_parameters);
    run_receiver(test_parameters, ctx);
    c.wait();
  }
}

INSTANTIATE_TEST_CASE_P(IpcMemoryAccess, xeIpcMemAccessTests,
                        ::testing::Combine(
                            // Note that the only memory type supported by IPC
                            // memory is device memory:
                            testing::Values(ZE_MEMORY_TYPE_DEVICE),
                            testing::Values(1024, 2048, 4096, 8192),
                            testing::Values(0x55, 0xAA),
                            // Note that the only memory type supported by IPC
                            // memory is device memory:
                            testing::Values(ZE_MEMORY_TYPE_DEVICE)));

} // namespace
