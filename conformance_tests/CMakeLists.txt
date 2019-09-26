## INTEL CONFIDENTIAL
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

# The current distinction between add_conformance_test and
# add_conformance_test_executable exists mainly for test_xe_ipc, which is
# currently written to have a "helper" binary that isn't a conformance test by
# itself.

function(add_conformance_test)
    set(options REQUIRES_OPENCL_INTEROP)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES INCLUDE_DIRECTORIES LINK_LIBRARIES KERNELS MEDIA)
    cmake_parse_arguments(ADD_CONFORMANCE_TEST
      "${options}" "${oneValueArgs}" "${multiValueArgs}"
      ${ARGN}
    )

    if(
      ADD_CONFORMANCE_TEST_REQUIRES_OPENCL_INTEROP AND
      NOT LevelZero_OpenCL_INTEROP
    )
        message(STATUS "Disabling build of ${ADD_CONFORMANCE_TEST_NAME} since Level-Zero OpenCL interop is not available")
        return()
    endif()

    add_conformance_test_executable(
      NAME ${ADD_CONFORMANCE_TEST_NAME}
      PREFIX "."
      SOURCES ${ADD_CONFORMANCE_TEST_SOURCES}
      INCLUDE_DIRECTORIES ${ADD_CONFORMANCE_TEST_INCLUDE_DIRECTORIES}
      LINK_LIBRARIES ${ADD_CONFORMANCE_TEST_LINK_LIBRARIES}
      KERNELS ${ADD_CONFORMANCE_TEST_KERNELS}
      MEDIA ${ADD_CONFORMANCE_TEST_MEDIA}
    )
endfunction()

function(add_conformance_test_executable)
    set(options "")
    set(oneValueArgs NAME PREFIX)
    set(multiValueArgs SOURCES INCLUDE_DIRECTORIES LINK_LIBRARIES KERNELS MEDIA)
    cmake_parse_arguments(ADD_CONFORMANCE_TEST_EXECUTABLE
      "${options}" "${oneValueArgs}" "${multiValueArgs}"
      ${ARGN}
    )

    add_executable(
      ${ADD_CONFORMANCE_TEST_EXECUTABLE_NAME}
      ${ADD_CONFORMANCE_TEST_EXECUTABLE_SOURCES}
    )

    if(CLANG_TIDY_EXE)
        set_target_properties(${ADD_CONFORMANCE_TEST_EXECUTABLE_NAME}
          PROPERTIES
            CXX_CLANG_TIDY ${CLANG_TIDY_EXE}
        )
    endif()

    target_link_libraries(${ADD_CONFORMANCE_TEST_EXECUTABLE_NAME}
      PRIVATE
        LevelZero::LevelZero
        level_zero_tests::xe_test_harness
        GMock::GMock
        GTest::GTest
        ${ADD_CONFORMANCE_TEST_EXECUTABLE_LINK_LIBRARIES}
    )

    target_include_directories(${ADD_CONFORMANCE_TEST_EXECUTABLE_NAME}
      PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${ADD_CONFORMANCE_TEST_EXECUTABLE_INCLUDE_DIRECTORIES}
    )

    foreach(kernel ${ADD_CONFORMANCE_TEST_EXECUTABLE_KERNELS})
        install(
          FILES "${CMAKE_CURRENT_SOURCE_DIR}/kernels/${kernel}.spv"
          DESTINATION "${CMAKE_BINARY_DIR}/out/conformance_tests/${ADD_CONFORMANCE_TEST_EXECUTABLE_PREFIX}"
        )
    endforeach()

    foreach(media ${ADD_CONFORMANCE_TEST_EXECUTABLE_MEDIA})
        install(
          FILES "${MEDIA_DIRECTORY}/${media}"
          DESTINATION "${CMAKE_BINARY_DIR}/out/conformance_tests/${ADD_CONFORMANCE_TEST_EXECUTABLE_PREFIX}"
        )
    endforeach()

    install(
      TARGETS ${ADD_CONFORMANCE_TEST_EXECUTABLE_NAME}
      DESTINATION "${CMAKE_BINARY_DIR}/out/conformance_tests/${ADD_CONFORMANCE_TEST_EXECUTABLE_PREFIX}"
      COMPONENT perf-tests
    )

    if(MSVC)
        set_target_properties(${ADD_CONFORMANCE_TEST_EXECUTABLE_NAME}
          PROPERTIES
            VS_DEBUGGER_COMMAND_ARGUMENTS ""
            VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
        )
    endif()
endfunction()

add_subdirectory(test_xe_barrier)
add_subdirectory(test_xe_cl_interop)
add_subdirectory(test_xe_cmdlist)
add_subdirectory(test_xe_cmdqueue)
add_subdirectory(test_xe_copy)
add_subdirectory(test_xe_device)
add_subdirectory(test_xe_driver)
add_subdirectory(test_xe_event)
add_subdirectory(test_xe_fence)
add_subdirectory(test_xe_image)
add_subdirectory(test_xe_ipc)
add_subdirectory(test_xe_memory)
add_subdirectory(test_xe_memory_overcommit)
add_subdirectory(test_xe_module)
add_subdirectory(test_xe_p2p)
add_subdirectory(test_xe_residency)
add_subdirectory(test_xe_sampler)
add_subdirectory(test_xe_template)
