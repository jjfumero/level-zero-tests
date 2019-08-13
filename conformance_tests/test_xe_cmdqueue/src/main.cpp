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

#include "gmock/gmock.h"
#include "logging/logging.hpp"
#include "xe_utils/xe_utils.hpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleMock(&argc, argv);
  std::vector<std::string> command_line(argv + 1, argv + argc);
  compute_samples::init_logging(command_line);

  xe_result_t result = xeInit(XE_INIT_FLAG_NONE);
  if (result) {
    throw std::runtime_error("xeInit failed: " +
                             compute_samples::to_string(result));
  }
  LOG_TRACE << "Driver initialized";
  compute_samples::print_platform_overview();

  return RUN_ALL_TESTS();
}
