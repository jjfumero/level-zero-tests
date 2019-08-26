#
# Copyright(c) 2018 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

function(add_core_library name)
    cmake_parse_arguments(F "" "" "SOURCE" ${ARGN})
    add_library(${name} ${F_SOURCE})
    add_library(level_zero_tests::${name} ALIAS ${name})

    target_include_directories(${name}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
endfunction()

function(add_core_library_test project_name)
    cmake_parse_arguments(F "" "" "SOURCE" ${ARGN})
    set(name "${project_name}_tests")
    add_executable(${name} ${F_SOURCE})
    add_executable(level_zero_tests::${name} ALIAS ${name})

    target_link_libraries(${name}
        PUBLIC
        level_zero_tests::${project_name}
        GTest::GTest
    )

    target_include_directories(${name}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${LevelZero_INCLUDE_DIRS}
    )

    add_test(NAME ${name} COMMAND ${name} WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()


function(add_check_resources target)
    cmake_parse_arguments(PARSED_ARGS "" "" "FILES" ${ARGN})
    foreach(resource ${PARSED_ARGS_FILES})
        file(COPY "${resource}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    endforeach()
endfunction()

function(add_core_xe_library name)
    cmake_parse_arguments(F "" "" "SOURCE" ${ARGN})
    add_library(${name} ${F_SOURCE})
    add_library(level_zero_tests::${name} ALIAS ${name})

    target_include_directories(${name}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    if(CLANG_TIDY_EXE)
        set_target_properties(
            ${name}
            PROPERTIES
                 CXX_CLANG_TIDY ${CLANG_TIDY_EXE}
        )
    endif()
endfunction()

function(add_core_xe_library_test project_name)
    cmake_parse_arguments(F "" "" "SOURCE" ${ARGN})
    set(name "${project_name}_tests")
    add_executable(${name} ${F_SOURCE})
    add_executable(level_zero_tests::${name} ALIAS ${name})

    target_link_libraries(${name}
        PUBLIC
        level_zero_tests::${project_name}
        GTest::GTest
    )

    if(CLANG_TIDY_EXE)
        set_target_properties(
            ${name}
            PROPERTIES
                 CXX_CLANG_TIDY ${CLANG_TIDY_EXE}
        )
    endif()

    target_include_directories(${name}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${LevelZero_INCLUDE_DIRS}
    )

    add_test(NAME ${name} COMMAND ${name} WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()
