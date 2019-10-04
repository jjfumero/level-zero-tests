/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2016 - 2019 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to the
 * source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 */

#ifndef XE_PEAK_H
#define XE_PEAK_H

#include "../include/common.h"

/* xe includes */
#include "ze_api.h"

#define MIN(X, Y) (X < Y) ? X : Y

#undef FETCH_2
#undef FETCH_8

#define FETCH_2(sum, id, A, jumpBy) \
    sum += A[id];                   \
    id += jumpBy;                   \
    sum += A[id];                   \
    id += jumpBy;
#define FETCH_4(sum, id, A, jumpBy) \
    FETCH_2(sum, id, A, jumpBy);    \
    FETCH_2(sum, id, A, jumpBy);
#define FETCH_8(sum, id, A, jumpBy) \
    FETCH_4(sum, id, A, jumpBy);    \
    FETCH_4(sum, id, A, jumpBy);

#define FETCH_PER_WI 16

enum class TimingMeasurement {
    BANDWIDTH = 0,
    BANDWIDTH_EVENT_TIMING,
    KERNEL_LAUNCH_LATENCY,
    KERNEL_COMPLETE_LATENCY
};

struct L0Context {
    ze_command_queue_handle_t command_queue = nullptr;
    ze_command_list_handle_t command_list = nullptr;
    ze_module_handle_t module = nullptr;
    ze_driver_handle_t driver = nullptr;
    ze_device_handle_t device = nullptr;
    uint32_t device_count = 0;
    const uint32_t default_device = 0;
    const uint32_t command_queue_id = 0;
    ze_device_properties_t device_property;
    ze_device_compute_properties_t device_compute_property;
    bool verbose = false;

    void init_xe();
    void clean_xe();
    void print_ze_device_properties(const ze_device_properties_t &props);
    void reset_commandlist();
    void execute_commandlist_and_sync();
    std::vector<uint8_t> load_binary_file(const std::string &file_path);
    void create_module(std::vector<uint8_t> binary_file);
};

struct XeWorkGroups {
    ze_thread_group_dimensions_t thread_group_dimensions;
    uint32_t group_size_x;
    uint32_t group_size_y;
    uint32_t group_size_z;
};

class XePeak {
  public:
    bool use_event_timer = false;
    bool verbose = false;
    bool run_global_bw = true;
    bool run_hp_compute = true;
    bool run_sp_compute = true;
    bool run_dp_compute = true;
    bool run_int_compute = true;
    bool run_transfer_bw = true;
    bool run_kernel_lat = true;
    int specified_platform, specified_device;
    uint32_t global_bw_max_size = 1 << 29;
    uint32_t transfer_bw_max_size = 1 << 29;
    uint32_t iters = 50;
    uint32_t warmup_iterations = 10;

    int parse_arguments(int argc, char **argv);

    /* Helper Functions */
    float run_kernel(L0Context context, ze_kernel_handle_t &function,
                     struct XeWorkGroups &workgroup_info, TimingMeasurement type, bool reset_command_list = true);
    uint64_t set_workgroups(L0Context &context,
                            const uint64_t total_work_items_requested, struct XeWorkGroups *workgroup_info);
    void setup_function(L0Context &context, ze_kernel_handle_t &function,
                        const char *name, void *input, void *output,
                        size_t outputSize = 0u);
    uint64_t get_max_work_items(L0Context &context);
    void print_test_complete();
    void run_command_queue(L0Context &context);
    void synchronize_command_queue(L0Context &context);
    /* Benchmark Functions*/
    void xe_peak_global_bw(L0Context &context);
    void xe_peak_kernel_latency(L0Context &context);
    void xe_peak_hp_compute(L0Context &context);
    void xe_peak_sp_compute(L0Context &context);
    void xe_peak_dp_compute(L0Context &context);
    void xe_peak_int_compute(L0Context &context);
    void xe_peak_transfer_bw(L0Context &context);

  private:
    void _transfer_bw_gpu_copy(L0Context &context, void *destination_buffer,
                               void *source_buffer, size_t buffer_size);
    void _transfer_bw_host_copy(void *destination_buffer, void *source_buffer,
                                size_t buffer_size);
    void _transfer_bw_shared_memory(L0Context &context,
                                    std::vector<float> local_memory);
    TimingMeasurement is_bandwidth_with_event_timer(void);
};

uint64_t max_device_object_size(L0Context &context);
TimingMeasurement is_bandwidth_with_event_timer(void);

#endif /* XE_PEAK_H */
