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

#include "gtest/gtest.h"

#include "logging/logging.hpp"
#include "test_harness/test_harness.hpp"
#include "ze_api.h"
#include "zet_api.h"

namespace lzt = level_zero_tests;

namespace {

class ModuleGetKernelNamesTests
    : public ::testing::Test,
      public ::testing::WithParamInterface<uint32_t> {};

TEST_P(
    ModuleGetKernelNamesTests,
    GivenValidModuleWhenGettingKernelNamesThenCorrectKernelNumberAndNamesAreReturned) {
  int num = GetParam();
  uint32_t kernel_count = 0;
  const ze_device_handle_t device = lzt::zeDevice::get_instance()->get_device();
  std::string filename =
      std::to_string(num) + "kernel" + (num == 1 ? "" : "s") + ".spv";
  ze_module_handle_t module = lzt::create_module(device, filename);
  std::vector<const char *> names(num);

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zetModuleGetKernelNames(module, &kernel_count, nullptr));
  EXPECT_EQ(kernel_count, num);
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zetModuleGetKernelNames(module, &kernel_count, names.data()));

  LOG_DEBUG << kernel_count << " Kernels in Module:";
  for (uint32_t i = 0; i < kernel_count; i++) {
    LOG_DEBUG << "\t" << names[i];
    EXPECT_EQ(names[i], "kernel" + std::to_string(i + 1));
  }

  lzt::destroy_module(module);
}

INSTANTIATE_TEST_CASE_P(ModuleGetKernelNamesParamTests,
                        ModuleGetKernelNamesTests,
                        ::testing::Values(0, 1, 10, 100, 1000));

} // namespace
