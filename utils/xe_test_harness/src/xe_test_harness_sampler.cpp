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
#include "xe_test_harness/xe_test_harness.hpp"

namespace level_zero_tests {

xe_sampler_handle_t create_sampler(xe_sampler_address_mode_t addrmode,
                                   xe_sampler_filter_mode_t filtmode,
                                   xe_bool_t normalized) {
  xe_sampler_desc_t descriptor;
  descriptor.version = XE_SAMPLER_DESC_VERSION_CURRENT;
  descriptor.addressMode = addrmode;
  descriptor.filterMode = filtmode;
  descriptor.isNormalized = normalized;

  xe_sampler_handle_t sampler = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeSamplerCreate(xeDevice::get_instance()->get_device(), &descriptor,
                            &sampler));
  EXPECT_NE(nullptr, sampler);
  return sampler;
}

xe_sampler_handle_t create_sampler() {
  return create_sampler(XE_SAMPLER_ADDRESS_MODE_NONE,
                        XE_SAMPLER_FILTER_MODE_NEAREST, false);
}

void destroy_sampler(xe_sampler_handle_t sampler) {

  EXPECT_EQ(XE_RESULT_SUCCESS, xeSamplerDestroy(sampler));
}

}; // namespace level_zero_tests
