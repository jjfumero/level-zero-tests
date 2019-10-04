/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2016 - 2019 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
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

#include "benchmark.hpp"
#include "gmock/gmock.h"

#include <iomanip>

using namespace ze_api_benchmarks;

namespace {
class XeNano : public ::testing::Test {
protected:
  XeNano() {
    api_static_probe_init();
    benchmark = new XeApp("xe_nano_benchmarks.spv");
    benchmark->singleDeviceInit();
    probe_setting.warm_up_iteration = 0;
    probe_setting.measure_iteration = 0;
  }

  ~XeNano() override {
    api_static_probe_cleanup();
    benchmark->singleDeviceCleanup();
    delete benchmark;
  }

  void header_print_iteration(std::string prefix,
                              probe_config_t &probe_setting) {
    std::cout << " All measurements are averaged per call except the function "
                 "call rate metric"
              << std::endl;
    std::cout << std::left << std::setw(25) << " " + prefix << std::internal
              << "Warm up iterations " << probe_setting.warm_up_iteration
              << std::setw(30) << " Measured iterations "
              << probe_setting.measure_iteration << std::endl;
  }

  XeApp *benchmark;
  probe_config_t probe_setting;
};

TEST_F(XeNano, zeKernelSetArgumentValue_Buffer) {
  probe_setting.warm_up_iteration = 1000;
  probe_setting.measure_iteration = 9000;

  header_print_iteration("Buffer argument", probe_setting);
  latency::parameter_buffer(benchmark, probe_setting);
  hardware_counter::parameter_buffer(benchmark, probe_setting);
  fuction_call_rate::parameter_buffer(benchmark, probe_setting);
  std::cout << std::endl;
}

TEST_F(XeNano, zeKernelSetArgumentValue_Immediate) {
  probe_setting.warm_up_iteration = 1000;
  probe_setting.measure_iteration = 9000;

  header_print_iteration("Immediate argument", probe_setting);
  latency::parameter_integer(benchmark, probe_setting);
  hardware_counter::parameter_integer(benchmark, probe_setting);
  fuction_call_rate::parameter_integer(benchmark, probe_setting);
  std::cout << std::endl;
}

TEST_F(XeNano, zeKernelSetArgumentValue_Image) {
  probe_setting.warm_up_iteration = 1000;
  probe_setting.measure_iteration = 9000;

  header_print_iteration("Image argument", probe_setting);
  latency::parameter_image(benchmark, probe_setting);
  hardware_counter::parameter_image(benchmark, probe_setting);
  fuction_call_rate::parameter_image(benchmark, probe_setting);
  std::cout << std::endl;
}

TEST_F(XeNano, zeCommandListAppendLaunchKernel) {
  probe_setting.warm_up_iteration = 500;
  probe_setting.measure_iteration = 2500;

  header_print_iteration("", probe_setting);
  latency::launch_function_no_parameter(benchmark, probe_setting);
  hardware_counter::launch_function_no_parameter(benchmark, probe_setting);
  std::cout << std::endl;
}

TEST_F(XeNano, zeCommandQueueExecuteCommandLists) {
  probe_setting.warm_up_iteration = 5;
  probe_setting.measure_iteration = 10;
  header_print_iteration("", probe_setting);
  latency::command_list_empty_execute(benchmark, probe_setting);
  hardware_counter::command_list_empty_execute(benchmark, probe_setting);
  fuction_call_rate::command_list_empty_execute(benchmark, probe_setting);
  std::cout << std::endl;
}

TEST_F(XeNano, zeDeviceGroupGetMemIpcHandle) {
  probe_setting.warm_up_iteration = 1000;
  probe_setting.measure_iteration = 9000;
  header_print_iteration("", probe_setting);
  latency::ipc_memory_handle_get(benchmark, probe_setting);
  hardware_counter::ipc_memory_handle_get(benchmark, probe_setting);
  fuction_call_rate::ipc_memory_handle_get(benchmark, probe_setting);
  std::cout << std::endl;
}
} /* end namespace */

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
