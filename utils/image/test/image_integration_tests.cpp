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

#include "image/image.hpp"
#include "gtest/gtest.h"

TEST(ImageIntegrationTests, ReadsPNGFile) {
  level_zero_tests::ImagePNG32Bit image("rgb_brg_3x2.png");
  const std::vector<uint32_t> pixels = {
      0xFF0000FF, //
      0x00FF00FF, //
      0x0000FFFF, //
      0x0000FFFF, //
      0xFF0000FF, //
      0x00FF00FF  //
  };
  EXPECT_EQ(image.get_pixels(), pixels);
}

TEST(ImageIntegrationTests, WritesPNGFile) {
  const std::vector<uint32_t> pixels = {
      0xFF0000FF, //
      0x00FF00FF, //
      0x0000FFFF, //
      0x0000FFFF, //
      0xFF0000FF, //
      0x00FF00FF  //
  };
  level_zero_tests::ImagePNG32Bit image(3, 2);
  image.write("output.png", pixels.data());

  level_zero_tests::ImagePNG32Bit output("output.png");
  EXPECT_EQ(output.get_pixels(), pixels);
  std::remove("output.png");

  EXPECT_EQ(image.get_pixels(), pixels);
}

TEST(ImageIntegrationTests, ReadsGrayscaleBMPFile) {
  level_zero_tests::ImageBMP8Bit image("kwkw_wwkk_4x2_mono.bmp");
  const std::vector<uint8_t> pixels = {
      0x00, //
      0xFF, //
      0x00, //
      0xFF, //
      0xFF, //
      0xFF, //
      0x00, //
      0x00  //
  };
  EXPECT_EQ(image.get_pixels(), pixels);
}

TEST(ImageIntegrationTests, WritesGrayscaleBMPFile) {
  const std::vector<uint8_t> pixels = {
      0x00, //
      0xFF, //
      0x00, //
      0xFF, //
      0xFF, //
      0xFF, //
      0x00, //
      0x00  //
  };
  level_zero_tests::ImageBMP8Bit image(4, 2);
  image.write("output.bmp", pixels.data());

  level_zero_tests::ImageBMP8Bit output("output.bmp");
  EXPECT_EQ(output.get_pixels(), pixels);
  std::remove("output.bmp");

  EXPECT_EQ(image.get_pixels(), pixels);
}

TEST(ImageIntegrationTests, ReadsColorBMPFile) {
  level_zero_tests::ImageBMP32Bit image("rgb_brg_3x2_argb.bmp");
  const std::vector<uint32_t> pixels = {
      0xFFFF0000, //
      0xFF00FF00, //
      0xFF0000FF, //
      0xFF0000FF, //
      0xFFFF0000, //
      0xFF00FF00  //
  };
  EXPECT_EQ(image.get_pixels(), pixels);
}

TEST(ImageIntegrationTests, WritesColorBMPFile) {
  const std::vector<uint32_t> pixels = {
      0xFFFF0000, //
      0xFF00FF00, //
      0xFF0000FF, //
      0xFF0000FF, //
      0xFFFF0000, //
      0xFF00FF00  //
  };
  level_zero_tests::ImageBMP32Bit image(3, 2);
  image.write("output.bmp", pixels.data());

  level_zero_tests::ImageBMP32Bit output("output.bmp");
  EXPECT_EQ(output.get_pixels(), pixels);
  std::remove("output.bmp");

  EXPECT_EQ(image.get_pixels(), pixels);
}
