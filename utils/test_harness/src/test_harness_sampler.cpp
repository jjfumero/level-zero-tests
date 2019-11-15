/*
 * Copyright(c) 2019 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "gtest/gtest.h"
#include "test_harness/test_harness.hpp"

namespace level_zero_tests {

ze_sampler_handle_t create_sampler(ze_sampler_address_mode_t addrmode,
                                   ze_sampler_filter_mode_t filtmode,
                                   ze_bool_t normalized) {
  ze_sampler_desc_t descriptor;
  descriptor.version = ZE_SAMPLER_DESC_VERSION_CURRENT;
  descriptor.addressMode = addrmode;
  descriptor.filterMode = filtmode;
  descriptor.isNormalized = normalized;

  ze_sampler_handle_t sampler = nullptr;
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeSamplerCreate(zeDevice::get_instance()->get_device(), &descriptor,
                            &sampler));
  EXPECT_NE(nullptr, sampler);
  return sampler;
}

ze_sampler_handle_t create_sampler() {
  return create_sampler(ZE_SAMPLER_ADDRESS_MODE_NONE,
                        ZE_SAMPLER_FILTER_MODE_NEAREST, false);
}

void destroy_sampler(ze_sampler_handle_t sampler) {

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeSamplerDestroy(sampler));
}

}; // namespace level_zero_tests
