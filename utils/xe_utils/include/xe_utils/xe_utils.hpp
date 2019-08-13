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

#ifndef COMPUTE_SAMPLES_XE_UTILS_HPP
#define COMPUTE_SAMPLES_XE_UTILS_HPP

#include <vector>
#include <string>

#include "xe_driver.h"
#include "xe_device.h"
#include "xe_cmdqueue.h"
#include "xe_image.h"

namespace compute_samples {

xe_device_handle_t get_default_device();
xe_device_group_handle_t get_default_device_group();
std::vector<xe_device_handle_t>
get_devices(xe_device_group_handle_t device_group);
std::vector<xe_device_group_handle_t> get_all_device_groups();

uint32_t get_device_group_count();
uint32_t get_device_count(xe_device_group_handle_t device_group);
uint32_t get_sub_device_count(xe_device_handle_t device);

std::string to_string(const xe_api_version_t version);
std::string to_string(const xe_result_t result);
std::string to_string(const xe_command_queue_desc_version_t version);
std::string to_string(const xe_command_queue_flag_t flags);
std::string to_string(const xe_command_queue_mode_t mode);
std::string to_string(const xe_command_queue_priority_t priority);
std::string to_string(const xe_image_format_layout_t layout);
std::string to_string(const xe_image_format_type_t type);
std::string to_string(const xe_image_format_swizzle_t swizzle);
std::string to_string(const xe_image_flag_t flag);
std::string to_string(const xe_image_type_t type);
std::string to_string(const xe_image_desc_version_t version);

void print_header_version();
void print_driver_version();
void print_device_overview(const xe_device_handle_t device);
void print_device_overview(const std::vector<xe_device_handle_t> devices);
void print_platform_overview();

} // namespace compute_samples

std::ostream &operator<<(std::ostream &os, const xe_api_version_t &x);
std::ostream &operator<<(std::ostream &os, const xe_result_t &x);
std::ostream &operator<<(std::ostream &os,
                         const xe_command_queue_desc_version_t &x);
std::ostream &operator<<(std::ostream &os, const xe_command_queue_flag_t &x);
std::ostream &operator<<(std::ostream &os, const xe_command_queue_mode_t &x);
std::ostream &operator<<(std::ostream &os,
                         const xe_command_queue_priority_t &x);
std::ostream &operator<<(std::ostream &os, const xe_image_desc_version_t &x);
std::ostream &operator<<(std::ostream &os, const xe_image_format_layout_t &x);
std::ostream &operator<<(std::ostream &os, const xe_image_format_type_t &x);
std::ostream &operator<<(std::ostream &os, const xe_image_format_swizzle_t &x);
std::ostream &operator<<(std::ostream &os, const xe_image_flag_t &x);
std::ostream &operator<<(std::ostream &os, const xe_image_type_t &x);

#endif
