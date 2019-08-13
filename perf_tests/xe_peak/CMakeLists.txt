if(UNIX)
    set(OS_SPECIFIC_LIBS pthread)
else()
    set(OS_SPECIFIC_LIBS "")
endif()

set(CMAKE_CXX_STANDARD "11")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(XE_PEAK_COMMON src/common.cpp src/options.cpp src/xe_peak.cpp)
set(XE_PEAK_BENCHMARKS
    src/global_bw.cpp
    src/kernel_latency.cpp
    src/hp_compute.cpp
    src/sp_compute.cpp
    src/integer_compute.cpp
    src/dp_compute.cpp
    src/transfer_bw.cpp
)

add_executable(xe_peak ${XE_PEAK_COMMON} ${XE_PEAK_BENCHMARKS})

add_performance_application(xe_peak)

install_resources_performance(xe_peak
    FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/kernels/xe_global_bw.spv"
    "${CMAKE_CURRENT_SOURCE_DIR}/kernels/xe_hp_compute.spv"
    "${CMAKE_CURRENT_SOURCE_DIR}/kernels/xe_sp_compute.spv"
    "${CMAKE_CURRENT_SOURCE_DIR}/kernels/xe_int_compute.spv"
    "${CMAKE_CURRENT_SOURCE_DIR}/kernels/xe_dp_compute.spv"
)