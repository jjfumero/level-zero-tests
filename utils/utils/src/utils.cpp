/*
 * INTEL CONFIDENTIAL
 *
 * Copyright 2019 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code (Material) are owned by Intel Corporation or its
 * suppliers or licensors.
 *
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors. The Material contains trade secrets and proprietary and
 * confidential information of Intel or its suppliers and licensors.
 * The Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure
 * or delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 */

#include "utils/utils.hpp"
#include "logging/logging.hpp"

#include <iostream>
#include <fstream>

namespace level_zero_tests {

ze_driver_handle_t get_default_driver() {
  ze_result_t result = ZE_RESULT_SUCCESS;

  static ze_driver_handle_t driver = nullptr;
  if (driver)
    return driver;

  uint32_t count = 1;
  result = zeDriverGet(&count, &driver);

  if (result || !driver) {
    throw std::runtime_error("zeDriverGet failed: " + to_string(result));
  }
  LOG_TRACE << "Device retrieved";
  return driver;
}

ze_device_handle_t get_default_device() {
  ze_result_t result = ZE_RESULT_SUCCESS;

  static ze_device_handle_t device = nullptr;
  if (device)
    return device;

  uint32_t count = 1;
  result = zeDeviceGet(get_default_driver(), &count, &device);

  if (result || !device) {
    throw std::runtime_error("zeDeviceGet failed: " + to_string(result));
  }
  LOG_TRACE << "Device retrieved";
  return device;
}

uint32_t get_device_count(ze_driver_handle_t driver) {
  uint32_t count = 0;
  ze_result_t result = zeDeviceGet(driver, &count, nullptr);

  if (result) {
    throw std::runtime_error("zeDeviceGet failed: " + to_string(result));
  }
  return count;
}

uint32_t get_driver_handle_count() {
  uint32_t count = 0;
  ze_result_t result = zeDriverGet(&count, nullptr);

  if (result) {
    throw std::runtime_error("zeDriverGet failed: " + to_string(result));
  }
  return count;
}

uint32_t get_sub_device_count(ze_device_handle_t device) {
  uint32_t count = 0;

  ze_result_t result = zeDeviceGetSubDevices(device, &count, nullptr);

  if (result) {
    throw std::runtime_error("zeDeviceGetSubDevices failed: " +
                             to_string(result));
  }
  return count;
}

std::vector<ze_driver_handle_t> get_all_driver_handles() {
  ze_result_t result = ZE_RESULT_SUCCESS;
  uint32_t driver_handle_count = get_driver_handle_count();

  std::vector<ze_driver_handle_t> driver_handles(driver_handle_count);

  result = zeDriverGet(&driver_handle_count, driver_handles.data());

  if (result) {
    throw std::runtime_error("zeDriverGet failed: " + to_string(result));
  }
  return driver_handles;
}

std::vector<ze_device_handle_t> get_devices(ze_driver_handle_t driver) {

  ze_result_t result = ZE_RESULT_SUCCESS;

  uint32_t device_count = get_device_count(driver);
  std::vector<ze_device_handle_t> devices(device_count);

  result = zeDeviceGet(driver, &device_count, devices.data());

  if (result) {
    throw std::runtime_error("zeDeviceGet failed: " + to_string(result));
  }
  return devices;
}

std::string to_string(const ze_api_version_t version) {
  std::stringstream ss;
  ss << ZE_MAJOR_VERSION(version) << "." << ZE_MINOR_VERSION(version);
  return ss.str();
}

std::string to_string(const ze_result_t result) {
  if (result == ZE_RESULT_SUCCESS) {
    return "ZE_RESULT_SUCCESS";
  } else if (result == ZE_RESULT_NOT_READY) {
    return "ZE_RESULT_NOT_READY";
  } else if (result == ZE_RESULT_ERROR_UNINITIALIZED) {
    return "ZE_RESULT_ERROR_UNINITIALIZED";
  } else if (result == ZE_RESULT_ERROR_DEVICE_LOST) {
    return "ZE_RESULT_ERROR_DEVICE_LOST";
  } else if (result == ZE_RESULT_ERROR_UNSUPPORTED) {
    return "ZE_RESULT_ERROR_UNSUPPORTED";
  } else if (result == ZE_RESULT_ERROR_INVALID_ARGUMENT) {
    return "ZE_RESULT_ERROR_INVALID_ARGUMENT";
  } else if (result == ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY) {
    return "ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY";
  } else if (result == ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY) {
    return "ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY";
  } else if (result == ZE_RESULT_ERROR_MODULE_BUILD_FAILURE) {
    return "ZE_RESULT_ERROR_MODULE_BUILD_FAILURE";
  } else if (result == ZE_RESULT_ERROR_UNKNOWN) {
    return "ZE_RESULT_ERROR_UNKNOWN";
  } else {
    throw std::runtime_error("Unknown ze_result_t value: " +
                             std::to_string(static_cast<int>(result)));
  }
}

std::string to_string(const ze_command_queue_desc_version_t version) {
  if (version == ZE_COMMAND_QUEUE_DESC_VERSION_CURRENT) {
    return "ZE_COMMAND_QUEUE_DESC_VERSION_CURRENT";
  } else {
    return "NON-CURRENT ZE_COMMAND_QUEUE_DESC_VERSION";
  }
}

std::string to_string(const ze_command_queue_flag_t flags) {
  if (flags == ZE_COMMAND_QUEUE_FLAG_NONE) {
    return "ZE_COMMAND_QUEUE_FLAG_NONE";
  } else if (flags == ZE_COMMAND_QUEUE_FLAG_COPY_ONLY) {
    return "ZE_COMMAND_QUEUE_FLAG_COPY_ONLY";
  } else if (flags == ZE_COMMAND_QUEUE_FLAG_LOGICAL_ONLY) {
    return "ZE_COMMAND_QUEUE_FLAG_LOGICAL_ONLY";
  } else if (flags == ZE_COMMAND_QUEUE_FLAG_SINGLE_SLICE_ONLY) {
    return "ZE_COMMAND_QUEUE_FLAG_SINGLE_SLICE_ONLY";
  } else {
    return "Unknown ze_command_queue_flag_t value: " +
           std::to_string(static_cast<int>(flags));
  }
}

std::string to_string(const ze_command_queue_mode_t mode) {
  if (mode == ZE_COMMAND_QUEUE_MODE_DEFAULT) {
    return "ZE_COMMAND_QUEUE_MODE_DEFAULT";
  } else if (mode == ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS) {
    return "ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS";
  } else if (mode == ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS) {
    return "ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS";
  } else {
    return "Unknown ze_command_queue_mode_t value: " +
           std::to_string(static_cast<int>(mode));
  }
}

std::string to_string(const ze_command_queue_priority_t priority) {
  if (priority == ZE_COMMAND_QUEUE_PRIORITY_NORMAL) {
    return "ZE_COMMAND_QUEUE_PRIORITY_NORMAL";
  } else if (priority == ZE_COMMAND_QUEUE_PRIORITY_LOW) {
    return "ZE_COMMAND_QUEUE_PRIORITY_LOW";
  } else if (priority == ZE_COMMAND_QUEUE_PRIORITY_HIGH) {
    return "ZE_COMMAND_QUEUE_PRIORITY_HIGH";
  } else {
    return "Unknown ze_command_queue_priority_t value: " +
           std::to_string(static_cast<int>(priority));
  }
}

std::string to_string(const ze_image_desc_version_t version) {
  if (version == ZE_IMAGE_DESC_VERSION_CURRENT) {
    return "ZE_IMAGE_DESC_VERSION_CURRENT";
  } else {
    return "NON-CURRENT ZE_IMAGE_DESC_VERSION";
  }
}

std::string to_string(const ze_image_format_layout_t layout) {
  if (layout == ZE_IMAGE_FORMAT_LAYOUT_8) {
    return "ZE_IMAGE_FORMAT_LAYOUT_8";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_16) {
    return "ZE_IMAGE_FORMAT_LAYOUT_16";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_32) {
    return "ZE_IMAGE_FORMAT_LAYOUT_32";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_8_8) {
    return "ZE_IMAGE_FORMAT_LAYOUT_8_8";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_8_8_8_8) {
    return "ZE_IMAGE_FORMAT_LAYOUT_8_8_8_8";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_16_16) {
    return "ZE_IMAGE_FORMAT_LAYOUT_16_16";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_16_16_16_16) {
    return "ZE_IMAGE_FORMAT_LAYOUT_16_16_16_16";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_32_32) {
    return "ZE_IMAGE_FORMAT_LAYOUT_32_32";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_32_32_32_32) {
    return "ZE_IMAGE_FORMAT_LAYOUT_32_32_32_32";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_10_10_10_2) {
    return "ZE_IMAGE_FORMAT_LAYOUT_10_10_10_2";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_11_11_10) {
    return "ZE_IMAGE_FORMAT_LAYOUT_11_11_10";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_5_6_5) {
    return "ZE_IMAGE_FORMAT_LAYOUT_5_6_5";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_5_5_5_1) {
    return "ZE_IMAGE_FORMAT_LAYOUT_5_5_5_1";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_4_4_4_4) {
    return "ZE_IMAGE_FORMAT_LAYOUT_4_4_4_4";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_Y8) {
    return "ZE_IMAGE_FORMAT_LAYOUT_Y8";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_NV12) {
    return "ZE_IMAGE_FORMAT_LAYOUT_NV12";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_YUYV) {
    return "ZE_IMAGE_FORMAT_LAYOUT_YUYV";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_VYUY) {
    return "ZE_IMAGE_FORMAT_LAYOUT_VYUY";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_YVYU) {
    return "ZE_IMAGE_FORMAT_LAYOUT_YVYU";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_UYVY) {
    return "ZE_IMAGE_FORMAT_LAYOUT_UYVY";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_AYUV) {
    return "ZE_IMAGE_FORMAT_LAYOUT_AYUV";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_YUAV) {
    return "ZE_IMAGE_FORMAT_LAYOUT_YUAV";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_P010) {
    return "ZE_IMAGE_FORMAT_LAYOUT_P010";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_Y410) {
    return "ZE_IMAGE_FORMAT_LAYOUT_Y410";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_P012) {
    return "ZE_IMAGE_FORMAT_LAYOUT_P012";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_Y16) {
    return "ZE_IMAGE_FORMAT_LAYOUT_Y16";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_P016) {
    return "ZE_IMAGE_FORMAT_LAYOUT_P016";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_Y216) {
    return "ZE_IMAGE_FORMAT_LAYOUT_Y216";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_P216) {
    return "ZE_IMAGE_FORMAT_LAYOUT_P216";
  } else if (layout == ZE_IMAGE_FORMAT_LAYOUT_P416) {
    return "ZE_IMAGE_FORMAT_LAYOUT_P416";
  } else {
    return "Unknown ze_image_format_layout_t value: " +
           std::to_string(static_cast<int>(layout));
  }
}

ze_image_format_layout_t to_layout(const std::string layout) {
  if (layout == "8") {
    return ZE_IMAGE_FORMAT_LAYOUT_8;
  } else if (layout == "16") {
    return ZE_IMAGE_FORMAT_LAYOUT_16;
  } else if (layout == "32") {
    return ZE_IMAGE_FORMAT_LAYOUT_32;
  } else if (layout == "8_8") {
    return ZE_IMAGE_FORMAT_LAYOUT_8_8;
  } else if (layout == "8_8_8_8") {
    return ZE_IMAGE_FORMAT_LAYOUT_8_8_8_8;
  } else if (layout == "16_16") {
    return ZE_IMAGE_FORMAT_LAYOUT_16_16;
  } else if (layout == "16_16_16_16") {
    return ZE_IMAGE_FORMAT_LAYOUT_16_16_16_16;
  } else if (layout == "32_32") {
    return ZE_IMAGE_FORMAT_LAYOUT_32_32;
  } else if (layout == "32_32_32_32") {
    return ZE_IMAGE_FORMAT_LAYOUT_32_32_32_32;
  } else if (layout == "10_10_10_2") {
    return ZE_IMAGE_FORMAT_LAYOUT_10_10_10_2;
  } else if (layout == "11_11_10") {
    return ZE_IMAGE_FORMAT_LAYOUT_11_11_10;
  } else if (layout == "5_6_5") {
    return ZE_IMAGE_FORMAT_LAYOUT_5_6_5;
  } else if (layout == "5_5_5_1") {
    return ZE_IMAGE_FORMAT_LAYOUT_5_5_5_1;
  } else if (layout == "4_4_4_4") {
    return ZE_IMAGE_FORMAT_LAYOUT_4_4_4_4;
  } else if (layout == "Y8") {
    return ZE_IMAGE_FORMAT_LAYOUT_Y8;
  } else if (layout == "NV12") {
    return ZE_IMAGE_FORMAT_LAYOUT_NV12;
  } else if (layout == "YUYV") {
    return ZE_IMAGE_FORMAT_LAYOUT_YUYV;
  } else if (layout == "VYUY") {
    return ZE_IMAGE_FORMAT_LAYOUT_VYUY;
  } else if (layout == "YVYU") {
    return ZE_IMAGE_FORMAT_LAYOUT_YVYU;
  } else if (layout == "UYVY") {
    return ZE_IMAGE_FORMAT_LAYOUT_UYVY;
  } else if (layout == "AYUV") {
    return ZE_IMAGE_FORMAT_LAYOUT_AYUV;
  } else if (layout == "YUAV") {
    return ZE_IMAGE_FORMAT_LAYOUT_YUAV;
  } else if (layout == "P010") {
    return ZE_IMAGE_FORMAT_LAYOUT_P010;
  } else if (layout == "Y410") {
    return ZE_IMAGE_FORMAT_LAYOUT_Y410;
  } else if (layout == "P012") {
    return ZE_IMAGE_FORMAT_LAYOUT_P012;
  } else if (layout == "Y16") {
    return ZE_IMAGE_FORMAT_LAYOUT_Y16;
  } else if (layout == "P016") {
    return ZE_IMAGE_FORMAT_LAYOUT_P016;
  } else if (layout == "Y216") {
    return ZE_IMAGE_FORMAT_LAYOUT_Y216;
  } else if (layout == "P216") {
    return ZE_IMAGE_FORMAT_LAYOUT_P216;
  } else if (layout == "P416") {
    return ZE_IMAGE_FORMAT_LAYOUT_P416;
  } else {
    std::cout << "Unknown ze_image_format_layout_t value: " << layout;
    return static_cast<ze_image_format_layout_t>(-1);
  }
}

std::string to_string(const ze_image_format_type_t type) {
  if (type == ZE_IMAGE_FORMAT_TYPE_UINT) {
    return "ZE_IMAGE_FORMAT_TYPE_UINT";
  } else if (type == ZE_IMAGE_FORMAT_TYPE_SINT) {
    return "ZE_IMAGE_FORMAT_TYPE_SINT";
  } else if (type == ZE_IMAGE_FORMAT_TYPE_UNORM) {
    return "ZE_IMAGE_FORMAT_TYPE_UNORM";
  } else if (type == ZE_IMAGE_FORMAT_TYPE_SNORM) {
    return "ZE_IMAGE_FORMAT_TYPE_SNORM";
  } else if (type == ZE_IMAGE_FORMAT_TYPE_FLOAT) {
    return "ZE_IMAGE_FORMAT_TYPE_FLOAT";
  } else {
    return "Unknown ze_image_format_type_t value: " +
           std::to_string(static_cast<int>(type));
  }
}

ze_image_format_type_t to_format_type(const std::string format_type) {
  if (format_type == "UINT") {
    return ZE_IMAGE_FORMAT_TYPE_UINT;
  } else if (format_type == "SINT") {
    return ZE_IMAGE_FORMAT_TYPE_SINT;
  } else if (format_type == "UNORM") {
    return ZE_IMAGE_FORMAT_TYPE_UNORM;
  } else if (format_type == "SNORM") {
    return ZE_IMAGE_FORMAT_TYPE_SNORM;
  } else if (format_type == "FLOAT") {
    return ZE_IMAGE_FORMAT_TYPE_FLOAT;
  } else {
    std::cout << "Unknown ze_image_format_type_t value: ";
    return (static_cast<ze_image_format_type_t>(-1));
  }
}

std::string to_string(const ze_image_format_swizzle_t swizzle) {
  if (swizzle == ZE_IMAGE_FORMAT_SWIZZLE_R) {
    return "ZE_IMAGE_FORMAT_SWIZZLE_R";
  } else if (swizzle == ZE_IMAGE_FORMAT_SWIZZLE_G) {
    return "ZE_IMAGE_FORMAT_SWIZZLE_G";
  } else if (swizzle == ZE_IMAGE_FORMAT_SWIZZLE_B) {
    return "ZE_IMAGE_FORMAT_SWIZZLE_B";
  } else if (swizzle == ZE_IMAGE_FORMAT_SWIZZLE_A) {
    return "ZE_IMAGE_FORMAT_SWIZZLE_A";
  } else if (swizzle == ZE_IMAGE_FORMAT_SWIZZLE_0) {
    return "ZE_IMAGE_FORMAT_SWIZZLE_0";
  } else if (swizzle == ZE_IMAGE_FORMAT_SWIZZLE_1) {
    return "ZE_IMAGE_FORMAT_SWIZZLE_1";
  } else if (swizzle == ZE_IMAGE_FORMAT_SWIZZLE_X) {
    return "ZE_IMAGE_FORMAT_SWIZZLE_X";
  } else {
    return "Unknown ze_image_format_swizzle_t value: " +
           std::to_string(static_cast<int>(swizzle));
  }
}

std::string to_string(const ze_image_flag_t flag) {
  std::string flags = "";
  if (flag & ZE_IMAGE_FLAG_PROGRAM_READ) {
    flags.append("|ZE_IMAGE_FLAG_PROGRAM_READ|");
  }
  if (flag & ZE_IMAGE_FLAG_PROGRAM_WRITE) {
    flags.append("|ZE_IMAGE_FLAG_PROGRAM_WRITE|");
  }
  if (flag & ZE_IMAGE_FLAG_BIAS_CACHED) {
    flags.append("|ZE_IMAGE_FLAG_BIAS_CACHED|");
  }
  if (flag & ZE_IMAGE_FLAG_BIAS_UNCACHED) {
    flags.append("|ZE_IMAGE_FLAG_BIAS_UNCACHED|");
  }

  return flags;
}

ze_image_flag_t to_flag(const std::string flag) {

  // by default setting to READ
  ze_image_flag_t image_flags = ZE_IMAGE_FLAG_PROGRAM_READ;

  // check if "READ" position is found in flag string
  if (flag.find("READ") != std::string::npos) {
    image_flags =
        static_cast<ze_image_flag_t>(image_flags | ZE_IMAGE_FLAG_PROGRAM_READ);
  }
  if (flag.find("WRITE") != std::string::npos) {
    image_flags =
        static_cast<ze_image_flag_t>(image_flags | ZE_IMAGE_FLAG_PROGRAM_WRITE);
  }
  if (flag.find("CACHED") != std::string::npos) {
    image_flags =
        static_cast<ze_image_flag_t>(image_flags | ZE_IMAGE_FLAG_BIAS_CACHED);
  }
  if (flag.find("UNCACHED") != std::string::npos) {
    image_flags =
        static_cast<ze_image_flag_t>(image_flags | ZE_IMAGE_FLAG_BIAS_UNCACHED);
  }

  return image_flags;
}

std::string to_string(const ze_image_type_t type) {
  if (type == ZE_IMAGE_TYPE_1D) {
    return "ZE_IMAGE_TYPE_1D";
  } else if (type == ZE_IMAGE_TYPE_2D) {
    return "ZE_IMAGE_TYPE_2D";
  } else if (type == ZE_IMAGE_TYPE_3D) {
    return "ZE_IMAGE_TYPE_3D";
  } else if (type == ZE_IMAGE_TYPE_1DARRAY) {
    return "ZE_IMAGE_TYPE_1DARRAY";
  } else if (type == ZE_IMAGE_TYPE_2DARRAY) {
    return "ZE_IMAGE_TYPE_2DARRAY";
  } else {
    return "Unknown ze_image_type_t value: " +
           std::to_string(static_cast<int>(type));
  }
}

ze_image_type_t to_image_type(const std::string type) {
  if (type == "1D") {
    return ZE_IMAGE_TYPE_1D;
  } else if (type == "2D") {
    return ZE_IMAGE_TYPE_2D;
  } else if (type == "3D") {
    return ZE_IMAGE_TYPE_3D;
  } else if (type == "1DARRAY") {
    return ZE_IMAGE_TYPE_1DARRAY;
  } else if (type == "2DARRAY") {
    return ZE_IMAGE_TYPE_2DARRAY;
  } else {
    std::cout << "Unknown ze_image_type_t value: ";
    return (static_cast<ze_image_type_t>(-1));
  }
}

void print_header_version() {
  LOG_INFO << "API header version: " << ZE_MAJOR_VERSION(ZE_API_HEADER_VERSION)
           << "." << ZE_MINOR_VERSION(ZE_API_HEADER_VERSION);
}

void print_driver_version() {
  uint32_t version = 0;
  ze_driver_handle_t driver = get_default_driver();
  ze_result_t result = zeDriverGetDriverVersion(driver, &version);
  if (result) {
    std::runtime_error("zeDriverGetDriverVersion failed: " + to_string(result));
  }
  LOG_TRACE << "Driver version retrieved";

  LOG_INFO << "Driver version: " << version;
}

void print_driver_overview(const ze_driver_handle_t driver) {
  ze_result_t result = ZE_RESULT_SUCCESS;

  ze_device_properties_t device_properties;
  device_properties.version = ZE_DEVICE_PROPERTIES_VERSION_CURRENT;
  auto devices = get_devices(driver);
  if (devices.size() > 0) {
    auto device = devices[0];
    result = zeDeviceGetProperties(device, &device_properties);
    if (result) {
      std::runtime_error("zeDeviceGetDeviceProperties failed: " +
                         to_string(result));
    }
    LOG_TRACE << "Device properties retrieved";
    LOG_INFO << "Device name: " << device_properties.name;
  }

  ze_api_version_t api_version;
  result = zeDriverGetApiVersion(driver, &api_version);
  if (result) {
    throw std::runtime_error("zeDriverGetApiVersion failed: " +
                             to_string(result));
  }
  LOG_TRACE << "Driver API version retrieved";

  LOG_INFO << "Driver API version: " << to_string(api_version);
}

void print_driver_overview(const std::vector<ze_driver_handle_t> driver) {
  for (const ze_driver_handle_t driver : driver) {
    print_driver_overview(driver);
  }
}

void print_platform_overview(const std::string context) {
  LOG_INFO << "Platform overview";
  if (context.size() > 0) {
    LOG_INFO << " (Context: " << context << ")";
  }
  print_header_version();
  print_driver_version();

  const std::vector<ze_driver_handle_t> drivers = get_all_driver_handles();
  LOG_INFO << "Driver Handle count: " << drivers.size();

  print_driver_overview(drivers);
}

void print_platform_overview() { print_platform_overview(""); }

std::vector<uint8_t> load_binary_file(const std::string &file_path) {
  LOG_ENTER_FUNCTION
  LOG_DEBUG << "File path: " << file_path;
  std::ifstream stream(file_path, std::ios::in | std::ios::binary);

  std::vector<uint8_t> binary_file;
  if (!stream.good()) {
    LOG_ERROR << "Failed to load binary file: " << file_path;
    LOG_EXIT_FUNCTION
    return binary_file;
  }

  size_t length = 0;
  stream.seekg(0, stream.end);
  length = static_cast<size_t>(stream.tellg());
  stream.seekg(0, stream.beg);
  LOG_DEBUG << "Binary file length: " << length;

  binary_file.resize(length);
  stream.read(reinterpret_cast<char *>(binary_file.data()), length);
  LOG_DEBUG << "Binary file loaded";

  LOG_EXIT_FUNCTION
  return binary_file;
}

void save_binary_file(const std::vector<uint8_t> &data,
                      const std::string &file_path) {
  LOG_ENTER_FUNCTION
  LOG_DEBUG << "File path: " << file_path;

  std::ofstream stream(file_path, std::ios::out | std::ios::binary);
  stream.write(reinterpret_cast<const char *>(data.data()),
               size_in_bytes(data));

  LOG_EXIT_FUNCTION
}

} // namespace level_zero_tests

std::ostream &operator<<(std::ostream &os, const ze_api_version_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const ze_result_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os,
                         const ze_command_queue_desc_version_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const ze_command_queue_flag_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const ze_command_queue_mode_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os,
                         const ze_command_queue_priority_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const ze_image_desc_version_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const ze_image_format_layout_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const ze_image_format_type_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const ze_image_format_swizzle_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const ze_image_flag_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const ze_image_type_t &x) {
  return os << level_zero_tests::to_string(x);
}
