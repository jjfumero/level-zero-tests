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

#ifndef _ZE_APP_HPP_
#define _ZE_APP_HPP_

#include "ze_api.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class XeApp {
public:
  XeApp(void);
  XeApp(std::string module_path);
  ~XeApp();

  void memoryAlloc(size_t size, void **ptr);
  void memoryAlloc(ze_driver_handle_t driver, ze_device_handle_t device,
                   size_t size, void **ptr);
  void memoryFree(const void *ptr);
  void memoryFree(ze_driver_handle_t driver, const void *ptr);
  void functionCreate(ze_kernel_handle_t *function, const char *pFunctionName);
  void functionCreate(ze_module_handle_t module, ze_kernel_handle_t *function,
                      const char *pFunctionName);
  void functionDestroy(ze_kernel_handle_t function);
  void imageCreate(ze_image_handle_t *image);
  void imageCreate(ze_device_handle_t device, ze_image_handle_t *image);
  void imageCreate(ze_image_handle_t *image, uint32_t width, uint32_t height,
                   uint32_t depth);
  void imageDestroy(ze_image_handle_t image);
  void commandListCreate(ze_command_list_handle_t *phCommandList);
  void commandListCreate(ze_device_handle_t device,
                         ze_command_list_handle_t *phCommandList);
  void commandListDestroy(ze_command_list_handle_t phCommandList);
  void commandListClose(ze_command_list_handle_t phCommandList);
  void commandListReset(ze_command_list_handle_t phCommandList);
  void commandListAppendImageCopyFromMemory(
      ze_command_list_handle_t command_list, ze_image_handle_t image,
      uint8_t *srcBuffer, ze_image_region_t *Region);

  void commandListAppendBarrier(ze_command_list_handle_t command_list);

  void commandListAppendImageCopyToMemory(ze_command_list_handle_t command_list,
                                          uint8_t *dstBuffer,
                                          ze_image_handle_t image,
                                          ze_image_region_t *Region);

  void commandQueueCreate(const uint32_t command_queue_id,
                          ze_command_queue_handle_t *command_queue);
  void commandQueueCreate(ze_device_handle_t device,
                          const uint32_t command_queue_id,
                          ze_command_queue_handle_t *command_queue);
  void commandQueueDestroy(ze_command_queue_handle_t command_queue);
  void commandQueueExecuteCommandList(ze_command_queue_handle_t command_queue,
                                      uint32_t numCommandLists,
                                      ze_command_list_handle_t *command_lists);
  void commandQueueSynchronize(ze_command_queue_handle_t command_queue);
  void moduleCreate(ze_device_handle_t device, ze_module_handle_t *module);
  void moduleDestroy(ze_module_handle_t module);

  void singleDeviceInit(void);
  void singleDeviceCleanup(void);

  uint32_t driverCount(void);
  void driverGet(uint32_t *driver_count, ze_driver_handle_t *driver);
  void driverGetDevices(ze_driver_handle_t driver, uint32_t device_count,
                        ze_device_handle_t *devices);
  uint32_t deviceCount(ze_driver_handle_t driver);

  ze_driver_handle_t driver;
  ze_device_handle_t device;
  ze_module_handle_t module;

private:
  std::string module_path;
  std::vector<uint8_t> binary_file;
  std::vector<uint8_t> load_binary_file(const std::string &file_path);
};
#endif /* _ZE_APP_HPP_*/
