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

#include "utils/utils.hpp"
#include "xe_utils/xe_utils.hpp"
#include "xe_test_harness/xe_test_harness.hpp"
#include "logging/logging.hpp"

namespace lzt = level_zero_tests;

#include "xe_copy.h"
#include "xe_driver.h"
#include "xe_memory.h"
#include "xe_event.h"
#include "xe_barrier.h"

namespace {

class xeCommandListEventTests : public ::testing::Test {
protected:
  xeCommandListEventTests() {
    ep.create_event(hEvent, XE_EVENT_SCOPE_FLAG_HOST, XE_EVENT_SCOPE_FLAG_NONE);
    cmdqueue = lzt::create_command_queue();
    cmdlist = lzt::create_command_list();
  }

  ~xeCommandListEventTests() {
    ep.destroy_event(hEvent);
    lzt::destroy_command_queue(cmdqueue);
    lzt::destroy_command_list(cmdlist);
  }

  xe_command_list_handle_t cmdlist;
  xe_command_queue_handle_t cmdqueue;
  xe_event_handle_t hEvent;
  size_t size = 16;
  lzt::xeEventPool ep;
};

TEST_F(
    xeCommandListEventTests,
    GivenMemoryCopyThatSignalsEventWhenCompleteWhenExecutingCommandListThenHostAndGpuReadEventCorrectly) {

  auto src_buffer = lzt::allocate_shared_memory(size);
  auto dst_buffer = lzt::allocate_shared_memory(size);
  memset(src_buffer, 0x1, size);
  memset(dst_buffer, 0x0, size);

  // Verify Host Reads Event as unset
  EXPECT_EQ(XE_RESULT_NOT_READY, xeEventHostSynchronize(hEvent, 0));

  // Execute and verify GPU reads event
  lzt::append_memory_copy(cmdlist, dst_buffer, src_buffer, size, hEvent, 0,
                          nullptr);
  lzt::append_wait_on_events(cmdlist, 1, &hEvent);
  lzt::close_command_list(cmdlist);
  lzt::execute_command_lists(cmdqueue, 1, &cmdlist, nullptr);

  // Verify Host Reads Event as set
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSynchronize(hEvent, UINT32_MAX));

  // Verify Memory Copy completed
  EXPECT_EQ(0, memcmp(src_buffer, dst_buffer, size));

  lzt::synchronize(cmdqueue, UINT32_MAX);
  lzt::free_memory(src_buffer);
  lzt::free_memory(dst_buffer);
}

TEST_F(
    xeCommandListEventTests,
    GivenMemorySetThatSignalsEventWhenCompleteWhenExecutingCommandListThenHostAndGpuReadEventCorrectly) {

  auto ref_buffer = lzt::allocate_shared_memory(size);
  auto dst_buffer = lzt::allocate_shared_memory(size);
  memset(ref_buffer, 0x1, size);
  memset(dst_buffer, 0x0, size);

  // Verify Host Reads Event as unset
  EXPECT_EQ(XE_RESULT_NOT_READY, xeEventHostSynchronize(hEvent, 0));

  // Execute and verify GPU reads event
  lzt::append_memory_set(cmdlist, dst_buffer, 0x1, size, hEvent, 0, nullptr);
  lzt::append_wait_on_events(cmdlist, 1, &hEvent);
  lzt::close_command_list(cmdlist);
  lzt::execute_command_lists(cmdqueue, 1, &cmdlist, nullptr);

  // Verify Host Reads Event as set
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSynchronize(hEvent, UINT32_MAX));

  // Verify Memory Set completed
  EXPECT_EQ(0, memcmp(ref_buffer, dst_buffer, size));

  lzt::synchronize(cmdqueue, UINT32_MAX);
  lzt::free_memory(ref_buffer);
  lzt::free_memory(dst_buffer);
}

TEST_F(
    xeCommandListEventTests,
    GivenMemoryCopyRegionThatSignalsEventWhenCompleteWhenExecutingCommandListThenHostAndGpuReadEventCorrectly) {

  size_t width = 16;
  size_t height = 16;
  size = height * width;
  auto src_buffer = lzt::allocate_shared_memory(size);
  auto dst_buffer = lzt::allocate_shared_memory(size);
  memset(src_buffer, 0x1, size);
  memset(dst_buffer, 0x0, size);

  // Verify Host Reads Event as unset
  EXPECT_EQ(XE_RESULT_NOT_READY, xeEventHostSynchronize(hEvent, 0));

  // Execute and verify GPU reads event
  xe_copy_region_t sr = {0, 0, width, height};
  xe_copy_region_t dr = {0, 0, width, height};
  lzt::append_memory_copy_region(cmdlist, dst_buffer, &dr, width, src_buffer,
                                 &sr, width, hEvent, 0, nullptr);
  lzt::append_barrier(cmdlist, nullptr, 0, nullptr);
  lzt::append_wait_on_events(cmdlist, 1, &hEvent);
  lzt::close_command_list(cmdlist);
  lzt::execute_command_lists(cmdqueue, 1, &cmdlist, nullptr);

  // Verify Host Reads Event as set
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSynchronize(hEvent, UINT32_MAX));

  // Verify Memory Set completed
  // NOTE: Test currently failing due to potential L0 coherency bug. See
  // LOKI-549
  EXPECT_EQ(0, memcmp(src_buffer, dst_buffer, size));

  lzt::synchronize(cmdqueue, UINT32_MAX);
  lzt::free_memory(src_buffer);
  lzt::free_memory(dst_buffer);
}

static xe_image_handle_t create_test_image(int height, int width) {
  xe_image_desc_t image_description;
  image_description.format.layout = XE_IMAGE_FORMAT_LAYOUT_32;
  image_description.version = XE_IMAGE_DESC_VERSION_CURRENT;
  image_description.flags = XE_IMAGE_FLAG_PROGRAM_WRITE;
  image_description.type = XE_IMAGE_TYPE_2D;
  image_description.format.type = XE_IMAGE_FORMAT_TYPE_UINT;
  image_description.format.x = XE_IMAGE_FORMAT_SWIZZLE_R;
  image_description.format.y = XE_IMAGE_FORMAT_SWIZZLE_G;
  image_description.format.z = XE_IMAGE_FORMAT_SWIZZLE_B;
  image_description.format.w = XE_IMAGE_FORMAT_SWIZZLE_A;
  image_description.width = width;
  image_description.height = height;
  image_description.depth = 1;
  xe_image_handle_t image = nullptr;

  lzt::create_xe_image(image, &image_description);

  return image;
}

TEST_F(
    xeCommandListEventTests,
    GivenImageCopyThatSignalsEventWhenCompleteWhenExecutingCommandListThenHostAndGpuReadEventCorrectly) {

  // create 2 images
  lzt::ImagePNG32Bit input("test_input.png");
  int width = input.width();
  int height = input.height();
  lzt::ImagePNG32Bit output(width, height);
  auto input_xeimage = create_test_image(height, width);
  auto output_xeimage = create_test_image(height, width);
  xe_event_handle_t hEvent1, hEvent2, hEvent3, hEvent4;
  ep.create_event(hEvent1, XE_EVENT_SCOPE_FLAG_HOST, XE_EVENT_SCOPE_FLAG_NONE);
  ep.create_event(hEvent2, XE_EVENT_SCOPE_FLAG_HOST, XE_EVENT_SCOPE_FLAG_NONE);
  ep.create_event(hEvent3, XE_EVENT_SCOPE_FLAG_HOST, XE_EVENT_SCOPE_FLAG_NONE);
  ep.create_event(hEvent4, XE_EVENT_SCOPE_FLAG_HOST, XE_EVENT_SCOPE_FLAG_NONE);

  // Use ImageCopyFromMemory to upload ImageA
  lzt::append_image_copy_from_mem(cmdlist, input_xeimage, input.raw_data(),
                                  hEvent1);
  lzt::append_wait_on_events(cmdlist, 1, &hEvent1);
  // use ImageCopy to copy A -> B
  lzt::append_image_copy(cmdlist, output_xeimage, input_xeimage, hEvent2);
  lzt::append_wait_on_events(cmdlist, 1, &hEvent2);
  // use ImageCopyRegion to copy part of A -> B
  xe_image_region_t sr = {0, 0, 0, 1, 1, 1};
  xe_image_region_t dr = {0, 0, 0, 1, 1, 1};
  lzt::append_image_copy_region(cmdlist, output_xeimage, input_xeimage, &dr,
                                &sr, hEvent3);
  lzt::append_wait_on_events(cmdlist, 1, &hEvent3);
  // use ImageCopyToMemory to dowload ImageB
  lzt::append_image_copy_to_mem(cmdlist, output.raw_data(), output_xeimage,
                                hEvent4);
  lzt::append_wait_on_events(cmdlist, 1, &hEvent4);
  // execute commands
  lzt::close_command_list(cmdlist);
  lzt::execute_command_lists(cmdqueue, 1, &cmdlist, nullptr);

  // Make sure all events signaled from host perspective
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSynchronize(hEvent1, UINT32_MAX));
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSynchronize(hEvent2, UINT32_MAX));
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSynchronize(hEvent3, UINT32_MAX));
  EXPECT_EQ(XE_RESULT_SUCCESS, xeEventHostSynchronize(hEvent4, UINT32_MAX));

  // Verfy A matches B
  EXPECT_EQ(0,
            memcmp(input.raw_data(), output.raw_data(), input.size_in_bytes()));

  lzt::synchronize(cmdqueue, UINT32_MAX);
  // cleanup
  ep.destroy_event(hEvent1);
  ep.destroy_event(hEvent2);
  ep.destroy_event(hEvent3);
  ep.destroy_event(hEvent4);
  lzt::destroy_xe_image(input_xeimage);
  lzt::destroy_xe_image(output_xeimage);
}

// Other Test Variants:
// ImageCopy
// ImageCopyRegion
// ImageCopyToMemory
// ImageCopyFromMemory

} // namespace
