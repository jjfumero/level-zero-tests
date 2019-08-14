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

XeApp::XeApp(std::string module_path) {
  device = nullptr;
  module = nullptr;
  device_group = nullptr;
  this->module_path = module_path;

  SUCCESS_OR_TERMINATE(xeInit(XE_INIT_FLAG_NONE));

  binary_file = load_binary_file(this->module_path);
  std::cout << std::endl;
}

void XeApp::moduleCreate(xe_device_handle_t device,
                         xe_module_handle_t *module) {
  xe_module_desc_t module_description;
  module_description.version = XE_MODULE_DESC_VERSION_CURRENT;
  module_description.format = XE_MODULE_FORMAT_IL_SPIRV;
  module_description.inputSize = binary_file.size();
  module_description.pInputModule = binary_file.data();
  module_description.pBuildFlags = nullptr;

  SUCCESS_OR_TERMINATE(
      xeModuleCreate(device, &module_description, module, nullptr));
}

void XeApp::moduleDestroy(xe_module_handle_t module) {
  SUCCESS_OR_TERMINATE(xeModuleDestroy(module));
}

XeApp::~XeApp(void) {}

void XeApp::memoryAlloc(size_t size, void **ptr) {
  assert(this->device != nullptr);
  assert(this->device_group != nullptr);
  memoryAlloc(this->device_group, this->device, size, ptr);
}

void XeApp::memoryAlloc(xe_device_group_handle_t device_group,
                        xe_device_handle_t device, size_t size, void **ptr) {
  SUCCESS_OR_TERMINATE(xeDeviceGroupAllocDeviceMem(
      device_group, device, XE_DEVICE_MEM_ALLOC_FLAG_DEFAULT, 0, size, 1, ptr));
}

void XeApp::memoryFree(const void *ptr) {
  assert(this->device_group != nullptr);
  SUCCESS_OR_TERMINATE(
      xeDeviceGroupFreeMem(this->device_group, const_cast<void *>(ptr)));
}

void XeApp::memoryFree(xe_device_group_handle_t device_group, const void *ptr) {
  SUCCESS_OR_TERMINATE(
      xeDeviceGroupFreeMem(device_group, const_cast<void *>(ptr)));
}

void XeApp::functionCreate(xe_function_handle_t *function,
                           const char *pFunctionName) {
  assert(this->module != nullptr);
  functionCreate(module, function, pFunctionName);
}

void XeApp::functionCreate(xe_module_handle_t module,
                           xe_function_handle_t *function,
                           const char *pFunctionName) {
  xe_function_desc_t function_description;
  function_description.version = XE_FUNCTION_DESC_VERSION_CURRENT;
  function_description.flags = XE_FUNCTION_FLAG_NONE;
  function_description.pFunctionName = pFunctionName;

  SUCCESS_OR_TERMINATE(
      xeFunctionCreate(module, &function_description, function));
}

void XeApp::functionDestroy(xe_function_handle_t function) {
  SUCCESS_OR_TERMINATE(xeFunctionDestroy(function));
}

void XeApp::imageCreate(xe_image_handle_t *image) {
  assert(this->device != nullptr);
  imageCreate(this->device, image);
}

void XeApp::imageCreate(xe_device_handle_t device, xe_image_handle_t *image) {
  xe_image_format_desc_t formatDesc = {
      XE_IMAGE_FORMAT_LAYOUT_32, XE_IMAGE_FORMAT_TYPE_FLOAT,
      XE_IMAGE_FORMAT_SWIZZLE_R, XE_IMAGE_FORMAT_SWIZZLE_0,
      XE_IMAGE_FORMAT_SWIZZLE_0, XE_IMAGE_FORMAT_SWIZZLE_1};

  xe_image_desc_t imageDesc = {XE_IMAGE_DESC_VERSION_CURRENT,
                               XE_IMAGE_FLAG_PROGRAM_READ,
                               XE_IMAGE_TYPE_2D,
                               formatDesc,
                               128,
                               128,
                               0,
                               0,
                               0};

  xeImageCreate(device, &imageDesc, image);
}

void XeApp::imageDestroy(xe_image_handle_t image) {
  SUCCESS_OR_TERMINATE(xeImageDestroy(image));
}

void XeApp::commandListCreate(xe_command_list_handle_t *phCommandList) {
  assert(this->device != nullptr);
  commandListCreate(this->device, phCommandList);
}

void XeApp::commandListCreate(xe_device_handle_t device,
                              xe_command_list_handle_t *phCommandList) {
  xe_command_list_desc_t command_list_description;
  command_list_description.version = XE_COMMAND_LIST_DESC_VERSION_CURRENT;

  SUCCESS_OR_TERMINATE(
      xeCommandListCreate(device, &command_list_description, phCommandList));
}

void XeApp::commandListDestroy(xe_command_list_handle_t command_list) {
  SUCCESS_OR_TERMINATE(xeCommandListDestroy(command_list));
}

void XeApp::commandListClose(xe_command_list_handle_t command_list) {
  SUCCESS_OR_TERMINATE(xeCommandListClose(command_list));
}

void XeApp::commandQueueCreate(const uint32_t command_queue_id,
                               xe_command_queue_handle_t *command_queue) {
  assert(this->device != nullptr);
  commandQueueCreate(device, command_queue_id, command_queue);
}

void XeApp::commandQueueCreate(xe_device_handle_t device,
                               const uint32_t command_queue_id,
                               xe_command_queue_handle_t *command_queue) {
  xe_command_queue_desc_t command_queue_description;
  command_queue_description.version = XE_COMMAND_QUEUE_DESC_VERSION_CURRENT;
  command_queue_description.ordinal = command_queue_id;
  command_queue_description.mode = XE_COMMAND_QUEUE_MODE_ASYNCHRONOUS;

  SUCCESS_OR_TERMINATE(
      xeCommandQueueCreate(device, &command_queue_description, command_queue));
}

void XeApp::commandQueueDestroy(xe_command_queue_handle_t command_queue) {
  SUCCESS_OR_TERMINATE(xeCommandQueueDestroy(command_queue));
}

void XeApp::commandQueueExecuteCommandList(
    xe_command_queue_handle_t command_queue, uint32_t numCommandLists,
    xe_command_list_handle_t *command_lists) {
  SUCCESS_OR_TERMINATE(xeCommandQueueExecuteCommandLists(
      command_queue, numCommandLists, command_lists, nullptr));
}

void XeApp::commandQueueSynchronize(xe_command_queue_handle_t command_queue) {
  SUCCESS_OR_TERMINATE(xeCommandQueueSynchronize(command_queue, UINT32_MAX));
}

void XeApp::singleDeviceInit(void) {
  uint32_t device_group_count = deviceGroupCount();
  assert(device_group_count > 0);

  device_group_count = 1;
  deviceGroupGet(&device_group_count, &device_group);

  uint32_t device_count = deviceCount(device_group);
  assert(device_count > 0);
  device_count = 1;
  deviceGroupGetDevices(device_group, device_count, &device);

  moduleCreate(device, &module);
}

void XeApp::singleDeviceCleanup(void) { moduleDestroy(module); }

uint32_t XeApp::deviceGroupCount(void) {
  uint32_t device_group_count = 0;

  SUCCESS_OR_TERMINATE(xeDeviceGroupGet(&device_group_count, nullptr));

  if (device_group_count == 0) {
    std::cerr << "ERROR: zero devices were found" << std::endl;
    std::terminate();
  }

  return device_group_count;
}

/* Retrieve "device_group_count" device groups */
void XeApp::deviceGroupGet(uint32_t *device_group_count,
                           xe_device_group_handle_t *device_group) {
  SUCCESS_OR_TERMINATE(xeDeviceGroupGet(device_group_count, device_group));
}

/* Retrieve array of devices in device group */
void XeApp::deviceGroupGetDevices(xe_device_group_handle_t device_group,
                                  uint32_t device_count,
                                  xe_device_handle_t *devices) {
  SUCCESS_OR_TERMINATE(
      xeDeviceGroupGetDevices(device_group, &device_count, devices));
}

uint32_t XeApp::deviceCount(xe_device_group_handle_t device_group) {
  uint32_t device_count = 0;

  SUCCESS_OR_TERMINATE(
      xeDeviceGroupGetDevices(device_group, &device_count, nullptr));
  if (device_count == 0) {
    std::cerr << "ERROR: zero devices were found" << std::endl;
    std::terminate();
  }

  return device_count;
}
