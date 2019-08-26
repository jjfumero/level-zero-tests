/*
 * Copyright(c) 2018 Intel Corporation
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

#include "ocl_utils/ocl_utils.hpp"
#include "ocl_utils/embargo/unified_shared_memory.hpp"
#include "gtest/gtest.h"
#include "utils/utils.hpp"

namespace lzt = level_zero_tests;

template <typename T> class SizeInBytes : public testing::Test {};
typedef testing::Types<cl_char, cl_uchar, cl_short, cl_ushort, cl_int, cl_uint,
                       cl_long, cl_ulong, cl_half, cl_float, cl_double>
    OpenCLTypes;
TYPED_TEST_CASE(SizeInBytes, OpenCLTypes);

TYPED_TEST(SizeInBytes, EmptyVector) {
  const std::vector<TypeParam> vector(0);
  EXPECT_EQ(0, lzt::size_in_bytes(vector));
}

TYPED_TEST(SizeInBytes, SingleElement) {
  const std::vector<TypeParam> vector(1);
  EXPECT_EQ(sizeof(TypeParam) * vector.size(), lzt::size_in_bytes(vector));
}

TYPED_TEST(SizeInBytes, MultipleElements) {
  const std::vector<TypeParam> vector(2);
  EXPECT_EQ(sizeof(TypeParam) * vector.size(), lzt::size_in_bytes(vector));
}

TEST(CompareCLVectors, cl_int8_equal) {
  const cl_int8 lhs = {0, 1, 2, 3, 4, 5, 6, 7};
  const cl_int8 rhs = lhs;
  EXPECT_TRUE(level_zero_tests::compare_cl_vectors(lhs, rhs));
}

TEST(CompareCLVectors, cl_int8_not_equal) {
  const cl_int8 lhs = {0, 1, 2, 3, 4, 5, 6, 7};
  const cl_int8 rhs = {1, 2, 3, 4, 5, 6, 7, 0};
  EXPECT_FALSE(level_zero_tests::compare_cl_vectors(lhs, rhs));
}

TEST(CompareCLVectors, cl_int4_equal) {
  const cl_int4 lhs = {0, 1, 2, 3};
  const cl_int4 rhs = lhs;
  EXPECT_TRUE(level_zero_tests::compare_cl_vectors(lhs, rhs));
}

TEST(CompareCLVectors, cl_int4_not_equal) {
  const cl_int4 lhs = {0, 1, 2, 3};
  const cl_int4 rhs = {1, 2, 3, 4};
  EXPECT_FALSE(level_zero_tests::compare_cl_vectors(lhs, rhs));
}

TEST(CompareCLVectors, cl_int2_equal) {
  const cl_int2 lhs = {0, 1};
  const cl_int2 rhs = lhs;
  EXPECT_TRUE(level_zero_tests::compare_cl_vectors(lhs, rhs));
}

TEST(CompareCLVectors, cl_int2_not_equal) {
  const cl_int2 lhs = {0, 1};
  const cl_int2 rhs = {1, 2};
  EXPECT_FALSE(level_zero_tests::compare_cl_vectors(lhs, rhs));
}

TEST(CLVectorToString, cl_int8) {
  const cl_int8 x = {0, 1, 2, 3, 4, 5, 6, 7};
  EXPECT_EQ("[0, 1, 2, 3, 4, 5, 6, 7]",
            level_zero_tests::cl_vector_to_string(x));
}

TEST(CLVectorToString, cl_int4) {
  const cl_int4 x = {0, 1, 2, 3};
  EXPECT_EQ("[0, 1, 2, 3]", level_zero_tests::cl_vector_to_string(x));
}

TEST(CLVectorToString, cl_int2) {
  const cl_int2 x = {0, 1};
  EXPECT_EQ("[0, 1]", level_zero_tests::cl_vector_to_string(x));
}

TEST(ToStringTest, UsmTypeHost) {
  const compute::usm_type t = compute::usm_type::host;
  const std::string expected = "host";
  EXPECT_EQ(expected, lzt::to_string(t));
}

TEST(ToStringTest, UsmTypeDevice) {
  const compute::usm_type t = compute::usm_type::device;
  const std::string expected = "device";
  EXPECT_EQ(expected, lzt::to_string(t));
}

TEST(ToStringTest, UsmTypeShared) {
  const compute::usm_type t = compute::usm_type::shared;
  const std::string expected = "shared";
  EXPECT_EQ(expected, lzt::to_string(t));
}
