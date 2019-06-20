cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)
project(level_zero_tests)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

include(GNUInstallDirs)

add_subdirectory(third_party)
add_subdirectory(perf_tests)
