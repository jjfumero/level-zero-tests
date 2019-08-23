/*
 * INTEL CONFIDENTIAL
 *
 * Copyright 2019 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code (Material) are owned by Intel Corporation or its
 * suppliers or licensors.
 *
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors. The Material contains trade secrets and proprietary and
 * confidential information of Intel or its suppliers and licensors.
 * The Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed,
 * or disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure
 * or delivery of the Materials, either expressly, by implication, inducement,
 * estoppel or otherwise. Any license under such intellectual property rights
 * must be express and approved by Intel in writing.
 */

kernel void xe_module_add_constant(global int *values, int addval) {

  const int xid = get_global_id(0);
  const int yid = get_global_id(1);
  const int zid = get_global_id(2);
  const int xdim = get_global_size(0);
  const int ydim = get_global_size(1);
  const int zdim = get_global_size(2);
  atomic_add(&values[0], addval);
  if (((xid + 1) == xdim) && ((yid + 1) == ydim) && ((zid + 1) == zdim)) {
    printf("values[0] = %7d xid = %3d  yid = %3d  zid = %3d\n", values[0], xid,
           yid, zid);
  }
}

kernel void xe_module_add_two_arrays(global int *output, global int *input) {
  const int tid = get_global_id(0);
  output[tid] = output[tid] + input[tid];
  printf("output[%3d] = %5d\n", tid, output[tid]);
}
