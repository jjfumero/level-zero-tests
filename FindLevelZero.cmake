include(FindPackageHandleStandardArgs)

find_path(LevelZero_INCLUDE_DIR
    NAMES xe_api.h
    HINTS "/usr/local/include/level_zero"
)

find_library(LevelZero_LIBRARY level_zero
    HINTS "/usr/local/lib/level_zero"  # Workaround for LevelZero not installing any ldconfig files
)

find_package_handle_standard_args(LevelZero
 DEFAULT_MSG
  LevelZero_INCLUDE_DIR
  LevelZero_LIBRARY
)

mark_as_advanced(LevelZero_LIBRARY LevelZero_INCLUDE_DIR)

if(LevelZero_FOUND)
  set(LevelZero_LIBRARIES    ${LevelZero_LIBRARY})
  set(LevelZero_INCLUDE_DIRS ${LevelZero_INCLUDE_DIR})
endif()

MESSAGE(STATUS "LevelZero_LIBRARIES: " ${LevelZero_LIBRARY})
MESSAGE(STATUS "LevelZero_INCLUDE_DIRS: " ${LevelZero_INCLUDE_DIR})