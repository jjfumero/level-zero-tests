cmake_minimum_required(VERSION 3.8.0 FATAL_ERROR)
project(intel-level-zero-tests
    VERSION 0.2.1  # corresponds to level-zero release
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(clang_tools)
include(custom_functions)

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

find_package(LevelZero REQUIRED)

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

if(BOOST_LIB_TOOLSET)
    add_definitions(-DBOOST_LIB_TOOLSET="${BOOST_LIB_TOOLSET}")
endif()
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.65 REQUIRED COMPONENTS log program_options timer chrono system)

# test_xe_cl_interop requires OpenCL
# FindOpenCL doesn't listen to OPENCL_ROOT, so use CMAKE_PREFIX_PATH for it
set(CMAKE_PREFIX_PATH "${OPENCL_ROOT}")
find_package(OpenCL REQUIRED)
set_target_properties(OpenCL::OpenCL
  PROPERTIES INTERFACE_COMPILE_DEFINITIONS CL_TARGET_OPENCL_VERSION=210
)

set(MEDIA_ROOT_DIRECTORY "${CMAKE_SOURCE_DIR}/mediadata")
set(MEDIA_DIRECTORY "${MEDIA_ROOT_DIRECTORY}/merged")
set(MEDIADATA_ROOT "${MEDIA_ROOT_DIRECTORY}/external")
file(COPY "${MEDIADATA_ROOT}/yuv" DESTINATION "${MEDIA_DIRECTORY}")
file(COPY "${MEDIA_ROOT_DIRECTORY}/internal/yuv" DESTINATION "${MEDIA_DIRECTORY}")
file(COPY "${MEDIA_ROOT_DIRECTORY}/internal/bmp" DESTINATION "${MEDIA_DIRECTORY}")
file(COPY "${MEDIA_ROOT_DIRECTORY}/internal/png" DESTINATION "${MEDIA_DIRECTORY}")

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
