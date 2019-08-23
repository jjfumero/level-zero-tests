find_program(CLANG_FORMAT NAMES clang-format-7)
if(CLANG_FORMAT)
    message(STATUS "Found clang-format-7: ${CLANG_FORMAT}")
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
else()
    message(WARNING "clang-format-7 not found. clang-format and clang-format-check targets will be disabled.")
endif()

option(CLANG_TIDY_CHECK "Include clang-tidy code checking as part of the build")
if(CLANG_TIDY_CHECK)
    find_program(CLANG_TIDY_EXE
      NAMES "clang-tidy" "clang-tidy-6.0"
      DOC "Path to clang-tidy executable"
    )
    if(NOT CLANG_TIDY_EXE)
        message(STATUS "clang-tidy not found.")
    else()
        message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
        set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()
endif()
