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
#include "test_harness/test_harness.hpp"
#include "gtest/gtest.h"
#include "logging/logging.hpp"

#include "ze_api.h"

namespace lzt = level_zero_tests;

namespace level_zero_tests {

zet_tracer_handle_t create_tracer_handle(const zet_tracer_desc_t tracer_desc) {
  return create_tracer_handle(zeDevice::get_instance()->get_driver(),
                              tracer_desc);
}

zet_tracer_handle_t create_tracer_handle(const ze_driver_handle_t driver,
                                         const zet_tracer_desc_t tracer_desc) {
  zet_tracer_handle_t tracer_handle;

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zetTracerCreate(driver, &tracer_desc, &tracer_handle));

  return tracer_handle;
}

void destroy_tracer_handle(zet_tracer_handle_t tracer_handle) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zetTracerDestroy(tracer_handle));
}

void set_tracer_prologues(zet_tracer_handle_t tracer_handle,
                          zet_core_callbacks_t prologues) {
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zetTracerSetPrologues(tracer_handle, &prologues, nullptr));
}

void set_tracer_epilogues(const zet_tracer_handle_t tracer_handle,
                          zet_core_callbacks_t epilogues) {
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zetTracerSetEpilogues(tracer_handle, &epilogues, nullptr));
}

void enable_tracer(zet_tracer_handle_t tracer_handle) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zetTracerSetEnabled(tracer_handle, true));
}

void disable_tracer(zet_tracer_handle_t tracer_handle) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zetTracerSetEnabled(tracer_handle, false));
}

} // namespace level_zero_tests
