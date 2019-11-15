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

kernel void bitonic_sort(global int *a, const int p, const int q) {
  const int i = get_global_id(0);
  const int d = (1 << (p - q));

  if ((i & d) == 0) {
    const int up = ((i >> p) & 2) == 0;
    const int a1 = a[i];
    const int a2 = a[i | d];

    if ((a1 > a2) == up) {
      a[i] = a2;
      a[i | d] = a1;
    }
  }
}
