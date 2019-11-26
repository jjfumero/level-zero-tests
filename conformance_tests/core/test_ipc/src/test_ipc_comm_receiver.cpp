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
#include "test_harness/test_harness.hpp"
#include "gtest/gtest.h"
#include <cstring>

namespace level_zero_tests {

void run_receiver(ipc_test_parameters &parms, write_rndv_comm_context &ctx) {
  boost::asio::io_service io_service;
  boost_ip::tcp::socket comm_socket(io_service);
  boost_ip::tcp::acceptor acceptor(
      io_service, boost_ip::tcp::endpoint(boost_ip::tcp::v4(), 65432));
  acceptor.accept(comm_socket);
  print_platform_overview("receiver process");
  int8_t buff[8 * 1024];
  void *buffer = buff;
  RndvPacket rp;
  if (read_from_socket(comm_socket, reinterpret_cast<uint8_t *>(&rp),
                       sizeof(rp)) != sizeof(rp)) {
    throw std::runtime_error("Client: reading stream message");
  }
  if (rp.opcode != RSO_RTS) {
    unexpected_opcode(rp, __LINE__);
  }
  EXPECT_EQ(0, std::memcmp(&parms, &rp.payload.parms, sizeof(parms)));
  const ipc_test_parameters RTS_msg = rp.payload.parms;
  LOG_DEBUG << "Message size from RTS is: " + RTS_msg.size;

  allocate_mem_and_get_ipc_handle(&rp.payload.ipc_mem_handle, &buffer,
                                  RTS_msg.destination_type, RTS_msg.size);

  rp.opcode = RSO_CTS;
  if (write_to_socket(comm_socket, reinterpret_cast<uint8_t *>(&rp),
                      sizeof(rp)) != sizeof(rp)) {
    throw std::runtime_error("Receiver: writing on stream socket");
  }
  if (read_from_socket(comm_socket, reinterpret_cast<uint8_t *>(&rp),
                       sizeof(rp)) != sizeof(rp)) {
    throw std::runtime_error("Client: reading stream message");
  }
  if (rp.opcode != RSO_ACK) {
    unexpected_opcode(rp, __LINE__);
  }

  comm_socket.close();
  acceptor.close();

  // Copy from device-allocated memory to output vector:
  std::vector<uint8_t> output(RTS_msg.size);
  append_memory_copy(ctx.command_list, output.data(), buffer,
                     size_in_bytes(output), nullptr);
  LOG_DEBUG << "Added memory copy command to command list";

  close_command_list(ctx.command_list);
  LOG_DEBUG << "Closed Command list";

  execute_command_lists(ctx.command_queue, 1, &ctx.command_list, nullptr);
  LOG_DEBUG << "Executed command lists in command queue";

  synchronize(ctx.command_queue, UINT32_MAX);
  LOG_DEBUG << "Synchronized command queue";

  // Validate correct data was received:
  EXPECT_EQ(0, validate_buff(output, RTS_msg));
  LOG_DEBUG << "Validated received data";

  free_memory(ctx.driver_handle, buffer);
  LOG_DEBUG << "Freed device buffer";
  finalize_comm(ctx, comm_socket);
}

} // namespace level_zero_tests
