/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2016 - 2019 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to the
 * source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 */

#undef FETCH_2
#undef FETCH_8

#define FETCH_2(sum, id, A, jumpBy)      sum += A[id];   id += jumpBy;   sum += A[id];   id += jumpBy;
#define FETCH_4(sum, id, A, jumpBy)      FETCH_2(sum, id, A, jumpBy);   FETCH_2(sum, id, A, jumpBy);
#define FETCH_8(sum, id, A, jumpBy)      FETCH_4(sum, id, A, jumpBy);   FETCH_4(sum, id, A, jumpBy);

#define FETCH_PER_WI  16

// Kernels fetching by local_size offset
__kernel void global_bandwidth_v1_local_offset(__global float *A, __global float *B)
{
    int id = (get_group_id(0) * get_local_size(0) * FETCH_PER_WI) + get_local_id(0);
    float sum = 0;

    FETCH_8(sum, id, A, get_local_size(0));
    FETCH_8(sum, id, A, get_local_size(0));

    B[get_global_id(0)] = sum;
}


__kernel void global_bandwidth_v2_local_offset(__global float2 *A, __global float *B)
{
    int id = (get_group_id(0) * get_local_size(0) * FETCH_PER_WI) + get_local_id(0);
    float2 sum = 0;

    FETCH_8(sum, id, A, get_local_size(0));
    FETCH_8(sum, id, A, get_local_size(0));

    B[get_global_id(0)] = (sum.S0) + (sum.S1);
}


__kernel void global_bandwidth_v4_local_offset(__global float4 *A, __global float *B)
{
    int id = (get_group_id(0) * get_local_size(0) * FETCH_PER_WI) + get_local_id(0);
    float4 sum = 0;

    FETCH_8(sum, id, A, get_local_size(0));
    FETCH_8(sum, id, A, get_local_size(0));

    B[get_global_id(0)] = (sum.S0) + (sum.S1) + (sum.S2) + (sum.S3);
}


__kernel void global_bandwidth_v8_local_offset(__global float8 *A, __global float *B)
{
    int id = (get_group_id(0) * get_local_size(0) * FETCH_PER_WI) + get_local_id(0);
    float8 sum = 0;

    FETCH_8(sum, id, A, get_local_size(0));
    FETCH_8(sum, id, A, get_local_size(0));

    B[get_global_id(0)] = (sum.S0) + (sum.S1) + (sum.S2) + (sum.S3) + (sum.S4) + (sum.S5) + (sum.S6) + (sum.S7);
}

__kernel void global_bandwidth_v16_local_offset(__global float16 *A, __global float *B)
{
    int id = (get_group_id(0) * get_local_size(0) * FETCH_PER_WI) + get_local_id(0);
    float16 sum = 0;

    FETCH_8(sum, id, A, get_local_size(0));
    FETCH_8(sum, id, A, get_local_size(0));

    float t = (sum.S0) + (sum.S1) + (sum.S2) + (sum.S3) + (sum.S4) + (sum.S5) + (sum.S6) + (sum.S7);
    t += (sum.S8) + (sum.S9) + (sum.SA) + (sum.SB) + (sum.SC) + (sum.SD) + (sum.SE) + (sum.SF);
    B[get_global_id(0)] = t;
}


// Kernels fetching by global_size offset
__kernel void global_bandwidth_v1_global_offset(__global float *A, __global float *B)
{
    int id = get_global_id(0);
    float sum = 0;

    FETCH_8(sum, id, A, get_global_size(0));
    FETCH_8(sum, id, A, get_global_size(0));

    B[get_global_id(0)] = sum;
}


__kernel void global_bandwidth_v2_global_offset(__global float2 *A, __global float *B)
{
    int id = get_global_id(0);
    float2 sum = 0;

    FETCH_8(sum, id, A, get_global_size(0));
    FETCH_8(sum, id, A, get_global_size(0));

    B[get_global_id(0)] = (sum.S0) + (sum.S1);
}


__kernel void global_bandwidth_v4_global_offset(__global float4 *A, __global float *B)
{
    int id = get_global_id(0);
    float4 sum = 0;

    FETCH_8(sum, id, A, get_global_size(0));
    FETCH_8(sum, id, A, get_global_size(0));

    B[get_global_id(0)] = (sum.S0) + (sum.S1) + (sum.S2) + (sum.S3);
}


__kernel void global_bandwidth_v8_global_offset(__global float8 *A, __global float *B)
{
    int id = get_global_id(0);
    float8 sum = 0;

    FETCH_8(sum, id, A, get_global_size(0));
    FETCH_8(sum, id, A, get_global_size(0));

    B[get_global_id(0)] = (sum.S0) + (sum.S1) + (sum.S2) + (sum.S3) + (sum.S4) + (sum.S5) + (sum.S6) + (sum.S7);
}

__kernel void global_bandwidth_v16_global_offset(__global float16 *A, __global float *B)
{
    int id = get_global_id(0);
    float16 sum = 0;

    FETCH_8(sum, id, A, get_global_size(0));
    FETCH_8(sum, id, A, get_global_size(0));

    float t = (sum.S0) + (sum.S1) + (sum.S2) + (sum.S3) + (sum.S4) + (sum.S5) + (sum.S6) + (sum.S7);
    t += (sum.S8) + (sum.S9) + (sum.SA) + (sum.SB) + (sum.SC) + (sum.SD) + (sum.SE) + (sum.SF);
    B[get_global_id(0)] = t;
}

