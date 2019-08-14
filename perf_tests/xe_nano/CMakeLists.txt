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


# if(UNIX)
#     set(OS_SPECIFIC_LIBS "")

#     set(PAPI_LIB papi)
#     find_library(PAPI_LIB_PATH ${PAPI_LIB})
#     if(PAPI_LIB_PATH)
#       set(XE_NANO_COMMON src/hardware_counter/hardware_counter_support.cpp)
#       set(OS_SPECIFIC_LIBS ${OS_SPECIFIC_LIBS} ${PAPI_LIB})
#       add_definitions(-DWITH_PAPI)
#       message(STATUS "PAPI_LIB: ${PAPI_LIB}")
#     else()
#       message(STATUS "PAPI library not found: No hardware counter support ")
#       set(XE_NANO_COMMON src/hardware_counter/hardware_counter_stub.cpp)
#     endif()
# else()
#     set(OS_SPECIFIC_LIBS "")
#     set(XE_NANO_COMMON src/hardware_counter/hardware_counter_stub.cpp)
# endif()

# set(CMAKE_CXX_STANDARD "11")
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(XE_NANO_COMMON
#     ${XE_NANO_COMMON}
#     ../common/src/xe_app.cpp
#     src/api_static_probe.cpp
# )

# set(XE_NANO_BENCHMARKS
#     src/xe_nano.cpp
#     src/benchmark.cpp
# )

# add_executable(xe_nano ${XE_NANO_COMMON} ${XE_NANO_BENCHMARKS})

# # add_dependencies(xe_nano copy_third_party_files ocloc_shared)

# target_include_directories(xe_nano PRIVATE "include" "../common/include")

# if (UNIX)
#     target_link_libraries(xe_nano
#         pthread
#     )
# endif()

# target_link_libraries(xe_nano ${OS_SPECIFIC_LIBS})

# if (UNIX)
#     target_link_libraries(xe_nano
#     ${LevelZero_LIBRARIES}
#     # ${NEO_MOCKABLE_LIB_NAME}
#     # compute_runtime_mockable_extra
#     # ${HW_LIBS_ULT}
#     gmock-gtest
#     pthread
#         rt
#     )
# else()
#     target_link_libraries(xe_nano
#     ${LevelZero_LIBRARIES}
#     # ${NEO_MOCKABLE_LIB_NAME}
#     # compute_runtime_mockable_extra
#     # ${HW_LIBS_ULT}
#     gmock-gtest
# 	dbghelp
#     )
# endif()

# # target_link_libraries(xe_nano
# #     ${NEO_MOCKABLE_LIB_NAME}
# #     compute_runtime_mockable_extra
# #     ${HW_LIBS_ULT}
# #     gmock-gtest
# # )

# #add_custom_command(
# #        TARGET xe_nano POST_BUILD
# #        COMMAND ${CMAKE_COMMAND} -E copy
# #                ${CMAKE_SOURCE_DIR}/level_zero/tests/perf_tests/xe_nano/kernels/xe_nano_benchmarks.spv
# #                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/xe_nano_benchmarks.spv)

# if(UNIX)
#     install(TARGETS xe_nano
#         DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
#         COMPONENT loki-xe-nano
#     )
# #    install(
# #        FILES
# #            "${CMAKE_CURRENT_SOURCE_DIR}/kernels/xe_nano_benchmarks.spv"
# #        DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
# #        COMPONENT loki-xe-nano
# #    )
# endif()

# if(MSVC)
#     set_target_properties(xe_nano
#         PROPERTIES
#             VS_DEBUGGER_COMMAND_ARGUMENTS ""
#             VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
#     )
# endif()





if(UNIX)
    set(OS_SPECIFIC_LIBS "")

    set(PAPI_LIB papi)
    find_library(PAPI_LIB_PATH ${PAPI_LIB})
    if(PAPI_LIB_PATH)
      set(XE_NANO_COMMON src/hardware_counter/hardware_counter_support.cpp)
      set(OS_SPECIFIC_LIBS ${OS_SPECIFIC_LIBS} ${PAPI_LIB})
      add_definitions(-DWITH_PAPI)
      message(STATUS "PAPI_LIB: ${PAPI_LIB}")
    else()
      message(STATUS "PAPI library not found: No hardware counter support ")
      set(XE_NANO_COMMON src/hardware_counter/hardware_counter_stub.cpp)
    endif()
else()
    set(OS_SPECIFIC_LIBS "")
    set(XE_NANO_COMMON src/hardware_counter/hardware_counter_stub.cpp)
endif()

set(CMAKE_CXX_STANDARD "11")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(XE_NANO_COMMON
    ${XE_NANO_COMMON}
    ../common/src/xe_app.cpp
    src/api_static_probe.cpp
)

set(XE_NANO_BENCHMARKS
    src/xe_nano.cpp
    src/benchmark.cpp
)

add_executable(xe_nano ${XE_NANO_COMMON} ${XE_NANO_BENCHMARKS})

target_link_libraries(xe_nano
    PRIVATE
        gmock
)

add_performance_application(xe_nano)

# install_resources_performance(test_xe_module
#     FILES
#     "${CMAKE_CURRENT_SOURCE_DIR}/kernels/xe_nano_benchmarks.spv"
# )