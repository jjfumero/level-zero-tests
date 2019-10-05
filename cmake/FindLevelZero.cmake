include(FindPackageHandleStandardArgs)

find_path(LevelZero_INCLUDE_DIR
  NAMES ze_api.h
  PATHS
    ${L0_ROOT}
    "/usr/local/"
  PATH_SUFFIXES "include" "include/level_zero"
  NO_DEFAULT_PATH
)

# If OpenCL is available, we can enable interop support
set(LevelZero_OpenCL_INTEROP FALSE)
set(CMAKE_PREFIX_PATH "${OPENCL_ROOT}")
find_package(OpenCL)
if (OpenCL_FOUND)
    set(LevelZero_OpenCL_FOUND TRUE)
endif()

find_library(LevelZero_LIBRARY
  NAMES level_zero level_zero32 level_zero64
  PATHS
    ${L0_ROOT}
    "/usr/local/"
  PATH_SUFFIXES "lib" "lib/level_zero/"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(LevelZero
  REQUIRED_VARS
    LevelZero_INCLUDE_DIR
    LevelZero_LIBRARY
  HANDLE_COMPONENTS
)
mark_as_advanced(LevelZero_LIBRARY LevelZero_INCLUDE_DIR)

if(LevelZero_FOUND)
    list(APPEND LevelZero_LIBRARIES ${LevelZero_LIBRARY})
    list(APPEND LevelZero_INCLUDE_DIRS ${LevelZero_INCLUDE_DIR})
    if(OpenCL_FOUND)
        list(APPEND LevelZero_INCLUDE_DIRS ${OpenCL_INCLUDE_DIRS})
    endif()
endif()

if(LevelZero_FOUND AND NOT TARGET LevelZero::LevelZero)
    add_library(LevelZero::LevelZero INTERFACE IMPORTED)
    set_target_properties(LevelZero::LevelZero
      PROPERTIES INTERFACE_LINK_LIBRARIES "${LevelZero_LIBRARIES}"
    )
    set_target_properties(LevelZero::LevelZero
      PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${LevelZero_INCLUDE_DIRS}"
    )
endif()

if (LevelZero_OpenCL_FOUND)
    set(LevelZero_OpenCL_INTEROP TRUE)
    set_target_properties(OpenCL::OpenCL
      PROPERTIES INTERFACE_COMPILE_DEFINITIONS CL_TARGET_OPENCL_VERSION=210
    )
    set_target_properties(LevelZero::LevelZero
      PROPERTIES INTERFACE_COMPILE_DEFINITIONS ZE_ENABLE_OCL_INTEROP
    )
    message(STATUS "Found OpenCL, enabling Level-Zero OpenCL interop support")
endif()


MESSAGE(STATUS "LevelZero_LIBRARIES: " ${LevelZero_LIBRARIES})
MESSAGE(STATUS "LevelZero_INCLUDE_DIRS: " ${LevelZero_INCLUDE_DIRS})
