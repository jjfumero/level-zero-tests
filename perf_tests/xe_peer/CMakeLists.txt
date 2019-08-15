# Copyright (c) 2019, Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
# set(OS_SPECIFIC_LIBS "")
# set(CMAKE_CXX_STANDARD "11")
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(XE_PEER_COMMON
#     ../common/src/xe_app.cpp
# )

# set(XE_PEER_BENCHMARKS
#     src/xe_peer.cpp
# )

# add_executable(xe_peer ${XE_PEER_BENCHMARKS} ${XE_PEER_COMMON} )

# # add_dependencies(xe_peer copy_third_party_files ocloc_shared)

# if (UNIX)
#     target_link_libraries(xe_peer
#         pthread
#     )
# endif()

# #target_link_libraries(xe_peer ${LevelZero_LIBRARIES} pthread gmock-gtest ${OS_SPECIFIC_LIBS})

# if (UNIX)
#     target_link_libraries(xe_peer
#     ${LevelZero_LIBRARIES}
#     # ${NEO_MOCKABLE_LIB_NAME}
#     # compute_runtime_mockable_extra
#     # ${HW_LIBS_ULT}
#     gmock-gtest
#     pthread
#         rt
#     )
# else()
#     target_link_libraries(xe_peer
#     ${LevelZero_LIBRARIES}
#     # ${NEO_MOCKABLE_LIB_NAME}
#     # compute_runtime_mockable_extra
#     # ${HW_LIBS_ULT}
#     gmock-gtest
# 	dbghelp
#     )
# endif()

# # target_link_libraries(xe_peer
# #     ${NEO_MOCKABLE_LIB_NAME}
# #     compute_runtime_mockable_extra
# #     ${HW_LIBS_ULT}
# #     gmock-gtest
# # )

# target_include_directories(xe_peer PRIVATE "include" "../common/include")

# #add_custom_command(
# #        TARGET xe_peer POST_BUILD
# #        COMMAND ${CMAKE_COMMAND} -E copy
# #                ${CMAKE_SOURCE_DIR}/level_zero/tests/perf_tests/xe_peer/kernels/xe_peer_benchmarks.spv
# #                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/xe_peer_benchmarks.spv)

# if(UNIX)
#     install(TARGETS xe_peer
#         DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
#         COMPONENT loki-xe-peer
#     )
# #    install(
# #        FILES
# #            "${CMAKE_CURRENT_SOURCE_DIR}/kernels/xe_peer_benchmarks.spv"
# #        DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
# #        COMPONENT loki-xe-peer
# #    )
# endif()

# if(MSVC)
#     set_target_properties(xe_peer
#         PROPERTIES
#             VS_DEBUGGER_COMMAND_ARGUMENTS ""
#             VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
#     )
# endif()






if(UNIX)
    set(OS_SPECIFIC_LIBS pthread rt)
else()
    set(OS_SPECIFIC_LIBS "")
endif()

set(CMAKE_CXX_STANDARD "11")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(XE_PEER_COMMON ../common/src/xe_app.cpp)
set(XE_PEER_BENCHMARKS
    src/xe_peer.cpp
)

add_executable(xe_peer ${XE_PEER_COMMON} ${XE_PEER_BENCHMARKS})

target_include_directories(xe_peer
  PRIVATE
    # "include"
    "../common/include"
)

add_performance_application(xe_peer)

install_resources_performance(xe_peak
  FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/kernels/xe_peer_benchmarks.spv"
)
