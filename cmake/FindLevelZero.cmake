include(FindPackageHandleStandardArgs)

find_path(LevelZero_INCLUDE_DIR
  NAMES xe_api.h
  PATHS
    ${L0_ROOT}
    "/usr/local/"
  PATH_SUFFIXES "include" "include/level_zero"
  NO_DEFAULT_PATH
)

# Level-Zero headers depend on the OpenCL headers... temporary
find_path(OpenCLHeaders_INCLUDE_DIR
  NAMES CL/cl.h
  PATHS "${OPENCL_ROOT}" "/usr"
  PATH_SUFFIXES "include"
  NO_DEFAULT_PATH
)

find_library(LevelZero_LIBRARY
  NAMES level_zero level_zero32 level_zero64
  PATHS
    ${L0_ROOT}
    "/usr/local/"
  PATH_SUFFIXES "lib" "lib/level_zero/"
  NO_DEFAULT_PATH
)

find_package_handle_standard_args(LevelZero
  DEFAULT_MSG
    LevelZero_INCLUDE_DIR
    OpenCLHeaders_INCLUDE_DIR
    LevelZero_LIBRARY
)

mark_as_advanced(LevelZero_LIBRARY LevelZero_INCLUDE_DIR OpenCLHeaders_INCLUDE_DIR)

if(LevelZero_FOUND)
    set(LevelZero_LIBRARIES ${LevelZero_LIBRARY})
    set(LevelZero_INCLUDE_DIRS ${LevelZero_INCLUDE_DIR} ${OpenCLHeaders_INCLUDE_DIR})
endif()

MESSAGE(STATUS "LevelZero_LIBRARIES: " ${LevelZero_LIBRARY})
MESSAGE(STATUS "LevelZero_INCLUDE_DIRS: " ${LevelZero_INCLUDE_DIR})
MESSAGE(STATUS "OpenCLHeaders_INCLUDE_DIRS: " ${OpenCLHeaders_INCLUDE_DIR})
