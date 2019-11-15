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

kernel void unified_mem_test(global char *data, int size) {
  int i, errCnt = 0;
  char dp = 1;

  /* First, read the data buffer, and verify it has the expected data pattern.
     Count the errors found: */
  for (i = 0; i < size; i++) {
    if (data[i] != dp)
      errCnt++;
    dp = (dp + 1) & 0xff;
  }
  /* If we successfully validated the data buffer then write a new data pattern
     back. The caller of this function will confirm the following data pattern
     after the
     the device function completes, to ensure validation was correct. */
  if (errCnt == 0) {
    dp = -1;
    for (i = 0; i < size; i++) {
      data[i] = dp;
      dp = (dp - 1) & 0xff;
    }
  }
}
