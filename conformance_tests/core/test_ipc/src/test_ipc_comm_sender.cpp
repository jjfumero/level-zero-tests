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
#include "logging/logging.hpp"
#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"
#include "gtest/gtest.h"

#ifdef __linux__
#include <sys/socket.h>
#include <sys/types.h>
#endif

namespace level_zero_tests {
const char *socket_path = "/tmp/ipc_socket";

void run_sender(const ipc_test_parameters &parms,
                write_rndv_comm_context &ctx) {

  void *our_buffer, *dest_buffer;
  // Allocate our buffer:
  if (parms.source_type == ZE_MEMORY_TYPE_UNKNOWN) {
    static int8_t buff[8 * 1024];
    our_buffer = buff;
  } else {
    allocate_mem(&our_buffer, parms.source_type, parms.size);
  }
  LOG_DEBUG << "Allocated buffer to be sent";

  const uint8_t dp = static_cast<uint8_t>(parms.data_pattern);

  // Initialize our buffer to a specific data pattern:
  append_memory_set(ctx.command_list, our_buffer, &dp, parms.size);
  close_command_list(ctx.command_list);
  execute_command_lists(ctx.command_queue, 1, &ctx.command_list, nullptr);
  synchronize(ctx.command_queue, UINT32_MAX);
  LOG_DEBUG << "Initialize buffer to be sent";
  reset_command_list(ctx.command_list);

  EXPECT_EQ(0, validate_vbuff(our_buffer, ctx, parms));

  boost::asio::io_service io_service;
  boost_ip::tcp::socket comm_socket(io_service);
  boost_ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"),
                             65432);

  write_rndv_sockets_connect(comm_socket, ep);
  print_platform_overview("sender process");

  RndvPacket rp;

  rp.opcode = RSO_RTS;
  rp.payload.parms = parms;
  if (write_to_socket(comm_socket, reinterpret_cast<uint8_t *>(&rp),
                      sizeof(rp)) != sizeof(rp)) {
    throw std::runtime_error("Client: writing RTS message");
  }
  LOG_DEBUG << "Transmitted RTS to receiver";

  ze_ipc_mem_handle_t ipc_mem_handle;

#ifdef __linux__
  /* Create a unix socket for streaming */

  struct sockaddr_un local_addr, remote_addr;
  local_addr.sun_family = AF_UNIX;
  strcpy(local_addr.sun_path, lzt::socket_path);
  unlink(local_addr.sun_path);

  int unix_rcv_socket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (unix_rcv_socket < 0) {
    perror("Server Connection Error");
    throw std::runtime_error("Client: Could not create socket");
  }

  if (bind(unix_rcv_socket, (struct sockaddr *)&local_addr,
           strlen(local_addr.sun_path) + sizeof(local_addr.sun_family)) == -1) {
    perror("Server Bind Error");
    throw std::runtime_error("Server: Could not bind to socket");
  }
  LOG_DEBUG << "Unix Socket Listening...";
  listen(unix_rcv_socket, 1);

  int len = sizeof(struct sockaddr_un);
  auto other_socket = accept(unix_rcv_socket, (struct sockaddr *)&remote_addr,
                             (socklen_t *)&len);
  LOG_DEBUG << "Connection accepted";

  int ipc_descriptor;

  if ((ipc_descriptor = read_fd_from_socket(other_socket)) < 0) {
    perror("Server Connection Error");
    throw std::runtime_error("Client: reading unix stream message");
  }
  LOG_DEBUG << "Received ipc descriptor from receiver";
  close(other_socket);
  close(unix_rcv_socket);
  memcpy(&ipc_mem_handle, static_cast<void *>(&ipc_descriptor),
         sizeof(ipc_descriptor));
#endif

  // if (read_from_socket(comm_socket, reinterpret_cast<uint8_t *>(&rp),
  //                      sizeof(rp)) != sizeof(rp)) {
  //   throw std::runtime_error("Client: reading stream message");
  // }
  // if (rp.opcode != RSO_CTS) {
  //   unexpected_opcode(rp, __LINE__);
  // }

  {
    ze_result_t result =
        zeDriverOpenMemIpcHandle(ctx.driver_handle, ctx.device,
                                 ipc_mem_handle /*rp.payload.ipc_mem_handle*/,
                                 ZE_IPC_MEMORY_FLAG_NONE, &dest_buffer);
    if (result != ZE_RESULT_SUCCESS) {
      send_nak(result, comm_socket);
      FAIL();
    }
  }
  LOG_DEBUG << "Got destination buffer";

  append_memory_copy(ctx.command_list, dest_buffer, our_buffer, parms.size,
                     nullptr);
  LOG_DEBUG << "Added memory copy command to command list";

  close_command_list(ctx.command_list);
  LOG_DEBUG << "Closed command list";

  execute_command_lists(ctx.command_queue, 1, &ctx.command_list, nullptr);
  LOG_DEBUG << "Executed command lists";

  synchronize(ctx.command_queue, UINT32_MAX);
  LOG_DEBUG << "Synchronized command queue";

  reset_command_list(ctx.command_list);

  EXPECT_EQ(0, validate_vbuff(dest_buffer, ctx, parms));
  LOG_DEBUG << "Validated buffer";
  {
    ze_result_t result =
        zeDriverCloseMemIpcHandle(ctx.driver_handle, dest_buffer);
    if (result != ZE_RESULT_SUCCESS) {
      send_nak(result, comm_socket);
      FAIL();
    }
  }
  LOG_DEBUG << "Closed ipc mem handle";
  finalize_comm(ctx, comm_socket);
  if (::testing::Test::HasFatalFailure()) {
    send_nak(ZE_RESULT_ERROR_UNKNOWN, comm_socket);
    return;
  }
  rp.opcode = RSO_ACK;
  if (write_to_socket(comm_socket, reinterpret_cast<uint8_t *>(&rp),
                      sizeof(rp)) != sizeof(rp)) {
    FAIL();
  }
  comm_socket.close();
  // destroy our buffer:
  if (parms.source_type != ZE_MEMORY_TYPE_UNKNOWN) {
    free_memory(&our_buffer);
  }
}

} // namespace level_zero_tests
