/*
 *
 * Copyright (C) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#ifndef level_zero_tests_BMP_HPP
#define level_zero_tests_BMP_HPP

#include <cstdint>
#include <stddef.h>

namespace level_zero_tests {

class BmpUtils {
public:
  static bool save_image_as_bmp(uint32_t *ptr, int width, int height,
                                const char *file_name);
  static bool save_image_as_bmp_32fc4(float *ptr, float scale, int width,
                                      int height, const char *file_name);
  static bool save_image_as_bmp_8u(uint8_t *ptr, int width, int height,
                                   const char *file_name);

  static bool load_bmp_image(uint8_t *&data, int &width, int &height,
                             int &pitch, uint16_t &bits_per_pixel,
                             const char *file_name);
  static bool load_bmp_image_8u(uint8_t *&data, int &width, int &height,
                                const char *file_name);
};
} // namespace level_zero_tests

#endif
