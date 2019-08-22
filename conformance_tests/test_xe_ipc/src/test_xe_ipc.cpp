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

#include "xe_driver.h"
#include "xe_memory.h"

namespace cs = compute_samples;

namespace {

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
    ipc_mem_handle_ = (xe_ipc_mem_handle_t)malloc(XE_MAX_IPC_HANDLE_SIZE);
    size_t bytes = boost::asio::read(
        sock, boost::asio::buffer(ipc_mem_handle_, XE_MAX_IPC_HANDLE_SIZE),
        error);

    if (error || bytes < XE_MAX_IPC_HANDLE_SIZE) {
      FAIL() << "Failed to retrieve ipc handle";
    }
  }

  void TearDown() override {
    EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceGroupCloseMemIpcHandle(
                                     cs::get_default_device_group(), memory_));
    cs::free_memory(memory_);
    free(ipc_mem_handle_);
  }

  void *memory_ = nullptr;
  xe_ipc_mem_handle_t ipc_mem_handle_ = nullptr;
};

TEST_F(xeIpcMemHandleOpenTests,
       GivenValidIpcMemoryHandleWhenOpeningIpcMemHandleThenSuccessIsReturned) {

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceGroupOpenMemIpcHandle(
                cs::get_default_device_group(),
                cs::xeDevice::get_instance()->get_device(), ipc_mem_handle_,
                XE_IPC_MEMORY_FLAG_NONE, &memory_));
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
