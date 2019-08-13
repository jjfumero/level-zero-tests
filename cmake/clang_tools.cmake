file(GLOB_RECURSE CONFORMANCE_SOURCE_FILES conformance_tests/*.cpp)
file(GLOB_RECURSE CONFORMANCE_HEADER_FILES conformance_tests/*.hpp)
file(GLOB_RECURSE PERF_SOURCE_FILES perf_tests/*.cpp)
file(GLOB_RECURSE PERF_HEADER_FILES perf_tests/*.hpp)
file(GLOB_RECURSE UTILS_SOURCE_FILES utils/*.cpp)
file(GLOB_RECURSE UTILS_HEADER_FILES utils/*.hpp)
set(ALL_SOURCE_FILES
  ${CONFORMANCE_SOURCE_FILES}
  ${PERF_SOURCE_FILES}
  ${UTILS_SOURCE_FILES}
)
set(ALL_HEADER_FILES
  ${CONFORMANCE_HEADER_FILES}
  ${PERF_HEADER_FILES}
  ${UTILS_HEADER_FILES}
)
set(ALL_TEST_FILES ${ALL_SOURCE_FILES} ${ALL_HEADER_FILES})

find_program(CLANG_FORMAT NAMES clang-format-7)
if (CLANG_FORMAT)
    MESSAGE(STATUS "clang format" ${CLANG_FORMAT})
    add_custom_target(clang-format
      COMMAND ${CLANG_FORMAT}
      -style=file
      -i
      ${ALL_TEST_FILES}
    )
    add_custom_target(clang-format-check
      COMMENT "Checking format compliance"
      COMMAND ${CLANG_FORMAT}
      -style=file
      --output-replacements-xml
      ${ALL_TEST_FILES} > ${CMAKE_BINARY_DIR}/clang_format_results
      COMMAND
        echo "Clang-Format returned Changes Required# ";
        `grep -c "replacement offset" ${CMAKE_BINARY_DIR}/clang_format_results`
      COMMAND
        ! grep -c "replacement offset"
        ${CMAKE_BINARY_DIR}/clang_format_results > /dev/null
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
