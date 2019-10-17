# INTEL CONFIDENTIAL
# Copyright (c) 2019 Intel Corporation. All Rights Reserved.
#
# The source code contained or described herein and all documents related to the
# source code ("Material") are owned by Intel Corporation or its suppliers
# or licensors. Title to the Material remains with Intel Corporation or its
# suppliers and licensors. The Material contains trade secrets and proprietary
# and confidential information of Intel or its suppliers and licensors. The
# Material is protected by worldwide copyright and trade secret laws and
# treaty provisions. No part of the Material may be used, copied, reproduced,
# modified, published, uploaded, posted, transmitted, distributed, or
# disclosed in any way without Intel's prior express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or delivery
# of the Materials, either expressly, by implication, inducement, estoppel or
# otherwise. Any license under such intellectual property rights must be
# express and approved by Intel in writing.

function(add_perf_test)
    set(options REQUIRES_OPENCL_INTEROP)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES INCLUDE_DIRECTORIES LINK_LIBRARIES KERNELS)
    cmake_parse_arguments(ADD_PERF_TEST
      "${options}" "${oneValueArgs}" "${multiValueArgs}"
      ${ARGN}
    )

    if(
      ADD_PERF_TEST_REQUIRES_OPENCL_INTEROP AND
      NOT LevelZero_OpenCL_INTEROP
    )
        message(STATUS "Disabling build of ${ADD_PERF_TEST_NAME} since Level-Zero OpenCL interop is not available")
        return()
    endif()

    add_executable(${ADD_PERF_TEST_NAME} ${ADD_PERF_TEST_SOURCES})

    if(CLANG_TIDY_EXE)
        set_target_properties(${ADD_PERF_TEST_NAME}
        PROPERTIES
            CXX_CLANG_TIDY ${CLANG_TIDY_EXE}
        )
    endif()

    target_link_libraries(${ADD_PERF_TEST_NAME}
      PRIVATE
        LevelZero::LevelZero
        ${ADD_PERF_TEST_LINK_LIBRARIES}
    )

    target_include_directories(${ADD_PERF_TEST_NAME}
      PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/perf_tests/common/include
        ${ADD_PERF_TEST_INCLUDE_DIRECTORIES}
    )

    foreach(kernel ${ADD_PERF_TEST_KERNELS})
        install(
          FILES "${CMAKE_CURRENT_SOURCE_DIR}/kernels/${kernel}.spv"
          DESTINATION "${CMAKE_BINARY_DIR}/out/perf_tests"
        )
    endforeach()

    install(
      TARGETS ${ADD_PERF_TEST_NAME}
      DESTINATION "${CMAKE_BINARY_DIR}/out/perf_tests"
      COMPONENT perf-tests
    )

    if(MSVC)
        set_target_properties(${ADD_PERF_TEST_NAME}
          PROPERTIES
            VS_DEBUGGER_COMMAND_ARGUMENTS ""
            VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
        )
    endif()
endfunction()

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION
  "${CMAKE_BINARY_DIR}/out/perf_tests"
)
include(InstallRequiredSystemLibraries)

add_subdirectory(xe_nano)
add_subdirectory(xe_peak)
add_subdirectory(xe_peer)
