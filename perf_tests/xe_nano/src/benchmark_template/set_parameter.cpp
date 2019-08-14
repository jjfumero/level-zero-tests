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

/*
 * If function signatures are updated, benchmark_template/set_parameter.hpp
 * needs to be updated.
 */
void parameter_buffer(XeApp *benchmark, probe_config_t &probe_setting) {
  xe_function_handle_t function;
  void *input_buffer = nullptr;
  const std::vector<int8_t> input = {72, 101, 108, 108, 111, 32,
                                     87, 111, 114, 108, 100, 33};
  benchmark->memoryAlloc(size_in_bytes(input), &input_buffer);

  benchmark->functionCreate(&function, "function_parameter_buffers");

  /* Warm up */
  for (int i = 0; i < probe_setting.warm_up_iteration; i++) {
    SUCCESS_OR_TERMINATE(xeFunctionSetArgumentValue(
        function, 0, sizeof(input_buffer), &input_buffer));
    SUCCESS_OR_TERMINATE(xeFunctionSetArgumentValue(
        function, 1, sizeof(input_buffer), &input_buffer));
    SUCCESS_OR_TERMINATE(xeFunctionSetArgumentValue(
        function, 2, sizeof(input_buffer), &input_buffer));
    SUCCESS_OR_TERMINATE(xeFunctionSetArgumentValue(
        function, 3, sizeof(input_buffer), &input_buffer));
    SUCCESS_OR_TERMINATE(xeFunctionSetArgumentValue(
        function, 4, sizeof(input_buffer), &input_buffer));
    SUCCESS_OR_TERMINATE(xeFunctionSetArgumentValue(
        function, 5, sizeof(input_buffer), &input_buffer));
  }

  NANO_PROBE(" Argument index 0\t", probe_setting, xeFunctionSetArgumentValue,
             function, 0, sizeof(input_buffer), &input_buffer);

  NANO_PROBE(" Argument index 5\t", probe_setting, xeFunctionSetArgumentValue,
             function, 5, sizeof(input_buffer), &input_buffer);

  benchmark->functionDestroy(function);
  benchmark->memoryFree(input_buffer);
}

void parameter_integer(XeApp *benchmark, probe_config_t &probe_setting) {
  xe_function_handle_t function;
  int input_a = 1;

  benchmark->functionCreate(&function, "function_parameter_integer");

  /* Warm up */
  for (int i = 0; i < probe_setting.warm_up_iteration; i++) {
    SUCCESS_OR_TERMINATE(
        xeFunctionSetArgumentValue(function, 0, sizeof(input_a), &input_a));
    SUCCESS_OR_TERMINATE(
        xeFunctionSetArgumentValue(function, 1, sizeof(input_a), &input_a));
    SUCCESS_OR_TERMINATE(
        xeFunctionSetArgumentValue(function, 2, sizeof(input_a), &input_a));
    SUCCESS_OR_TERMINATE(
        xeFunctionSetArgumentValue(function, 3, sizeof(input_a), &input_a));
    SUCCESS_OR_TERMINATE(
        xeFunctionSetArgumentValue(function, 4, sizeof(input_a), &input_a));
    SUCCESS_OR_TERMINATE(
        xeFunctionSetArgumentValue(function, 5, sizeof(input_a), &input_a));
  }

  NANO_PROBE(" Argument index 0\t", probe_setting, xeFunctionSetArgumentValue,
             function, 0, sizeof(input_a), &input_a);

  NANO_PROBE(" Argument index 5\t", probe_setting, xeFunctionSetArgumentValue,
             function, 5, sizeof(input_a), &input_a);

  benchmark->functionDestroy(function);
}

void parameter_image(XeApp *benchmark, probe_config_t &probe_setting) {
  xe_function_handle_t function;
  xe_image_handle_t input_a;

  benchmark->functionCreate(&function, "function_parameter_image");
  benchmark->imageCreate(&input_a);

  /* Warm up */
  for (int i = 0; i < probe_setting.warm_up_iteration; i++) {
    SUCCESS_OR_TERMINATE(
        xeFunctionSetArgumentValue(function, 0, sizeof(input_a), &input_a));
    SUCCESS_OR_TERMINATE(
        xeFunctionSetArgumentValue(function, 1, sizeof(input_a), &input_a));
    SUCCESS_OR_TERMINATE(
        xeFunctionSetArgumentValue(function, 2, sizeof(input_a), &input_a));
    SUCCESS_OR_TERMINATE(
        xeFunctionSetArgumentValue(function, 3, sizeof(input_a), &input_a));
    SUCCESS_OR_TERMINATE(
        xeFunctionSetArgumentValue(function, 4, sizeof(input_a), &input_a));
    SUCCESS_OR_TERMINATE(
        xeFunctionSetArgumentValue(function, 5, sizeof(input_a), &input_a));
  }

  NANO_PROBE(" Argument index 0\t", probe_setting, xeFunctionSetArgumentValue,
             function, 0, sizeof(input_a), &input_a);

  NANO_PROBE(" Argument index 5\t", probe_setting, xeFunctionSetArgumentValue,
             function, 5, sizeof(input_a), &input_a);

  benchmark->functionDestroy(function);
}
