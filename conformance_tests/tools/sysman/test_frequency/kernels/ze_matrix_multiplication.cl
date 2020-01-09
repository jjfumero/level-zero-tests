/*
 * Copyright(c) 2020 Intel Corporation
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

// Matrix A has M rows and K columns
// Matrix B has K rows and N columns
// Matrix C has M rows and N columns

#define TILE_SIZE 16
__attribute__((reqd_work_group_size(TILE_SIZE, TILE_SIZE, 1))) kernel void
xe_matrix_multiplication(const global float *a, const global float *b,
                         const int m, const int k, const int n,
                         global float *c) {
  const int2 global_id = {get_global_id(0), get_global_id(1)};
  const int2 local_id = {get_local_id(0), get_local_id(1)};

  local float a_tile[TILE_SIZE * TILE_SIZE];
  local float b_tile[TILE_SIZE * TILE_SIZE];

  float sum = 0.0f;
  for (int tile_id = 0; tile_id < k / TILE_SIZE; ++tile_id) {
    a_tile[local_id.y * TILE_SIZE + local_id.x] =
        a[(tile_id * TILE_SIZE + local_id.y) * m + global_id.x];
    b_tile[local_id.y * TILE_SIZE + local_id.x] =
        b[global_id.y * k + (tile_id * TILE_SIZE + local_id.x)];
    barrier(CLK_LOCAL_MEM_FENCE);

    for (int i = 0; i < TILE_SIZE; ++i) {
      sum += a_tile[i * TILE_SIZE + local_id.x] *
             b_tile[local_id.y * TILE_SIZE + i];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
  }

  c[global_id.y * m + global_id.x] = sum;
}
