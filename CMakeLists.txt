cmake_minimum_required(VERSION 3.8.0 FATAL_ERROR)
project(intel-level-zero-tests
    VERSION 0.2.1  # corresponds to level-zero release
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(clang_tools)
include(custom_functions)

set(THIRD_PARTY_DIRECTORY "${CMAKE_SOURCE_DIR}/third_party")

set(MEDIA_ROOT_DIRECTORY "${CMAKE_SOURCE_DIR}/mediadata")
set(MEDIA_DIRECTORY "${MEDIA_ROOT_DIRECTORY}/merged")
if(NOT DEFINED MEDIADATA_ROOT)
    set(MEDIADATA_ROOT "${MEDIA_ROOT_DIRECTORY}/external")
endif()
file(COPY "${MEDIADATA_ROOT}/yuv" DESTINATION "${MEDIA_DIRECTORY}")
file(COPY "${MEDIA_ROOT_DIRECTORY}/internal/yuv" DESTINATION "${MEDIA_DIRECTORY}")
file(COPY "${MEDIA_ROOT_DIRECTORY}/internal/bmp" DESTINATION "${MEDIA_DIRECTORY}")
file(COPY "${MEDIA_ROOT_DIRECTORY}/internal/png" DESTINATION "${MEDIA_DIRECTORY}")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "Default installation directory" FORCE)
endif()

if(NOT TARGET GTest::GTest AND NOT TARGET GMock::GMock)
    if(NOT DEFINED GTEST_ROOT)
        set(GTEST_ROOT "${THIRD_PARTY_DIRECTORY}/googletest")
    endif()
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory("${GTEST_ROOT}" "${GTEST_ROOT}/build" EXCLUDE_FROM_ALL)
    add_library(GTest::GTest ALIAS gtest)
    add_library(GMock::GMock ALIAS gmock)
endif()
enable_testing()

if(NOT TARGET Boost::boost)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    find_package(Boost 1.70 REQUIRED COMPONENTS log program_options timer chrono system)
endif()

if(NOT TARGET PNG::PNG)
    # Find static zlib
    find_library(ZLIB_LIBRARY
      NAMES libz.a zlibstatic zlibstaticd
      PATHS "${ZLIB_ROOT}"
      PATH_SUFFIXES lib
      NO_DEFAULT_PATH
    )
    find_path(ZLIB_INCLUDE_DIR
      NAMES zlib.h
      PATHS "${ZLIB_ROOT}"
      PATH_SUFFIXES include
      NO_DEFAULT_PATH
    )

    # Find static libpng
    find_library(PNG_LIBRARY
      NAMES libpng.a libpng16_static libpng16_staticd
      PATHS "${PNG_ROOT}"
      PATH_SUFFIXES lib
      NO_DEFAULT_PATH
    )
    find_path(PNG_INCLUDE_DIR
      NAMES png.h
      PATHS "${PNG_ROOT}"
      PATH_SUFFIXES include
      NO_DEFAULT_PATH
    )

    set(CMAKE_PREFIX_PATH "${PNG_ROOT}")
    find_package(PNG REQUIRED)
endif()

# test_xe_cl_interop requires OpenCL
if(NOT TARGET OpenCL::OpenCL)
    set(CMAKE_PREFIX_PATH "${OPENCL_ROOT}")
    find_package(OpenCL REQUIRED)
    set_target_properties(OpenCL::OpenCL
      PROPERTIES INTERFACE_COMPILE_DEFINITIONS CL_TARGET_OPENCL_VERSION=210
    )
endif()

include(FindLevelZero)
find_package(LevelZero REQUIRED)

add_subdirectory(utils)
add_subdirectory(perf_tests)
add_subdirectory(conformance_tests)

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
    /ci-build-linux.sh
    /ci-build-windows.cmd
)
include(CPack)
