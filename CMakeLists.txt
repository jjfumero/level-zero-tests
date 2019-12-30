cmake_minimum_required(VERSION 3.8.0 FATAL_ERROR)
project(intel-level-zero-tests
    VERSION 0.4.0  # corresponds to level-zero release
)

# Generate VERSION and VERSION_SUFFIX (if building in tree) files to facilitate
# artifact publishing.
file(WRITE "${CMAKE_BINARY_DIR}/VERSION" "${PROJECT_VERSION}")
find_program(GIT_EXE NAMES "git")
if(GIT_EXE AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    if(MSVC)
        execute_process(
            COMMAND CMD /c git rev-list --count HEAD
            OUTPUT_VARIABLE VERSION_SUFFIX
        )
    else()
        execute_process(
            COMMAND git rev-list --count HEAD
            OUTPUT_VARIABLE VERSION_SUFFIX
        )
    endif()
    string(STRIP "${VERSION_SUFFIX}" VERSION_SUFFIX)
    file(WRITE "${CMAKE_BINARY_DIR}/VERSION_SUFFIX" "${VERSION_SUFFIX}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(clang_tools)
include(custom_functions)
include(vs2019_missing_redist_workaround)

# FindPNG doesn't listen to PNG_ROOT, so use CMAKE_PREFIX_PATH for it
if(PNG_ROOT)
    list(APPEND CMAKE_PREFIX_PATH "${PNG_ROOT}")
endif()
# Older versions of the FindPNG module don't search for the static libraries, so
# help it out
find_library(PNG_LIBRARY
  NAMES libpng.a libpng16_static libpng16_staticd
  PATHS "${PNG_ROOT}"
  PATH_SUFFIXES lib
  NO_DEFAULT_PATH
)
find_package(PNG REQUIRED)

if(NOT TARGET GTest::GTest AND NOT TARGET GMock::GMock)
    if(NOT DEFINED GTEST_ROOT)
        set(GTEST_ROOT "${CMAKE_SOURCE_DIR}/third_party/googletest")
    endif()
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory("${GTEST_ROOT}" "${GTEST_ROOT}/build" EXCLUDE_FROM_ALL)
    add_library(GTest::GTest ALIAS gtest)
    add_library(GMock::GMock ALIAS gmock)
endif()
enable_testing()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.65 REQUIRED COMPONENTS log program_options timer chrono system)

option(REQUIRE_LEVELZERO_OPENCL_INTEROP
  "Enables OpenCL interop testing with Level-Zero (requires OpenCL)"
  NO
)
if(REQUIRE_LEVELZERO_OPENCL_INTEROP)
    list(APPEND LevelZero_COMPONENTS "OpenCL")
endif()

find_package(LevelZero REQUIRED COMPONENTS ${LevelZero_COMPONENTS})

set(MEDIA_ROOT_DIRECTORY "${CMAKE_SOURCE_DIR}/mediadata")
set(MEDIA_DIRECTORY "${MEDIA_ROOT_DIRECTORY}/merged")
set(MEDIADATA_ROOT "${MEDIA_ROOT_DIRECTORY}/external")
file(COPY "${MEDIADATA_ROOT}/yuv" DESTINATION "${MEDIA_DIRECTORY}")
file(COPY "${MEDIA_ROOT_DIRECTORY}/internal/yuv" DESTINATION "${MEDIA_DIRECTORY}")
file(COPY "${MEDIA_ROOT_DIRECTORY}/internal/bmp" DESTINATION "${MEDIA_DIRECTORY}")
file(COPY "${MEDIA_ROOT_DIRECTORY}/internal/png" DESTINATION "${MEDIA_DIRECTORY}")

if(NOT DEFINED GROUP)
    set(GROUP "/")
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/out" CACHE PATH "..." FORCE)
endif()

function(assert_valid_test_group_name group_name)
    set(regex "^([/]([A-Za-z0-9_]*))*$")
    string(REGEX MATCH "${regex}" valid_name "${group_name}")
    if(NOT "${group_name}" STREQUAL "${valid_name}")
        message(FATAL_ERROR "${group_name} is not a valid test group name, valid regex: ${regex}")
    endif()
endfunction()

# Cmake 3.8 doesn't support the list(SUBLIST ...) command
function(list_sublist list_var start_index length result_var)
    if(length EQUAL 0)
        set("${result_var}" "" PARENT_SCOPE)
        return()
    endif()
    math(EXPR end_index "${start_index}+${length}-1")
    set("${result_var}" "")
    foreach(index RANGE ${start_index} ${end_index})
        list(GET "${list_var}" "${index}" item)
        list(APPEND "${result_var}" "${item}")
    endforeach()
    set("${result_var}" "${${result_var}}" PARENT_SCOPE)
endfunction()

function(is_test_group_member group1 group2 result_var)
    assert_valid_test_group_name("${group1}")
    assert_valid_test_group_name("${group2}")
    string(REGEX MATCHALL "([A-Za-z0-9_])+" group1_list "${group1}")
    string(REGEX MATCHALL "([A-Za-z0-9_])+" group2_list "${group2}")
    list(LENGTH group1_list group1_list_length)
    list(LENGTH group2_list group2_list_length)
    if(group1_list_length GREATER group2_list_length)
        set(list_length ${group2_list_length})
    else()
        set(list_length ${group1_list_length})
    endif()
    # list(SUBLIST group2_list 0 "${group1_list_length}" group2_common)
    list_sublist(group2_list 0 "${list_length}" group2_common)
    if(group2_common STREQUAL group1_list)
        set("${result_var}" "TRUE" PARENT_SCOPE)
    else()
        set("${result_var}" "FALSE" PARENT_SCOPE)
    endif()
endfunction()

function(add_lzt_test_executable)
    set(options "")
    set(oneValueArgs NAME PREFIX GROUP)
    set(multiValueArgs SOURCES INCLUDE_DIRECTORIES LINK_LIBRARIES KERNELS MEDIA)
    cmake_parse_arguments(ADD_LZT_TEST_EXECUTABLE
      "${options}" "${oneValueArgs}" "${multiValueArgs}"
      ${ARGN}
    )

    assert_valid_test_group_name("${ADD_LZT_TEST_EXECUTABLE_GROUP}")

    is_test_group_member("${GROUP}" "${ADD_LZT_TEST_EXECUTABLE_GROUP}" in_scope)
    if(NOT in_scope)
        return()
    endif()

    is_test_group_member("/conformance_tests" "${ADD_LZT_TEST_EXECUTABLE_GROUP}" is_conformance_test)
    is_test_group_member("/perf_tests" "${ADD_LZT_TEST_EXECUTABLE_GROUP}" is_perf_test)
    if(is_conformance_test)
        list(APPEND ADD_LZT_TEST_EXECUTABLE_LINK_LIBRARIES
          level_zero_tests::test_harness
          GMock::GMock
          GTest::GTest
        )
        set(component "conformance-tests")
    elseif(is_perf_test)
        list(APPEND ADD_LZT_TEST_EXECUTABLE_INCLUDE_DIRECTORIES
          ${CMAKE_SOURCE_DIR}/perf_tests/common/include
        )
        set(component "perf-tests")
    endif()

    add_executable(${ADD_LZT_TEST_EXECUTABLE_NAME} ${ADD_LZT_TEST_EXECUTABLE_SOURCES})

    if(CLANG_TIDY_EXE)
        set_target_properties(${ADD_LZT_TEST_EXECUTABLE_NAME}
        PROPERTIES
          CXX_CLANG_TIDY ${CLANG_TIDY_EXE}
        )
    endif()

    target_link_libraries(${ADD_LZT_TEST_EXECUTABLE_NAME}
      PRIVATE
        LevelZero::LevelZero
        ${ADD_LZT_TEST_EXECUTABLE_LINK_LIBRARIES}
    )

    target_include_directories(${ADD_LZT_TEST_EXECUTABLE_NAME}
      PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${ADD_LZT_TEST_EXECUTABLE_INCLUDE_DIRECTORIES}
    )

    set(destination "${CMAKE_INSTALL_PREFIX}/${ADD_LZT_TEST_EXECUTABLE_PREFIX}")

    foreach(kernel ${ADD_LZT_TEST_EXECUTABLE_KERNELS})
        install(
          FILES "${CMAKE_CURRENT_SOURCE_DIR}/kernels/${kernel}.spv"
          DESTINATION ${destination}
        )
    endforeach()

    foreach(media ${ADD_LZT_TEST_EXECUTABLE_MEDIA})
        install(
          FILES "${MEDIA_DIRECTORY}/${media}"
          DESTINATION ${destination}
        )
    endforeach()

    install(
      TARGETS ${ADD_LZT_TEST_EXECUTABLE_NAME}
      DESTINATION ${destination}
      COMPONENT ${component}
    )

    if(MSVC)
        set_target_properties(${ADD_LZT_TEST_EXECUTABLE_NAME}
          PROPERTIES
            VS_DEBUGGER_COMMAND_ARGUMENTS ""
            VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
        )
    endif()
endfunction()

function(add_lzt_test)
    set(options REQUIRES_OPENCL_INTEROP)
    set(oneValueArgs NAME GROUP)
    set(multiValueArgs SOURCES INCLUDE_DIRECTORIES LINK_LIBRARIES KERNELS MEDIA)
    cmake_parse_arguments(ADD_LZT_TEST
      "${options}" "${oneValueArgs}" "${multiValueArgs}"
      ${ARGN}
    )

    assert_valid_test_group_name("${ADD_LZT_TEST_GROUP}")

    is_test_group_member("${GROUP}" "${ADD_LZT_TEST_GROUP}" in_scope)
    if(NOT in_scope)
        message(STATUS "Ignoring ${ADD_LZT_TEST_NAME} because it is not in ${GROUP}")
        return()
    else()
        message(STATUS "Adding ${ADD_LZT_TEST_NAME}")
    endif()

    if(
      ADD_LZT_TEST_REQUIRES_OPENCL_INTEROP AND
      NOT LevelZero_OpenCL_INTEROP
    )
        message(STATUS "Disabling build of ${ADD_LZT_TEST_NAME} since Level-Zero OpenCL interop is not available")
        return()
    endif()

    add_lzt_test_executable(
      NAME ${ADD_LZT_TEST_NAME}
      GROUP ${ADD_LZT_TEST_GROUP}
      SOURCES ${ADD_LZT_TEST_SOURCES}
      INCLUDE_DIRECTORIES ${ADD_LZT_TEST_INCLUDE_DIRECTORIES}
      LINK_LIBRARIES ${ADD_LZT_TEST_LINK_LIBRARIES}
      KERNELS ${ADD_LZT_TEST_KERNELS}
      MEDIA ${ADD_LZT_TEST_MEDIA}
    )

    file(APPEND
      "${CMAKE_INSTALL_PREFIX}/manifest.txt"
      "${ADD_LZT_TEST_NAME}${CMAKE_NEWLINE}"
    )
endfunction()

if(WIN32)
    set(CMAKE_NEWLINE "\r\n")
else()
    set(CMAKE_NEWLINE "\n")
endif()

add_subdirectory(utils)
add_subdirectory(perf_tests)
add_subdirectory(conformance_tests)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${CMAKE_BINARY_DIR}/out")
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "intel-level-zero")
set(CPACK_PACKAGE_VENDOR "Intel Corporation")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Conformance tests & Performance tests and benchmarks for level-zero.")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
# set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
# set(CPACK_COMPONENTS_ALL conformance-tests perf-tests)
# set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
# set(CPACK_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-tests-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-Source")
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
    /.git/
    /.gitignore
    /build/
    /manifests/
    /.irepo/
    /nbproject
    /CONTRIBUTING.md
    /MAINTAINERS.md
    /dependencies.yml
    /docker
    /ci/
)
include(CPack)
