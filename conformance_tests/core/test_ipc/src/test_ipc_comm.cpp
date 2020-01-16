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

#include <chrono>
#include <thread>
#ifdef __linux__
#include <sys/socket.h>
#endif
#include "test_ipc_comm.hpp"
#include "logging/logging.hpp"
#include "gtest/gtest.h"
#include "test_harness/test_harness.hpp"

namespace level_zero_tests {

const std::string to_string(rndv_socket_opcode oc) {
  switch (oc) {
  case RSO_RTS_SOCKETS:
    return "RSO_RTS_SOCKETS";
    break;
  case RSO_ACK_SOCKETS:
    return "RSO_ACK_SOCKETS";
    break;
  case RSO_RTS:
    return "RSO_RTS";
    break;
  case RSO_CTS:
    return "RSO_CTS";
    break;
  case RSO_ACK:
    return "RSO_ACK";
    break;
  case RSO_NAK:
    return "RSO_NAK";
    break;
  default:
    return "unknown opcode";
    break;
  }
}

size_t read_from_socket(boost_ip::tcp::socket &socket, uint8_t *buffer,
                        size_t size) {
  boost::system::error_code error;
  size_t received_bytes =
      boost::asio::read(socket, boost::asio::buffer(buffer, size), error);
  if (((error.value()) != boost::system::errc::success) ||
      (received_bytes < size)) {
    return 0;
  }
  return received_bytes;
}

size_t write_to_socket(boost_ip::tcp::socket &socket, uint8_t *buffer,
                       size_t size) {
  boost::system::error_code error;
  size_t sent_bytes =
      boost::asio::write(socket, boost::asio::buffer(buffer, size), error);
  if (((error.value()) != boost::system::errc::success) ||
      (sent_bytes < size)) {
    return 0;
  }
  return sent_bytes;
}

#ifdef __linux__

int read_fd_from_socket(int unix_socket) {
  // call recvmsg to receive the descriptor on the unix_socket
  int fd = -1;
  char recv_buff[sizeof(ze_ipc_mem_handle_t)] = {};
  char cmsg_buff[CMSG_SPACE(sizeof(ze_ipc_mem_handle_t))];

  struct iovec msg_buffer;
  msg_buffer.iov_base = recv_buff;
  msg_buffer.iov_len = sizeof(recv_buff);

  struct msghdr msg_header = {};
  msg_header.msg_iov = &msg_buffer;
  msg_header.msg_iovlen = 1;
  msg_header.msg_control = cmsg_buff;
  msg_header.msg_controllen = CMSG_LEN(sizeof(fd));

  size_t bytes = recvmsg(unix_socket, &msg_header, 0);
  if (bytes < 0) {
    throw std::runtime_error("Client: Error receiving ipc handle");
  }

  struct cmsghdr *control_header = CMSG_FIRSTHDR(&msg_header);
  memmove(&fd, CMSG_DATA(control_header), sizeof(int));
  return fd;
}

int write_fd_to_socket(int unix_socket,
                       int fd) { // fd is the ipc handle
  char send_buff[sizeof(ze_ipc_mem_handle_t)];
  char cmsg_buff[CMSG_SPACE(sizeof(ze_ipc_mem_handle_t))];

  struct iovec msg_buffer;
  msg_buffer.iov_base = send_buff;
  msg_buffer.iov_len = sizeof(*send_buff);

  // build a msghdr containing the desriptor (fd)
  // fd is sent as ancillary data, i.e. msg_control
  // member of msghdr
  struct msghdr msg_header = {};
  msg_header.msg_iov = &msg_buffer;
  msg_header.msg_iovlen = 1;
  msg_header.msg_control = cmsg_buff;
  msg_header.msg_controllen = CMSG_LEN(sizeof(fd));

  struct cmsghdr *control_header = CMSG_FIRSTHDR(&msg_header);
  control_header->cmsg_type = SCM_RIGHTS;
  control_header->cmsg_level = SOL_SOCKET;
  control_header->cmsg_len = CMSG_LEN(sizeof(fd));

  *(int *)CMSG_DATA(control_header) = fd;
  // call sendmsg to send descriptor across socket
  ssize_t bytesSent = sendmsg(unix_socket, &msg_header, 0);
  if (bytesSent < 0) {
    return -1;
  }

  return 0;
}
#endif

void init_comm(write_rndv_comm_context &ctx) {
  ctx.driver_handle = lzt::get_default_driver();
  LOG_DEBUG << "Got driver handle";
  ctx.device = lzt::get_default_device();
  LOG_DEBUG << "Got device";
  ctx.command_queue = create_command_queue();
  LOG_DEBUG << "Got command queue";
  ctx.command_list = create_command_list();
  LOG_DEBUG << "Got command list";
}

void send_nak(ze_result_t result, boost_ip::tcp::socket &socket) {
  RndvPacket rp;

  if (socket.is_open()) {
    rp.opcode = RSO_NAK;
    rp.payload.nak_info.line = __LINE__;
    rp.payload.nak_info.result = result;
    if (write_to_socket(socket, reinterpret_cast<uint8_t *>(&rp), sizeof(rp)) !=
        sizeof(rp)) {
      throw std::runtime_error("Sending NAK message");
    }
  }
}

void finalize_comm(write_rndv_comm_context &ctx,
                   boost_ip::tcp::socket &socket) {
  destroy_command_list(ctx.command_list);
  LOG_DEBUG << "Destroyed command list";
  destroy_command_queue(ctx.command_queue);
  LOG_DEBUG << "Destroyed command queue";
}

void unexpected_opcode(const RndvPacket &rp, int line) {
  LOG_DEBUG << "unexpected opcode: " + to_string(rp.opcode)
            << " received at line: " + line;
  throw std::runtime_error("Client: reading stream message, unexpected opcode");
}

void write_rndv_sockets_connect(boost_ip::tcp::socket &socket,
                                boost_ip::tcp::endpoint &ep, int timeout_secs) {
  do {
    boost::system::error_code error;
    socket.connect(ep, error);
    if ((error.value()) != boost::system::errc::success) {
      // An error occurred. Probably connection refused.
      LOG_DEBUG << "Sleeping 1 second.." << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    } else {
      break;
    }
    timeout_secs--;
  } while (timeout_secs > 0);
}

uint32_t _validate_vbuff(const void *buff, write_rndv_comm_context &ctx,
                         const ipc_test_parameters &parms,
                         const char *const file, const int line) {
  std::vector<uint8_t> my_buff(parms.size);
  append_memory_copy(ctx.command_list, my_buff.data(), buff, parms.size,
                     nullptr);
  close_command_list(ctx.command_list);
  execute_command_lists(ctx.command_queue, 1, &ctx.command_list, nullptr);
  synchronize(ctx.command_queue, UINT32_MAX);
  reset_command_list(ctx.command_list);
  return _validate_buff(my_buff, parms, file, line);
}

uint32_t _validate_buff(const std::vector<uint8_t> &buff,
                        const ipc_test_parameters &parms,
                        const char *const file, const int my_line) {
  EXPECT_EQ(buff.size(), parms.size);
  uint32_t errs = 0;
  for (unsigned i = 0; i < buff.size(); i++) {
    if (buff[i] != parms.data_pattern) {
      LOG_DEBUG << file << ":" << my_line << " Unexpected data.  Expected: 0x"
                << std::hex << parms.data_pattern << std::endl;
      LOG_DEBUG << file << ":" << my_line << " got: output[" << std::dec << i
                << "] = 0x" << std::hex << int(buff[i]) << std::endl;
      errs++;
    }
  }
  return errs;
}

} // namespace level_zero_tests
