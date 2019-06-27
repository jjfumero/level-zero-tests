cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)
project(intel-loki-perftests
    VERSION 0.2.0  # corresponds to LOKI release
)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

find_package(LevelZero REQUIRED)

add_subdirectory(third_party)
add_subdirectory(perf_tests)

set(CPACK_PACKAGE_VENDOR "Intel Corporation")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Performance tests and benchmarks for level-zero.")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_GENERATOR "TGZ;ZIP")
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
    /linux.yml
    /docker
    /ci-build.sh
    /ci-env
)
include(CPack)
