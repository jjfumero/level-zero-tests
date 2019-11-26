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

#include "test_ipc_comm.hpp"
#include "utils/utils.hpp"
#include "logging/logging.hpp"
#include "gtest/gtest.h"

namespace level_zero_tests {

void run_sender_sockets(const ipc_test_parameters &parms) {
  RndvPacket rp;
  boost::asio::io_service io_service;
  boost_ip::tcp::socket comm_socket(io_service);
  boost_ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"),
                             65433);

  write_rndv_sockets_connect(comm_socket, ep);
  rp.opcode = RSO_RTS_SOCKETS;
  rp.payload.parms = parms;
  if (write_to_socket(comm_socket, reinterpret_cast<uint8_t *>(&rp),
                      sizeof(rp)) != sizeof(rp)) {
    throw std::runtime_error("Client: writing RTS message");
  }
  if (read_from_socket(comm_socket, reinterpret_cast<uint8_t *>(&rp),
                       sizeof(rp)) != sizeof(rp)) {
    throw std::runtime_error("Client: reading stream message");
  }
  if (rp.opcode != RSO_ACK_SOCKETS) {
    unexpected_opcode(rp, __LINE__);
  }
  EXPECT_EQ(0, std::memcmp(&rp.payload.parms, &parms, sizeof(parms)));
  comm_socket.close();
}

} // namespace level_zero_tests
