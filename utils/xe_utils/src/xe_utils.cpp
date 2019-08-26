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

#include "xe_utils/xe_utils.hpp"
#include "logging/logging.hpp"

namespace level_zero_tests {

uint32_t get_device_group_count() {
  uint32_t count = 0;
  xe_result_t result = xeDeviceGroupGet(&count, nullptr);

  if (result) {
    throw std::runtime_error("xeDeviceGroupGet failed:" + to_string(result));
  }
  return count;
}

xe_device_group_handle_t get_default_device_group() {
  xe_result_t result = XE_RESULT_SUCCESS;

  xe_device_group_handle_t device_group = nullptr;
  uint32_t count = 1;
  result = xeDeviceGroupGet(&count, &device_group);

  if (result || !device_group) {
    throw std::runtime_error("xeDeviceGroupGet failed: " + to_string(result));
  }
  LOG_TRACE << "Device group retrieved";
  return device_group;
}

uint32_t get_device_count(xe_device_group_handle_t device_group) {
  uint32_t count = 0;
  xe_result_t result = xeDeviceGroupGetDevices(device_group, &count, nullptr);

  if (result) {
    throw std::runtime_error("xeDeviceGroupGetDevices failed: " +
                             to_string(result));
  }
  return count;
}

xe_device_handle_t get_default_device() {
  xe_result_t result = XE_RESULT_SUCCESS;

  // TODO: Select device from command line
  xe_device_handle_t device = nullptr;
  uint32_t count = 1;
  result = xeDeviceGroupGetDevices(get_default_device_group(), &count, &device);

  if (result) {
    throw std::runtime_error("xeDeviceGroupGetDevices failed: " +
                             to_string(result));
  }
  LOG_TRACE << "Device retrieved";

  return device;
}

uint32_t get_sub_device_count(xe_device_handle_t device) {
  uint32_t count = 0;

  xe_result_t result = xeDeviceGetSubDevices(device, &count, nullptr);

  if (result) {
    throw std::runtime_error("xeDeviceGetSubDevices failed: " +
                             to_string(result));
  }
  return count;
}

std::vector<xe_device_group_handle_t> get_all_device_groups() {
  xe_result_t result = XE_RESULT_SUCCESS;
  uint32_t device_group_count = get_device_group_count();

  std::vector<xe_device_group_handle_t> device_groups(device_group_count);

  result = xeDeviceGroupGet(&device_group_count, device_groups.data());

  if (result) {
    throw std::runtime_error("xeDeviceGroupGet failed: " + to_string(result));
  }
  return device_groups;
}

std::vector<xe_device_handle_t>
get_devices(xe_device_group_handle_t device_group) {

  xe_result_t result = XE_RESULT_SUCCESS;

  uint32_t device_count = get_device_count(device_group);
  std::vector<xe_device_handle_t> devices(device_count);

  result = xeDeviceGroupGetDevices(device_group, &device_count, devices.data());

  if (result) {
    throw std::runtime_error("xeDeviceGroupGetDevices failed: " +
                             to_string(result));
  }
  return devices;
}

std::string to_string(const xe_api_version_t version) {
  std::stringstream ss;
  ss << XE_MAJOR_VERSION(version) << "." << XE_MINOR_VERSION(version);
  return ss.str();
}

std::string to_string(const xe_result_t result) {
  if (result == XE_RESULT_SUCCESS) {
    return "XE_RESULT_SUCCESS";
  } else if (result == XE_RESULT_NOT_READY) {
    return "XE_RESULT_NOT_READY";
  } else if (result == XE_RESULT_ERROR_UNINITIALIZED) {
    return "XE_RESULT_ERROR_UNINITIALIZED";
  } else if (result == XE_RESULT_ERROR_DEVICE_LOST) {
    return "XE_RESULT_ERROR_DEVICE_LOST";
  } else if (result == XE_RESULT_ERROR_UNSUPPORTED) {
    return "XE_RESULT_ERROR_UNSUPPORTED";
  } else if (result == XE_RESULT_ERROR_INVALID_PARAMETER) {
    return "XE_RESULT_ERROR_INVALID_PARAMETER";
  } else if (result == XE_RESULT_ERROR_OUT_OF_HOST_MEMORY) {
    return "XE_RESULT_ERROR_OUT_OF_HOST_MEMORY";
  } else if (result == XE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY) {
    return "XE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY";
  } else if (result == XE_RESULT_ERROR_MODULE_BUILD_FAILURE) {
    return "XE_RESULT_ERROR_MODULE_BUILD_FAILURE";
  } else if (result == XE_RESULT_ERROR_UNKNOWN) {
    return "XE_RESULT_ERROR_UNKNOWN";
  } else {
    throw std::runtime_error("Unknown xe_result_t value: " +
                             static_cast<int>(result));
  }
}

std::string to_string(const xe_command_queue_desc_version_t version) {
  if (version == XE_COMMAND_QUEUE_DESC_VERSION_CURRENT) {
    return "XE_COMMAND_QUEUE_DESC_VERSION_CURRENT";
  } else {
    return "NON-CURRENT XE_COMMAND_QUEUE_DESC_VERSION";
  }
}

std::string to_string(const xe_command_queue_flag_t flags) {
  if (flags == XE_COMMAND_QUEUE_FLAG_NONE) {
    return "XE_COMMAND_QUEUE_FLAG_NONE";
  } else if (flags == XE_COMMAND_QUEUE_FLAG_COPY_ONLY) {
    return "XE_COMMAND_QUEUE_FLAG_COPY_ONLY";
  } else if (flags == XE_COMMAND_QUEUE_FLAG_LOGICAL_ONLY) {
    return "XE_COMMAND_QUEUE_FLAG_LOGICAL_ONLY";
  } else if (flags == XE_COMMAND_QUEUE_FLAG_SINGLE_SLICE_ONLY) {
    return "XE_COMMAND_QUEUE_FLAG_SINGLE_SLICE_ONLY";
  } else {
    return "Unknown xe_command_queue_flag_t value: " +
           std::to_string(static_cast<int>(flags));
  }
}

std::string to_string(const xe_command_queue_mode_t mode) {
  if (mode == XE_COMMAND_QUEUE_MODE_DEFAULT) {
    return "XE_COMMAND_QUEUE_MODE_DEFAULT";
  } else if (mode == XE_COMMAND_QUEUE_MODE_SYNCHRONOUS) {
    return "XE_COMMAND_QUEUE_MODE_SYNCHRONOUS";
  } else if (mode == XE_COMMAND_QUEUE_MODE_ASYNCHRONOUS) {
    return "XE_COMMAND_QUEUE_MODE_ASYNCHRONOUS";
  } else {
    return "Unknown xe_command_queue_mode_t value: " +
           std::to_string(static_cast<int>(mode));
  }
}

std::string to_string(const xe_command_queue_priority_t priority) {
  if (priority == XE_COMMAND_QUEUE_PRIORITY_NORMAL) {
    return "XE_COMMAND_QUEUE_PRIORITY_NORMAL";
  } else if (priority == XE_COMMAND_QUEUE_PRIORITY_LOW) {
    return "XE_COMMAND_QUEUE_PRIORITY_LOW";
  } else if (priority == XE_COMMAND_QUEUE_PRIORITY_HIGH) {
    return "XE_COMMAND_QUEUE_PRIORITY_HIGH";
  } else {
    return "Unknown xe_command_queue_priority_t value: " +
           std::to_string(static_cast<int>(priority));
  }
}

std::string to_string(const xe_image_desc_version_t version) {
  if (version == XE_IMAGE_DESC_VERSION_CURRENT) {
    return "XE_IMAGE_DESC_VERSION_CURRENT";
  } else {
    return "NON-CURRENT XE_IMAGE_DESC_VERSION";
  }
}

std::string to_string(const xe_image_format_layout_t layout) {
  if (layout == XE_IMAGE_FORMAT_LAYOUT_8) {
    return "XE_IMAGE_FORMAT_LAYOUT_8";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_16) {
    return "XE_IMAGE_FORMAT_LAYOUT_16";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_32) {
    return "XE_IMAGE_FORMAT_LAYOUT_32";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_8_8) {
    return "XE_IMAGE_FORMAT_LAYOUT_8_8";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_8_8_8_8) {
    return "XE_IMAGE_FORMAT_LAYOUT_8_8_8_8";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_16_16) {
    return "XE_IMAGE_FORMAT_LAYOUT_16_16";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_16_16_16_16) {
    return "XE_IMAGE_FORMAT_LAYOUT_16_16_16_16";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_32_32) {
    return "XE_IMAGE_FORMAT_LAYOUT_32_32";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_32_32_32_32) {
    return "XE_IMAGE_FORMAT_LAYOUT_32_32_32_32";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_10_10_10_2) {
    return "XE_IMAGE_FORMAT_LAYOUT_10_10_10_2";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_11_11_10) {
    return "XE_IMAGE_FORMAT_LAYOUT_11_11_10";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_5_6_5) {
    return "XE_IMAGE_FORMAT_LAYOUT_5_6_5";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_5_5_5_1) {
    return "XE_IMAGE_FORMAT_LAYOUT_5_5_5_1";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_4_4_4_4) {
    return "XE_IMAGE_FORMAT_LAYOUT_4_4_4_4";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_Y8) {
    return "XE_IMAGE_FORMAT_LAYOUT_Y8";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_NV12) {
    return "XE_IMAGE_FORMAT_LAYOUT_NV12";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_YUYV) {
    return "XE_IMAGE_FORMAT_LAYOUT_YUYV";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_VYUY) {
    return "XE_IMAGE_FORMAT_LAYOUT_VYUY";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_YVYU) {
    return "XE_IMAGE_FORMAT_LAYOUT_YVYU";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_UYVY) {
    return "XE_IMAGE_FORMAT_LAYOUT_UYVY";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_AYUV) {
    return "XE_IMAGE_FORMAT_LAYOUT_AYUV";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_YUAV) {
    return "XE_IMAGE_FORMAT_LAYOUT_YUAV";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_P010) {
    return "XE_IMAGE_FORMAT_LAYOUT_P010";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_Y410) {
    return "XE_IMAGE_FORMAT_LAYOUT_Y410";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_P012) {
    return "XE_IMAGE_FORMAT_LAYOUT_P012";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_Y16) {
    return "XE_IMAGE_FORMAT_LAYOUT_Y16";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_P016) {
    return "XE_IMAGE_FORMAT_LAYOUT_P016";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_Y216) {
    return "XE_IMAGE_FORMAT_LAYOUT_Y216";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_P216) {
    return "XE_IMAGE_FORMAT_LAYOUT_P216";
  } else if (layout == XE_IMAGE_FORMAT_LAYOUT_P416) {
    return "XE_IMAGE_FORMAT_LAYOUT_P416";
  } else {
    return "Unknown xe_image_format_layout_t value: " +
           std::to_string(static_cast<int>(layout));
  }
}

std::string to_string(const xe_image_format_type_t type) {
  if (type == XE_IMAGE_FORMAT_TYPE_UINT) {
    return "XE_IMAGE_FORMAT_TYPE_UINT";
  } else if (type == XE_IMAGE_FORMAT_TYPE_SINT) {
    return "XE_IMAGE_FORMAT_TYPE_SINT";
  } else if (type == XE_IMAGE_FORMAT_TYPE_UNORM) {
    return "XE_IMAGE_FORMAT_TYPE_UNORM";
  } else if (type == XE_IMAGE_FORMAT_TYPE_SNORM) {
    return "XE_IMAGE_FORMAT_TYPE_SNORM";
  } else if (type == XE_IMAGE_FORMAT_TYPE_FLOAT) {
    return "XE_IMAGE_FORMAT_TYPE_FLOAT";
  } else {
    return "Unknown xe_image_format_type_t value: " +
           std::to_string(static_cast<int>(type));
  }
}

std::string to_string(const xe_image_format_swizzle_t swizzle) {
  if (swizzle == XE_IMAGE_FORMAT_SWIZZLE_R) {
    return "XE_IMAGE_FORMAT_SWIZZLE_R";
  } else if (swizzle == XE_IMAGE_FORMAT_SWIZZLE_G) {
    return "XE_IMAGE_FORMAT_SWIZZLE_G";
  } else if (swizzle == XE_IMAGE_FORMAT_SWIZZLE_B) {
    return "XE_IMAGE_FORMAT_SWIZZLE_B";
  } else if (swizzle == XE_IMAGE_FORMAT_SWIZZLE_A) {
    return "XE_IMAGE_FORMAT_SWIZZLE_A";
  } else if (swizzle == XE_IMAGE_FORMAT_SWIZZLE_0) {
    return "XE_IMAGE_FORMAT_SWIZZLE_0";
  } else if (swizzle == XE_IMAGE_FORMAT_SWIZZLE_1) {
    return "XE_IMAGE_FORMAT_SWIZZLE_1";
  } else if (swizzle == XE_IMAGE_FORMAT_SWIZZLE_X) {
    return "XE_IMAGE_FORMAT_SWIZZLE_X";
  } else {
    return "Unknown xe_image_format_swizzle_t value: " +
           std::to_string(static_cast<int>(swizzle));
  }
}

std::string to_string(const xe_image_flag_t flag) {
  std::string flags = "";
  if (flag & XE_IMAGE_FLAG_PROGRAM_READ) {
    flags.append("|XE_IMAGE_FLAG_PROGRAM_READ|");
  }
  if (flag & XE_IMAGE_FLAG_PROGRAM_WRITE) {
    flags.append("|XE_IMAGE_FLAG_PROGRAM_WRITE|");
  }
  if (flag & XE_IMAGE_FLAG_BIAS_CACHED) {
    flags.append("|XE_IMAGE_FLAG_BIAS_CACHED|");
  }
  if (flag & XE_IMAGE_FLAG_BIAS_UNCACHED) {
    flags.append("|XE_IMAGE_FLAG_BIAS_UNCACHED|");
  }

  return flags;
}

std::string to_string(const xe_image_type_t type) {
  if (type == XE_IMAGE_TYPE_1D) {
    return "XE_IMAGE_TYPE_1D";
  } else if (type == XE_IMAGE_TYPE_2D) {
    return "XE_IMAGE_TYPE_2D";
  } else if (type == XE_IMAGE_TYPE_3D) {
    return "XE_IMAGE_TYPE_3D";
  } else if (type == XE_IMAGE_TYPE_1DARRAY) {
    return "XE_IMAGE_TYPE_1DARRAY";
  } else if (type == XE_IMAGE_TYPE_2DARRAY) {
    return "XE_IMAGE_TYPE_2DARRAY";
  } else {
    return "Unknown xe_image_type_t value: " +
           std::to_string(static_cast<int>(type));
  }
}

void print_header_version() {
  LOG_INFO << "API header version: " << XE_MAJOR_VERSION(XE_API_HEADER_VERSION)
           << "." << XE_MINOR_VERSION(XE_API_HEADER_VERSION);
}

void print_driver_version() {
  uint32_t version = 0;
  xe_device_group_handle_t dg = get_default_device_group();
  xe_result_t result = xeGetDriverVersion(&version);
  if (result) {
    std::runtime_error("xeDriverGetVersion failed: " + to_string(result));
  }
  LOG_TRACE << "Driver version retrieved";

  LOG_INFO << "Driver version: " << version;
}

void print_device_group_overview(const xe_device_group_handle_t device_group) {
  xe_result_t result = XE_RESULT_SUCCESS;

  xe_device_properties_t device_properties;
  device_properties.version = XE_DEVICE_PROPERTIES_VERSION_CURRENT;
  auto devices = get_devices(device_group);
  if (devices.size() > 0) {
    auto device = devices[0];
    result = xeDeviceGetProperties(device, &device_properties);
    if (result) {
      std::runtime_error("xeDeviceGetDeviceProperties failed: " +
                         to_string(result));
    }
    LOG_TRACE << "DeviceGroup properties retrieved";
    LOG_INFO << "DeviceGroup name: " << device_properties.name;
  }

  xe_api_version_t api_version;
  result = xeDeviceGroupGetApiVersion(device_group, &api_version);
  if (result) {
    throw std::runtime_error("xeDeviceGroupGetApiVersion failed: " +
                             to_string(result));
  }
  LOG_TRACE << "Device API version retrieved";

  LOG_INFO << "Device API version: " << to_string(api_version);
}

void print_device_group_overview(
    const std::vector<xe_device_group_handle_t> device_groups) {
  for (const xe_device_group_handle_t device_group : device_groups) {
    print_device_group_overview(device_group);
  }
}

void print_platform_overview() {
  LOG_INFO << "Platform overview";
  print_header_version();
  print_driver_version();

  const std::vector<xe_device_group_handle_t> device_groups =
      get_all_device_groups();
  LOG_INFO << "Device Group count: " << device_groups.size();

  print_device_group_overview(device_groups);
}

} // namespace level_zero_tests

std::ostream &operator<<(std::ostream &os, const xe_api_version_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const xe_result_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os,
                         const xe_command_queue_desc_version_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const xe_command_queue_flag_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const xe_command_queue_mode_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os,
                         const xe_command_queue_priority_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const xe_image_desc_version_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const xe_image_format_layout_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const xe_image_format_type_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const xe_image_format_swizzle_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const xe_image_flag_t &x) {
  return os << level_zero_tests::to_string(x);
}

std::ostream &operator<<(std::ostream &os, const xe_image_type_t &x) {
  return os << level_zero_tests::to_string(x);
}
