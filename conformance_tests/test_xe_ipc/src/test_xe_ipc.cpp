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
#ifdef __linux__
#include <boost/asio.hpp>
#include <boost/process.hpp>
#endif

#include "gtest/gtest.h"
#include "xe_utils/xe_utils.hpp"
#include "xe_test_harness/xe_test_harness.hpp"
#include "logging/logging.hpp"

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

#ifdef __linux__
class xeIpcMemHandleOpenTests : public ::testing::Test {
protected:
  void SetUp() override {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket sock(io_service);

    boost::asio::ip::tcp::acceptor acceptor(
        io_service,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 65432));

    // get a handle from another process
    boost::process::system("./test_xe_ipc_helper");
    acceptor.accept(sock);

    boost::system::error_code error;
    size_t bytes = boost::asio::read(
        sock, boost::asio::buffer(&ipc_mem_handle_, sizeof(ipc_mem_handle_)),
        error);

    if (error || bytes < sizeof(ipc_mem_handle_)) {
      FAIL() << "Failed to retrieve ipc handle";
    }
  }

  void TearDown() override {
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeDriverCloseMemIpcHandle(lzt::get_default_driver(), memory_));
    lzt::free_memory(memory_);
  }

  void *memory_ = nullptr;
  ze_ipc_mem_handle_t ipc_mem_handle_;
};

TEST_F(xeIpcMemHandleOpenTests,
       GivenValidIpcMemoryHandleWhenOpeningIpcMemHandleThenSuccessIsReturned) {

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeDriverOpenMemIpcHandle(
                lzt::get_default_driver(),
                lzt::zeDevice::get_instance()->get_device(), ipc_mem_handle_,
                ZE_IPC_MEMORY_FLAG_NONE, &memory_));
}
#endif

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

} // namespace
