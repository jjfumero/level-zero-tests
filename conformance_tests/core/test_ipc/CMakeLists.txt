#
# INTEL CONFIDENTIAL
#
# Copyright 2019 Intel Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code (Material) are owned by Intel Corporation or its
# suppliers or licensors.
#
# Title to the Material remains with Intel Corporation or its suppliers and
# licensors. The Material contains trade secrets and proprietary and
# confidential information of Intel or its suppliers and licensors.
# The Material is protected by worldwide copyright and trade secret laws and
# treaty provisions. No part of the Material may be used, copied, reproduced,
# modified, published, uploaded, posted, transmitted, distributed,
# or disclosed in any way without Intel's prior express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure
# or delivery of the Materials, either expressly, by implication, inducement,
# estoppel or otherwise. Any license under such intellectual property rights
# must be express and approved by Intel in writing.
#

set(ipc_libraries
  level_zero_tests::logging
  level_zero_tests::utils
  Boost::system
)

if(UNIX)
    list(APPEND ipc_libraries rt)
endif()

add_conformance_test_executable(
  NAME test_ipc_event_helper
  PREFIX "ipc"  # install to prefix so it's not confused for a test
  SOURCES
    src/test_ipc_event_helper.cpp
  LINK_LIBRARIES ${ipc_libraries}
)

add_conformance_test(
  NAME test_ipc
  SOURCES
    src/test_ipc.cpp
    src/test_ipc_event.cpp
    src/main.cpp
    src/test_ipc_comm_sender_sockets.cpp
    src/test_ipc_comm_receiver.cpp
    src/test_ipc_comm.cpp
  LINK_LIBRARIES ${ipc_libraries}
)

add_conformance_test_executable(
  NAME test_ipc_helper
  PREFIX "ipc"  # install to prefix so it's not confused for a test
  SOURCES
    src/test_ipc_helper.cpp
    src/test_ipc_comm_sender.cpp
    src/test_ipc_comm_receiver_sockets.cpp
    src/test_ipc_comm.cpp
  LINK_LIBRARIES ${ipc_libraries}
)
