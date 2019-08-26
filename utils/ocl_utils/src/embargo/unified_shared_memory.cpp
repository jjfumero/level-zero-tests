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

#include "ocl_utils/embargo/unified_shared_memory.hpp"

namespace level_zero_tests {
std::string to_string(const compute::usm_type &x) {
  if (x == compute::usm_type::host) {
    return "host";
  } else if (x == compute::usm_type::device) {
    return "device";
  } else if (x == compute::usm_type::shared) {
    return "shared";
  } else {
    return "unknown";
  }
}
} // namespace level_zero_tests

namespace boost {
namespace compute {
std::ostream &operator<<(std::ostream &os, const compute::usm_type &x) {
  return os << level_zero_tests::to_string(x);
}

std::istream &operator>>(std::istream &is, compute::usm_type &x) {
  std::string s = "";
  is >> s;
  if (s == "host") {
    x = compute::usm_type::host;
  } else if (s == "device") {
    x = compute::usm_type::device;
  } else if (s == "shared") {
    x = compute::usm_type::shared;
  } else {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}
} // namespace compute
} // namespace boost
