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

#ifndef CL_INTEL_FUNCTION_POINTERS_H
#define CL_INTEL_FUNCTION_POINTERS_H

#include <CL/cl.h>

typedef CL_API_ENTRY cl_int(CL_API_CALL *clGetDeviceFunctionPointerINTEL_fn)(
    cl_device_id device, cl_program program, const char *function_name,
    cl_ulong *function_pointer_ret) CL_API_SUFFIX__VERSION_1_2;

extern CL_API_ENTRY cl_int CL_API_CALL clGetDeviceFunctionPointerINTEL(
    cl_device_id device, cl_program program, const char *function_name,
    cl_ulong *function_pointer_ret) CL_API_SUFFIX__VERSION_1_2;

#endif