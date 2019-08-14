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

kernel void function_parameter_buffers(global char *input_a,
                                       global char *input_b,
                                       global char *input_c,
                                       global char *input_d,
                                       global char *input_e,
                                       global char *input_f) {
}

kernel void function_parameter_integer(int a, int b, int c, int e, int f,
                                       int g) {
}

kernel void function_parameter_image(image2d_t input_a, image2d_t input_b,
                                     image2d_t input_c, image2d_t input_d,
                                     image2d_t input_e, image2d_t input_f) {
}

kernel void function_no_parameter() {
}
