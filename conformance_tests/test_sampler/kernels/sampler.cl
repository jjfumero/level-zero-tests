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


const sampler_t sampler_kernel =
    CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

kernel void sampler_inkernel(read_only image2d_t input,
                             write_only image2d_t output) {
  const int2 output_coord = {get_global_id(0), get_global_id(1)};
  const int2 output_size = {get_global_size(0), get_global_size(1)};

  const float pixel_center = 0.5f;
  const float2 input_coord = (convert_float2(output_coord) + pixel_center) /
                             convert_float2(output_size);
  const float4 pixel = read_imagef(input, sampler_kernel, input_coord);
  write_imagef(output, output_coord, pixel);
}

kernel void sampler_inhost(read_only image2d_t input,
                             write_only image2d_t output, sampler_t sampler_host ) {
  const int2 output_coord = {get_global_id(0), get_global_id(1)};
  const int2 output_size = {get_global_size(0), get_global_size(1)};

  const float pixel_center = 0.5f;
  const float2 input_coord = (convert_float2(output_coord) + pixel_center) /
                             convert_float2(output_size);
  const float4 pixel = read_imagef(input, sampler_host, input_coord);
  write_imagef(output, output_coord, pixel);
}


kernel void sampler_noop(sampler_t sampler) {



}


