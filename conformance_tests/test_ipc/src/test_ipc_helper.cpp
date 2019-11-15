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
#include <iostream>

#include "logging/logging.hpp"
#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"

int main() {
  ze_result_t result = zeInit(ZE_INIT_FLAG_NONE);
  if (result) {
    throw std::runtime_error("zeInit failed: " +
                             level_zero_tests::to_string(result));
  }
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::socket sock{io_service};

  ze_ipc_mem_handle_t ipc_mem_handle;

  void *memory_ = nullptr;
  lzt::allocate_mem_and_get_ipc_handle(&ipc_mem_handle, &memory_,
                                       ZE_MEMORY_TYPE_DEVICE);

  sock.connect(boost::asio::ip::tcp::endpoint(
      boost::asio::ip::address::from_string("127.0.0.1"), 65432));

  boost::system::error_code error;
  auto buffer = boost::asio::buffer(&ipc_mem_handle, sizeof(ipc_mem_handle));
  size_t bytes = boost::asio::write(sock, buffer, error);

  if (error || (bytes < sizeof(ipc_mem_handle))) {
    LOG_INFO << "Failed to send ipc handle";
  }
  sock.close();

  return 0;
}
