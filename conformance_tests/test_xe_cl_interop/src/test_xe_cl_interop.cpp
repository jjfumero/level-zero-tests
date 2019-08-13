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

#include "gtest/gtest.h"

#include "xe_utils/xe_utils.hpp"
#include "xe_test_harness/xe_test_harness.hpp"
#include "logging/logging.hpp"
#include <boost/compute/core.hpp>
#include "boost/compute/intel/context.hpp"
#include "boost/compute/intel/device.hpp"

namespace cs = compute_samples;

#include "xe_driver.h"
#include "xe_cl_interop.h"

#include "boost/compute/system.hpp"
namespace compute = boost::compute;
#include "ocl_utils/ocl_utils.hpp"

namespace {

class xeCLInteropTests : public ::testing::Test {
protected:
  void SetUp() override {
    cl_context_ = compute::system::default_context();
    cl_command_queue_ = compute::system::default_queue();
    compute::device device = compute::system::default_device();
    const std::vector<cl_mem_properties_intel> properties = {
        CL_MEM_ALLOC_FLAGS_INTEL, CL_MEM_ALLOC_DEFAULT_INTEL, 0};
    cl_int ret;
    static char hostBufA[1024];
    cl_memory_ =
        clCreateBuffer(cl_context_, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                       sizeof(hostBufA), hostBufA, &ret);
    EXPECT_NE(nullptr, cl_memory_);
    EXPECT_EQ(0, ret);

    const char *KernelSource = {
        "\n"
        "__kernel void square( __global float* input, __global float* output, "
        "\n"
        " const unsigned int count) {            \n"
        " int i = get_global_id(0);              \n"
        " if(i < count) \n"
        " output[i] = input[i] * input[i] * input[i]; \n"
        "}                     \n"};
    cl_program_ = clCreateProgramWithSource(
        cl_context_, 1, (const char **)&KernelSource, NULL, &ret);
    EXPECT_NE(nullptr, cl_program_);
    EXPECT_EQ(0, ret);
    ret = clBuildProgram(cl_program_, 1, &device.get(), NULL, NULL, NULL);
    EXPECT_EQ(0, ret);
  }
  void TearDown() override { clReleaseMemObject(cl_memory_); }
  compute::context cl_context_;
  compute::command_queue cl_command_queue_;
  cl_mem cl_memory_;
  cl_program cl_program_;
};

class xeDeviceRegisterCLCommandQueueTests : public xeCLInteropTests {};

TEST_F(
    xeDeviceRegisterCLCommandQueueTests,
    GivenOpenCLContextAndOpenCLCommandQueueWhenRegisteringOpenCLCommandQueueThenNotNullCommandQueueIsReturned) {
  xe_command_queue_handle_t command_queue = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceRegisterCLCommandQueue(
                cs::xeDevice::get_instance()->get_device(), cl_context_.get(),
                cl_command_queue_.get(), &command_queue));
  EXPECT_NE(nullptr, command_queue);
}

class xeDeviceRegisterCLProgramTests : public xeCLInteropTests {};

TEST_F(
    xeDeviceRegisterCLProgramTests,
    GivenOpenCLContextAndOpenCLProgramWhenRegisteringOpenCLProgramThenNotNullModuleHandleIsReturned) {
  xe_module_handle_t module_handle = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS, xeDeviceRegisterCLProgram(
                                   cs::xeDevice::get_instance()->get_device(),
                                   cl_context_, cl_program_, &module_handle));
  EXPECT_NE(nullptr, module_handle);
}

class xeDeviceRegisterCLMemoryTests : public xeCLInteropTests {};

TEST_F(
    xeDeviceRegisterCLMemoryTests,
    GivenOpenCLContextAndCLDeviceMemoryWhenRegisteringCLMemoryThenNotNullMemoryIsReturned) {
  void *ptr = nullptr;
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeDeviceRegisterCLMemory(cs::xeDevice::get_instance()->get_device(),
                                     cl_context_.get(), cl_memory_, &ptr));
  EXPECT_NE(nullptr, ptr);
}

} // namespace

// TODO: Test CL programs
// TODO: Test CL memory objects
// TODO: Test Xe/CL interoperability on API level
// TODO: Test Xe/CL interoperability on device level
