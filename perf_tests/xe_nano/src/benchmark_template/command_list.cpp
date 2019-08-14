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

void launch_function_no_parameter(XeApp *benchmark,
                                  probe_config_t &probe_setting) {
  xe_function_handle_t function;
  xe_command_list_handle_t command_list;
  benchmark->commandListCreate(&command_list);

  benchmark->functionCreate(&function, "function_no_parameter");

  xe_thread_group_dimensions_t thread_group_dimensions;
  thread_group_dimensions.groupCountX = 1;
  thread_group_dimensions.groupCountY = 1;
  thread_group_dimensions.groupCountZ = 1;

  /* Warm up */
  for (int i = 0; i < probe_setting.warm_up_iteration; i++) {
    xeCommandListAppendLaunchFunction(
        command_list, function, &thread_group_dimensions, nullptr, 0, nullptr);
  }

  NANO_PROBE(" Function with no parameters\t", probe_setting,
             xeCommandListAppendLaunchFunction, command_list, function,
             &thread_group_dimensions, nullptr, 0, nullptr);

  benchmark->functionDestroy(function);
  benchmark->commandListDestroy(command_list);
}

void command_list_empty_execute(XeApp *benchmark,
                                probe_config_t &probe_setting) {
  xe_command_list_handle_t command_list;
  xe_command_queue_handle_t command_queue;

  benchmark->commandQueueCreate(0, /*command_queue_id */
                                &command_queue);
  benchmark->commandListCreate(&command_list);
  benchmark->commandListClose(command_list);

  /* Warm up */
  for (int i = 0; i < probe_setting.warm_up_iteration; i++) {
    xeCommandQueueExecuteCommandLists(command_queue, 1, &command_list, nullptr);
  }

  NANO_PROBE(" Empty command list\t", probe_setting,
             xeCommandQueueExecuteCommandLists, command_queue, 1, &command_list,
             nullptr);

  benchmark->commandListDestroy(command_list);
  benchmark->commandQueueDestroy(command_queue);
}
