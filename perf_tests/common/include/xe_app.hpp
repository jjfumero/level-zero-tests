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

#ifndef _XE_APP_HPP_
#define _XE_APP_HPP_

#include "xe_api.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class XeApp {
public:
  XeApp(std::string module_path);
  ~XeApp();

  void memoryAlloc(size_t size, void **ptr);
  void memoryAlloc(xe_device_group_handle_t device_group,
                   xe_device_handle_t device, size_t size, void **ptr);
  void memoryFree(const void *ptr);
  void memoryFree(xe_device_group_handle_t device_group, const void *ptr);
  void functionCreate(xe_function_handle_t *function,
                      const char *pFunctionName);
  void functionCreate(xe_module_handle_t module, xe_function_handle_t *function,
                      const char *pFunctionName);
  void functionDestroy(xe_function_handle_t function);
  void imageCreate(xe_image_handle_t *image);
  void imageCreate(xe_device_handle_t device, xe_image_handle_t *image);
  void imageDestroy(xe_image_handle_t image);
  void commandListCreate(xe_command_list_handle_t *phCommandList);
  void commandListCreate(xe_device_handle_t device,
                         xe_command_list_handle_t *phCommandList);
  void commandListDestroy(xe_command_list_handle_t phCommandList);
  void commandListClose(xe_command_list_handle_t phCommandList);

  void commandQueueCreate(const uint32_t command_queue_id,
                          xe_command_queue_handle_t *command_queue);
  void commandQueueCreate(xe_device_handle_t device,
                          const uint32_t command_queue_id,
                          xe_command_queue_handle_t *command_queue);
  void commandQueueDestroy(xe_command_queue_handle_t command_queue);
  void commandQueueExecuteCommandList(xe_command_queue_handle_t command_queue,
                                      uint32_t numCommandLists,
                                      xe_command_list_handle_t *command_lists);
  void commandQueueSynchronize(xe_command_queue_handle_t command_queue);
  void moduleCreate(xe_device_handle_t device, xe_module_handle_t *module);
  void moduleDestroy(xe_module_handle_t module);

  void singleDeviceInit(void);
  void singleDeviceCleanup(void);

  uint32_t deviceGroupCount(void);
  void deviceGroupGet(uint32_t *device_group_count,
                      xe_device_group_handle_t *device_group);
  void deviceGroupGetDevices(xe_device_group_handle_t device_group,
                             uint32_t device_count,
                             xe_device_handle_t *devices);
  uint32_t deviceCount(xe_device_group_handle_t device_group);

  xe_device_group_handle_t device_group;
  xe_device_handle_t device;
  xe_module_handle_t module;

private:
  std::string module_path;
  std::vector<uint8_t> binary_file;
  std::vector<uint8_t> load_binary_file(const std::string &file_path);
};
#endif /* _XE_APP_HPP_*/
