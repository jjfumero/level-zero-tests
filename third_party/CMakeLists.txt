# Add googlemock support if not already added
if (NOT TARGET gmock)
    #Allow custom googlemock directory
    if (NOT DEFINED GOOGLE_MOCK_DIR)
        set(GOOGLE_MOCK_DIR ${CMAKE_SOURCE_DIR}/third_party/googletest/googlemock)
    endif()

    # Ensure googlemock exists
    if (EXISTS "${GOOGLE_MOCK_DIR}/include/gmock")
        message(STATUS "GOOGLE_MOCK_DIR: ${GOOGLE_MOCK_DIR}")
        set(GOOGLETEST_VERSION 1.9.0)
        add_subdirectory(${GOOGLE_MOCK_DIR} EXCLUDE_FROM_ALL)

    else()
        message(FATAL_ERROR "GOOGLE_MOCK_DIR: NOT FOUND.")
    endif()
endif()

