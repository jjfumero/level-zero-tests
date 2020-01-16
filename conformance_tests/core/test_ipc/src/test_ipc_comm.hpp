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

#ifndef __TEST_IPC_COMM_HPP__

#define __TEST_IPC_COMM_HPP__

#include "ze_memory.h" /* for ze_memory_type_t */
#include <cstddef>
#include "ze_api.h"
#include <boost/asio.hpp>
namespace boost_ip = boost::asio::ip;

namespace level_zero_tests {

struct ipc_test_parameters {
  ze_memory_type_t source_type;
  size_t size;
  int data_pattern;
  ze_memory_type_t destination_type;
};

struct write_rndv_comm_context {
  ze_device_handle_t device;
  ze_driver_handle_t driver_handle;
  ze_command_queue_handle_t command_queue;
  ze_command_list_handle_t command_list;
};

/* These two functions allow transmission of a vector over sockets: */
void run_sender_sockets(const ipc_test_parameters &parms);
void run_receiver_sockets(ipc_test_parameters &parms);

/* These two functions allow transmission of a vector over the IPC API of L0: */
void run_sender(const ipc_test_parameters &parms, write_rndv_comm_context &ctx);
void run_receiver(ipc_test_parameters &parms, write_rndv_comm_context &ctx);

enum rndv_socket_opcode {
  /* Opcodes for sockets comm mechanism: */
  RSO_RTS_SOCKETS, /* Request to send, preface. */
  RSO_ACK_SOCKETS, /* Acknowledge message received preface messages. */
  /* Opcodes for IPC API in L0 comm mechanism: */
  RSO_RTS, /* Request to send */
  RSO_CTS, /* Clear to send */
  RSO_ACK, /* Acknowledge message received. */
  RSO_NAK, /* NOT acknowledge message received. */
};

struct RndvPacket {
  rndv_socket_opcode opcode;
  union {
    ipc_test_parameters parms; /* If opcode is RSO_RTS_SOCKETS then parms is
                                  filled in by the sender.  Receiver responds
                                  with RSO_ACK_SOCKETS, and the same parms. */
    /*size_t size;*/ /* If opcode is RTS, then parms is filled in with the size,
                        source & destination memory type is the payload. */
    ze_ipc_mem_handle_t ipc_mem_handle; /* If opcode is CTS, then ipc_mem_handle
                                           is the payload. */
    /* If opcode is ACK, then there is no payload. */
    struct NakInfo {
      int line;
      ze_result_t result;
    } nak_info;
    /* If opcode is NAK, then the line number of the error is stored in line and
     * the error is in result. */
  } payload;
};

void write_rndv_sockets_connect(boost_ip::tcp::socket &socket,
                                boost_ip::tcp::endpoint &ep,
                                int timeout_secs = 10);

size_t read_from_socket(boost_ip::tcp::socket &socket, uint8_t *buffer,
                        size_t size);

size_t write_to_socket(boost_ip::tcp::socket &socket, uint8_t *buffer,
                       size_t size);

#ifdef __linux__
int read_fd_from_socket(int socket);
int write_fd_to_socket(int socket, int fd);
#endif

/*

  Two peers, the 'sender' and the 'receiver', running in separate processes do
  the following actions to transmit a buffer using the rendezvous protocol.
  The buffer starts on the sender side, and has a given size:

     Sender:                                   Receiver
  1. Sends RTS to receiver with size of buffer
  2.                                           Receives RTS from sender,
                                               allocates buffer of given size
                                               creates ipc mem handle for buffer
                                               Sends CTS to sender with ipc mem
                                               handle.
  3. Receives CTS from receiver
     opens the buffer associated with the
     ipc mem handle.
     Copies the buffer from the sender's
     original buffer to the buffer associated
     with the ipc mem handle
     Checks all results
     if any step fails, send NAK to receiver
     with info for the NAK.
     Else (if no errors is detected) sends ACK
     to receiver.
  4.                                           Wait for socket packet.
                                               if it is an ACK then the message
                                               was received w/o error.
                                               Else if the packet is NAK the
                                               message was received with an
                                               error.

*/

/* low-level code for use by the peers themselves. */
void init_comm(write_rndv_comm_context &ctx);
void send_nak(ze_result_t result, boost_ip::tcp::socket &socket);
void finalize_comm(write_rndv_comm_context &ctx, boost_ip::tcp::socket &socket);
void unexpected_opcode(const RndvPacket &rp, int line);
const std::string to_string(rndv_socket_opcode oc);

// Returns the number of errors found:
uint32_t _validate_vbuff(const void *buff, write_rndv_comm_context &ctx,
                         const ipc_test_parameters &parms,
                         const char *const file, const int line);
uint32_t _validate_buff(const std::vector<uint8_t> &buff,
                        const ipc_test_parameters &parms,
                        const char *const file, const int line);

#define validate_vbuff(A, B, C) _validate_vbuff(A, B, C, __FILE__, __LINE__)
#define validate_buff(A, B) _validate_buff(A, B, __FILE__, __LINE__)

} // namespace level_zero_tests

#endif
