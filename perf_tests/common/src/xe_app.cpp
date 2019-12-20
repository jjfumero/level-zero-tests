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

#include "xe_app.hpp"

#include "common.hpp"

#include <assert.h>

bool verbose = false;

std::vector<uint8_t> XeApp::load_binary_file(const std::string &file_path) {
  if (verbose)
    std::cout << "File path: " << file_path << std::endl;
  std::ifstream stream(file_path, std::ios::in | std::ios::binary);

  std::vector<uint8_t> binary_file;
  if (!stream.good()) {
    std::cerr << "Failed to load binary file: " << file_path;
    return binary_file;
  }

  size_t length = 0;
  stream.seekg(0, stream.end);
  length = static_cast<size_t>(stream.tellg());
  stream.seekg(0, stream.beg);
  if (verbose)
    std::cout << "Binary file length: " << length << std::endl;

  binary_file.resize(length);
  stream.read(reinterpret_cast<char *>(binary_file.data()), length);
  if (verbose)
    std::cout << "Binary file loaded" << std::endl;

  return binary_file;
}

XeApp::XeApp(void) {
  device = nullptr;
  module = nullptr;
  driver = nullptr;
  this->module_path = "";

  SUCCESS_OR_TERMINATE(zeInit(ZE_INIT_FLAG_NONE));
}

XeApp::XeApp(std::string module_path) {
  device = nullptr;
  module = nullptr;
  driver = nullptr;
  this->module_path = module_path;

  SUCCESS_OR_TERMINATE(zeInit(ZE_INIT_FLAG_NONE));

  binary_file = load_binary_file(this->module_path);
  std::cout << std::endl;
}

void XeApp::moduleCreate(ze_device_handle_t device,
                         ze_module_handle_t *module) {
  ze_module_desc_t module_description;
  module_description.version = ZE_MODULE_DESC_VERSION_CURRENT;
  module_description.format = ZE_MODULE_FORMAT_IL_SPIRV;
  module_description.inputSize = binary_file.size();
  module_description.pInputModule = binary_file.data();
  module_description.pBuildFlags = nullptr;

  SUCCESS_OR_TERMINATE(
      zeModuleCreate(device, &module_description, module, nullptr));
}

void XeApp::moduleDestroy(ze_module_handle_t module) {
  SUCCESS_OR_TERMINATE(zeModuleDestroy(module));
}

XeApp::~XeApp(void) {}

void XeApp::memoryAlloc(size_t size, void **ptr) {
  assert(this->device != nullptr);
  assert(this->driver != nullptr);
  memoryAlloc(this->driver, this->device, size, ptr);
}

void XeApp::memoryAlloc(ze_driver_handle_t driver, ze_device_handle_t device,
                        size_t size, void **ptr) {
  SUCCESS_OR_TERMINATE(zeDriverAllocDeviceMem(
      driver, device, ZE_DEVICE_MEM_ALLOC_FLAG_DEFAULT, 0, size, 1, ptr));
}

void XeApp::memoryAllocHost(size_t size, void **ptr) {
  assert(this->driver != nullptr);
  memoryAllocHost(this->driver, size, ptr);
}

void XeApp::memoryAllocHost(ze_driver_handle_t driver, size_t size,
                            void **ptr) {
  SUCCESS_OR_TERMINATE(zeDriverAllocHostMem(
      driver, ZE_HOST_MEM_ALLOC_FLAG_DEFAULT, size, 1, ptr));
}

void XeApp::memoryFree(const void *ptr) {
  assert(this->driver != nullptr);
  SUCCESS_OR_TERMINATE(zeDriverFreeMem(this->driver, const_cast<void *>(ptr)));
}

void XeApp::memoryFree(ze_driver_handle_t driver, const void *ptr) {
  SUCCESS_OR_TERMINATE(zeDriverFreeMem(driver, const_cast<void *>(ptr)));
}

void XeApp::functionCreate(ze_kernel_handle_t *function,
                           const char *pFunctionName) {
  assert(this->module != nullptr);
  functionCreate(module, function, pFunctionName);
}

void XeApp::functionCreate(ze_module_handle_t module,
                           ze_kernel_handle_t *function,
                           const char *pFunctionName) {
  ze_kernel_desc_t function_description;
  function_description.version = ZE_KERNEL_DESC_VERSION_CURRENT;
  function_description.flags = ZE_KERNEL_FLAG_NONE;
  function_description.pKernelName = pFunctionName;

  SUCCESS_OR_TERMINATE(zeKernelCreate(module, &function_description, function));
}

void XeApp::functionDestroy(ze_kernel_handle_t function) {
  SUCCESS_OR_TERMINATE(zeKernelDestroy(function));
}

void XeApp::imageCreate(ze_image_handle_t *image) {
  assert(this->device != nullptr);
  imageCreate(this->device, image);
}

void XeApp::imageCreate(ze_device_handle_t device, ze_image_handle_t *image) {
  ze_image_format_desc_t formatDesc = {
      ZE_IMAGE_FORMAT_LAYOUT_32, ZE_IMAGE_FORMAT_TYPE_FLOAT,
      ZE_IMAGE_FORMAT_SWIZZLE_R, ZE_IMAGE_FORMAT_SWIZZLE_0,
      ZE_IMAGE_FORMAT_SWIZZLE_0, ZE_IMAGE_FORMAT_SWIZZLE_1};

  ze_image_desc_t imageDesc = {ZE_IMAGE_DESC_VERSION_CURRENT,
                               ZE_IMAGE_FLAG_PROGRAM_READ,
                               ZE_IMAGE_TYPE_2D,
                               formatDesc,
                               128,
                               128,
                               0,
                               0,
                               0};

  zeImageCreate(device, &imageDesc, image);
}

void XeApp::imageCreate(ze_image_handle_t *image, uint32_t width,
                        uint32_t height, uint32_t depth) {

  assert(this->device != nullptr);
  ze_image_format_desc_t formatDesc = {
      ZE_IMAGE_FORMAT_LAYOUT_32, ZE_IMAGE_FORMAT_TYPE_FLOAT,
      ZE_IMAGE_FORMAT_SWIZZLE_R, ZE_IMAGE_FORMAT_SWIZZLE_0,
      ZE_IMAGE_FORMAT_SWIZZLE_0, ZE_IMAGE_FORMAT_SWIZZLE_1};

  ze_image_desc_t imageDesc = {ZE_IMAGE_DESC_VERSION_CURRENT,
                               ZE_IMAGE_FLAG_PROGRAM_READ,
                               ZE_IMAGE_TYPE_2D,
                               formatDesc,
                               width,
                               height,
                               depth,
                               0,
                               0};

  zeImageCreate(this->device, &imageDesc, image);
}

void XeApp::imageDestroy(ze_image_handle_t image) {
  SUCCESS_OR_TERMINATE(zeImageDestroy(image));
}

void XeApp::commandListCreate(ze_command_list_handle_t *phCommandList) {
  assert(this->device != nullptr);
  commandListCreate(this->device, phCommandList);
}

void XeApp::commandListCreate(ze_device_handle_t device,
                              ze_command_list_handle_t *phCommandList) {
  ze_command_list_desc_t command_list_description;
  command_list_description.version = ZE_COMMAND_LIST_DESC_VERSION_CURRENT;

  SUCCESS_OR_TERMINATE(
      zeCommandListCreate(device, &command_list_description, phCommandList));
}

void XeApp::commandListDestroy(ze_command_list_handle_t command_list) {
  SUCCESS_OR_TERMINATE(zeCommandListDestroy(command_list));
}

void XeApp::commandListClose(ze_command_list_handle_t command_list) {
  SUCCESS_OR_TERMINATE(zeCommandListClose(command_list));
}

void XeApp::commandListReset(ze_command_list_handle_t command_list) {
  SUCCESS_OR_TERMINATE(zeCommandListReset(command_list));
}

void XeApp::commandListAppendImageCopyFromMemory(
    ze_command_list_handle_t command_list, ze_image_handle_t image,
    uint8_t *srcBuffer, ze_image_region_t *Region) {
  SUCCESS_OR_TERMINATE(zeCommandListAppendImageCopyFromMemory(
      command_list, image, srcBuffer, Region, nullptr));
}

void XeApp::commandListAppendBarrier(ze_command_list_handle_t command_list) {
  SUCCESS_OR_TERMINATE(
      zeCommandListAppendBarrier(command_list, nullptr, 0, nullptr));
}

void XeApp::commandListAppendImageCopyToMemory(
    ze_command_list_handle_t command_list, uint8_t *dstBuffer,
    ze_image_handle_t image, ze_image_region_t *Region) {
  SUCCESS_OR_TERMINATE(zeCommandListAppendImageCopyToMemory(
      command_list, dstBuffer, image, Region, nullptr));
}

void XeApp::commandListAppendMemoryCopy(ze_command_list_handle_t command_list,
                                        void *dstptr, void *srcptr,
                                        size_t size) {
  SUCCESS_OR_TERMINATE(zeCommandListAppendMemoryCopy(command_list, dstptr,
                                                     srcptr, size, nullptr));
}

void XeApp::commandQueueCreate(const uint32_t command_queue_id,
                               ze_command_queue_handle_t *command_queue) {
  assert(this->device != nullptr);
  commandQueueCreate(device, command_queue_id, command_queue);
}

void XeApp::commandQueueCreate(ze_device_handle_t device,
                               const uint32_t command_queue_id,
                               ze_command_queue_handle_t *command_queue) {
  ze_command_queue_desc_t command_queue_description;
  command_queue_description.version = ZE_COMMAND_QUEUE_DESC_VERSION_CURRENT;
  command_queue_description.ordinal = command_queue_id;
  command_queue_description.mode = ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS;

  SUCCESS_OR_TERMINATE(
      zeCommandQueueCreate(device, &command_queue_description, command_queue));
}

void XeApp::commandQueueDestroy(ze_command_queue_handle_t command_queue) {
  SUCCESS_OR_TERMINATE(zeCommandQueueDestroy(command_queue));
}

void XeApp::commandQueueExecuteCommandList(
    ze_command_queue_handle_t command_queue, uint32_t numCommandLists,
    ze_command_list_handle_t *command_lists) {
  SUCCESS_OR_TERMINATE(zeCommandQueueExecuteCommandLists(
      command_queue, numCommandLists, command_lists, nullptr));
}

void XeApp::commandQueueSynchronize(ze_command_queue_handle_t command_queue) {
  SUCCESS_OR_TERMINATE(zeCommandQueueSynchronize(command_queue, UINT32_MAX));
}

void XeApp::singleDeviceInit(void) {
  uint32_t driver_count = driverCount();
  assert(driver_count > 0);

  driver_count = 1;
  driverGet(&driver_count, &driver);

  uint32_t device_count = deviceCount(driver);
  assert(device_count > 0);
  device_count = 1;
  driverGetDevices(driver, device_count, &device);

  /*  if module paths string size is not 0 then only call modulecreate */
  if (this->module_path.size() != 0)
    moduleCreate(device, &module);
}

void XeApp::singleDeviceCleanup(void) {

  /*  if module pathsstring size is not 0 then only call moduledestory */
  if (this->module_path.size() != 0)
    moduleDestroy(module);
}

uint32_t XeApp::driverCount(void) {
  uint32_t driver_count = 0;

  SUCCESS_OR_TERMINATE(zeDriverGet(&driver_count, nullptr));

  if (driver_count == 0) {
    std::cerr << "ERROR: zero devices were found" << std::endl;
    std::terminate();
  }

  return driver_count;
}

/* Retrieve "driver_count" */
void XeApp::driverGet(uint32_t *driver_count, ze_driver_handle_t *driver) {
  SUCCESS_OR_TERMINATE(zeDriverGet(driver_count, driver));
}

/* Retrieve array of devices in driver */
void XeApp::driverGetDevices(ze_driver_handle_t driver, uint32_t device_count,
                             ze_device_handle_t *devices) {
  SUCCESS_OR_TERMINATE(zeDeviceGet(driver, &device_count, devices));
}

uint32_t XeApp::deviceCount(ze_driver_handle_t driver) {
  uint32_t device_count = 0;

  SUCCESS_OR_TERMINATE(zeDeviceGet(driver, &device_count, nullptr));
  if (device_count == 0) {
    std::cerr << "ERROR: zero devices were found" << std::endl;
    std::terminate();
  }

  return device_count;
}
