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

#ifndef COMPUTE_SAMPLES_OCL_UTILS_UNIFIED_SHARED_MEMORY_HPP
#define COMPUTE_SAMPLES_OCL_UTILS_UNIFIED_SHARED_MEMORY_HPP

#include <iostream>
#include <string>

#include "ocl_entrypoints/embargo/cl_intel_unified_shared_memory.h"

namespace compute_samples {
enum class usm_type {
  host = CL_MEM_TYPE_HOST_INTEL,
  device = CL_MEM_TYPE_DEVICE_INTEL,
  shared = CL_MEM_TYPE_SHARED_INTEL,
  unknown = CL_MEM_TYPE_UNKNOWN_INTEL
};
std::string to_string(const usm_type &x);
std::ostream &operator<<(std::ostream &os, const usm_type &x);
std::ostream &operator<<(std::ostream &os, const usm_type &x);
std::istream &operator>>(std::istream &is, usm_type &x);
} // namespace compute_samples

#endif
