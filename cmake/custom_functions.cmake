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

function(add_kernels target)
    foreach(kernel ${ARGN})
        target_sources(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/${kernel}")
        source_group("Kernel Files" FILES "${CMAKE_CURRENT_SOURCE_DIR}/${kernel}")
        #TODO Replace kernel links below by cmake's CREATE_LINK after updating infrastructure to cmake version 3.14.
        if(WIN32)
          file(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${kernel}" native_current_binary_dir_kernel)
          file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${kernel}" native_current_source_dir_kernel)
          if(NOT EXISTS ${native_current_binary_dir_kernel})
            execute_process(COMMAND cmd /c mklink /H ${native_current_binary_dir_kernel} ${native_current_source_dir_kernel})
          endif()
        else()
          execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${CMAKE_CURRENT_SOURCE_DIR}/${kernel}" "${CMAKE_CURRENT_BINARY_DIR}/${kernel}")
        endif()
    endforeach()
endfunction()

function(install_kernels target)
    foreach(kernel ${ARGN})
        install(FILES "${kernel}" DESTINATION ".")
    endforeach()
endfunction()

function(add_core_library name)
    cmake_parse_arguments(F "" "" "SOURCE" ${ARGN})
    add_library(${name} ${F_SOURCE})
    add_library(compute_samples::${name} ALIAS ${name})

    target_include_directories(${name}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    install(TARGETS ${name}
        EXPORT ${name}-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
    )

    install(DIRECTORY include/${name} DESTINATION include)

    include(CMakePackageConfigHelpers)
    configure_package_config_file(cmake/${name}-config.cmake.in
        ${PROJECT_BINARY_DIR}/${name}-config.cmake
        INSTALL_DESTINATION lib/cmake/${PROJECT_NAME})
    install(FILES ${PROJECT_BINARY_DIR}/${name}-config.cmake
        DESTINATION lib/cmake/${PROJECT_NAME})
    install(EXPORT ${name}-targets
        FILE ${name}-targets.cmake
        NAMESPACE compute_samples::
        DESTINATION lib/cmake/${PROJECT_NAME}
        COMPONENT ${name})

    set_target_properties(${name} PROPERTIES FOLDER core/${name})
endfunction()

function(add_core_library_test project_name)
    cmake_parse_arguments(F "" "" "SOURCE" ${ARGN})
    set(name "${project_name}_tests")
    add_executable(${name} ${F_SOURCE})
    add_executable(compute_samples::${name} ALIAS ${name})

    target_link_libraries(${name}
        PUBLIC
        compute_samples::${project_name}
        GTest::GTest
    )

    target_include_directories(${name}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${LevelZero_INCLUDE_DIRS}
    )

    install(TARGETS ${name} DESTINATION ".")

    set_target_properties(${name} PROPERTIES FOLDER core/${project_name})

    add_test(NAME ${name} COMMAND ${name} WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()

function(install_resources target)
    cmake_parse_arguments(PARSED_ARGS "" "" "FILES;DIRECTORIES" ${ARGN})
    foreach(resource ${PARSED_ARGS_FILES})
        file(COPY "${resource}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
        install(FILES "${resource}" DESTINATION ".")
    endforeach()
    foreach(resource ${PARSED_ARGS_DIRECTORIES})
        file(COPY "${resource}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
        install(DIRECTORY "${resource}" DESTINATION ".")
    endforeach()
endfunction()

function(add_core_xe_library name)
    cmake_parse_arguments(F "" "" "SOURCE" ${ARGN})
    add_library(${name} ${F_SOURCE})
    add_library(compute_samples::${name} ALIAS ${name})

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

    install(TARGETS ${name}
        EXPORT ${name}-targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
    )

    install(DIRECTORY include/${name} DESTINATION include)

    include(CMakePackageConfigHelpers)
    configure_package_config_file(cmake/${name}-config.cmake.in
        ${PROJECT_BINARY_DIR}/${name}-config.cmake
        INSTALL_DESTINATION lib/cmake/${PROJECT_NAME})
    install(FILES ${PROJECT_BINARY_DIR}/${name}-config.cmake
        DESTINATION lib/cmake/${PROJECT_NAME})
    install(EXPORT ${name}-targets
        FILE ${name}-targets.cmake
        NAMESPACE compute_samples::
        DESTINATION lib/cmake/${PROJECT_NAME}
        COMPONENT ${name})

    set_target_properties(${name} PROPERTIES FOLDER core/${name})
endfunction()

function(add_core_xe_library_test project_name)
    cmake_parse_arguments(F "" "" "SOURCE" ${ARGN})
    set(name "${project_name}_tests")
    add_executable(${name} ${F_SOURCE})
    add_executable(compute_samples::${name} ALIAS ${name})

    target_link_libraries(${name}
        PUBLIC
        compute_samples::${project_name}
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

    install(TARGETS ${name} DESTINATION ".")

    set_target_properties(${name} PROPERTIES FOLDER core/${project_name})

    add_test(NAME ${name} COMMAND ${name} WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()

function(install_resources_performance target)
    cmake_parse_arguments(PARSED_ARGS "" "" "FILES;DIRECTORIES" ${ARGN})
    foreach(resource ${PARSED_ARGS_FILES})
        file(COPY "${resource}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
        install(FILES "${resource}" DESTINATION "." COMPONENT perf-tests)
    endforeach()
    foreach(resource ${PARSED_ARGS_DIRECTORIES})
        file(COPY "${resource}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
        install(DIRECTORY "${resource}" DESTINATION "${CMAKE_BINARY_DIR}/perf_tests" COMPONENT perf-tests)
    endforeach()
endfunction()

function(add_performance_application name)
    target_link_libraries(${name}
        PRIVATE
            ${LevelZero_LIBRARIES}
            ${OS_SPECIFIC_LIBS}
    )

    target_include_directories(${name}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/perf_tests/common/include
            ${LevelZero_INCLUDE_DIRS}
    )

    if(UNIX)
        install(
            TARGETS ${name}
            DESTINATION "${CMAKE_BINARY_DIR}/out/perf_tests"
            COMPONENT perf-tests
        )
    endif()

    if(MSVC)
        set_target_properties(${name}
            PROPERTIES
                VS_DEBUGGER_COMMAND_ARGUMENTS ""
                VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
        )
    endif()

endfunction()

function(install_kernels_conformance target)
    foreach(kernel ${ARGN})
        install(FILES "${kernel}" DESTINATION "." COMPONENT conformance-tests)
    endforeach()
endfunction()


function(install_resources_conformance target)
    cmake_parse_arguments(PARSED_ARGS "" "" "FILES;DIRECTORIES" ${ARGN})
    foreach(resource ${PARSED_ARGS_FILES})
        file(COPY "${resource}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
        install(FILES "${resource}" DESTINATION "." COMPONENT conformance-tests)
    endforeach()
    foreach(resource ${PARSED_ARGS_DIRECTORIES})
        file(COPY "${resource}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
        install(DIRECTORY "${resource}" DESTINATION "${CMAKE_BINARY_DIR}/conformance_tests" COMPONENT conformance-tests)
    endforeach()
endfunction()

function(add_test_suite name)
    cmake_parse_arguments(F "" "" "SOURCE" ${ARGN})
    add_executable(${name} ${F_SOURCE})

    target_include_directories(${name}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${LevelZero_INCLUDE_DIRS}
    )

    if(CLANG_TIDY_EXE)
        set_target_properties(
            ${name}
            PROPERTIES
                 CXX_CLANG_TIDY ${CLANG_TIDY_EXE}
        )
    endif()

    target_link_libraries(${name}
        PUBLIC
        GMock::GMock
        GTest::GTest
    )

    install(TARGETS ${name}
            DESTINATION "${CMAKE_BINARY_DIR}/out/conformance_tests"
            COMPONENT conformance-tests)

    set_target_properties(${name} PROPERTIES FOLDER tests/${name})

    add_test(NAME ${name} COMMAND ${name} WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endfunction()
