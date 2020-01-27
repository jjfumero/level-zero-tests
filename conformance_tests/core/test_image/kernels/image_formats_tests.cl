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

const sampler_t image_sampler =
    CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;



kernel void image_format_uint(read_only image2d_t image_in, write_only image2d_t image_out) {
  const int2 coord = {get_global_id(0), get_global_id(1)};
  uint4 pixel = read_imageui(image_in, coord);
  pixel.x++;
  write_imageui(image_out, coord, pixel);
}


kernel void image_format_int(read_only image2d_t image_in, write_only image2d_t image_out) {
  const int2 coord = {get_global_id(0), get_global_id(1)};
  int4 pixel = read_imagei(image_in, coord);
  pixel.x--;
  write_imagei(image_out, coord, pixel);
}


kernel void image_format_float(read_only image2d_t image_in, write_only image2d_t image_out) {
  const int2 coord = {get_global_id(0), get_global_id(1)};
  float4 pixel = read_imagef(image_in, image_sampler, coord);
  pixel.x-= 0.123;
  write_imagef(image_out, coord, pixel);
}


kernel void image_format_unorm(read_only image2d_t image_in, write_only image2d_t image_out) {
  const int2 coord = {get_global_id(0), get_global_id(1)};
  float4 pixel = read_imagef(image_in, image_sampler, coord);
  pixel.x+= 1.0;
  write_imagef(image_out, coord, pixel);
}

kernel void image_format_snorm(read_only image2d_t image_in, write_only image2d_t image_out) {
  const int2 coord = {get_global_id(0), get_global_id(1)};
  float4 pixel = read_imagef(image_in, image_sampler, coord);
  pixel.x+= 2.0;
  write_imagef(image_out, coord, pixel);
}
