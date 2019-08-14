find_program(CLANG_FORMAT NAMES clang-format-7)
if (CLANG_FORMAT)
    MESSAGE(STATUS "clang format" ${CLANG_FORMAT})
    add_custom_target(clang-format
      COMMENT "Checking code formatting and fixing issues"
      COMMAND
        ${CMAKE_SOURCE_DIR}/clang-format-patch.sh ${CMAKE_SOURCE_DIR} |
        git -C ${CMAKE_SOURCE_DIR} apply -
    )
    add_custom_target(clang-format-check
      COMMENT "Checking code formatting"
      COMMAND
        ${CMAKE_SOURCE_DIR}/clang-format-patch.sh ${CMAKE_SOURCE_DIR}
        | tee ${CMAKE_BINARY_DIR}/clang_format_results.patch
      COMMAND ! [ -s ${CMAKE_BINARY_DIR}/clang_format_results.patch ]
    )
endif ()

if(DEFINED ENV{CLANG_TIDY_CHECK})
    find_program(CLANG_TIDY_EXE
      NAMES "clang-tidy" "clang-tidy-6.0"
      DOC "Path to clang-tidy executable"
    )
    if(NOT CLANG_TIDY_EXE)
        message(STATUS "clang-tidy not found.")
    else()
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()
endif()
