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

typedef unsigned long uint64_t;
typedef unsigned uint32_t;
typedef unsigned short uint16_t;

kernel void fill_device_memory(__global uint64_t *pattern_memory,
                               const size_t pattern_memory_count,
                               const uint16_t pattern_base) {
  uint32_t i;

  /* fill memory with pattern */
  for (i = 0; i < pattern_memory_count; i++)
    pattern_memory[i] = (i << (sizeof(uint16_t) * 8)) + pattern_base;
}

/*
 * Verify pattern buffer against expected pattern.
 * In case of unexpected differences, use output buffers to record
 * some of those differences.
 */
kernel void test_device_memory(__global uint64_t *pattern_memory,
                               const size_t pattern_memory_count,
                               const uint16_t pattern_base,
                               __global uint64_t *expected_output,
                               __global uint64_t *found_output,
                               const size_t output_count) {
  uint32_t i, j;

  j = 0;
  for (i = 0; i < pattern_memory_count; i++) {
    if (pattern_memory[i] != (i << (sizeof(uint16_t) * 8)) + pattern_base) {
      if (j++ < output_count) {
        expected_output[j] = (i << (sizeof(uint16_t) * 8)) + pattern_base;
        found_output[j] = pattern_memory[i];
      }
    }
  }
}
