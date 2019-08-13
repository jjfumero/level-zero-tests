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

namespace cs = compute_samples;

#include "xe_copy.h"
#include "xe_driver.h"
#include "xe_memory.h"
#include "xe_event.h"
#include "xe_barrier.h"

using namespace compute_samples;

namespace {

class xeCommandListAppendMemorySetTests : public xeEventPoolCommandListTests {};

TEST_F(
    xeCommandListAppendMemorySetTests,
    GivenDeviceMemorySizeAndValueWhenAppendingMemorySetThenSuccessIsReturned) {
  const size_t size = 4096;
  void *memory = allocate_device_memory(size);
  const int value = 0x00;

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemorySet(cl.command_list_, memory, value, size,
                                         nullptr, 0, nullptr));
  free_memory(memory);
}

TEST_F(
    xeCommandListAppendMemorySetTests,
    GivenDeviceMemorySizeAndValueWhenAppendingMemorySetWithHEventThenSuccessIsReturned) {
  const size_t size = 4096;
  void *memory = allocate_device_memory(size);
  const int value = 0x00;
  xe_event_handle_t hEvent = nullptr;

  ep.create_event(hEvent);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemorySet(cl.command_list_, memory, value, size,
                                         hEvent, 0, nullptr));
  ep.destroy_event(hEvent);

  free_memory(memory);
}

TEST_F(
    xeCommandListAppendMemorySetTests,
    GivenDeviceMemorySizeAndValueWhenAppendingMemorySetWithEventsThenSuccessIsReturned) {
  const size_t size = 4096;
  void *memory = allocate_device_memory(size);
  const int value = 0x00;
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> hEvents(event_count, nullptr);

  ep.create_events(hEvents, event_count);

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemorySet(cl.command_list_, memory, value, size,
                                         nullptr, event_count, hEvents.data()));
  ep.destroy_events(hEvents);

  free_memory(memory);
}

TEST_F(
    xeCommandListAppendMemorySetTests,
    GivenDeviceMemorySizeAndValueWhenAppendingMemorySetWithHEventAndEventsThenSuccessIsReturned) {
  const size_t size = 4096;
  void *memory = allocate_device_memory(size);
  const int value = 0x00;
  xe_event_handle_t hEvent = nullptr;
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> hEvents(event_count, nullptr);

  ep.create_event(hEvent);
  ep.create_events(hEvents, event_count);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendMemorySet(
                                   cl.command_list_, memory, value, size,
                                   hEvent, hEvents.size(), hEvents.data()));
  ep.destroy_event(hEvent);
  ep.destroy_events(hEvents);

  free_memory(memory);
}

class xeCommandListCommandQueueTests : public ::testing::Test {
protected:
  xeCommandList cl;
  xeCommandQueue cq;
};

class xeCommandListAppendMemoryCopyWithDataVerificationTests
    : public xeCommandListCommandQueueTests {};

TEST_F(
    xeCommandListAppendMemoryCopyWithDataVerificationTests,
    GivenHostMemoryDeviceMemoryAndSizeWhenAppendingMemoryCopyThenSuccessIsReturnedAndCopyIsCorrect) {
  const size_t size = 4 * 1024;
  std::vector<uint8_t> host_memory1(size), host_memory2(size, 0);
  void *device_memory = allocate_device_memory(size_in_bytes(host_memory1));
  uint8_t ui8_data_pattern;

  ui8_data_pattern = 0;
  for (auto &ui : host_memory1) {
    ui = ui8_data_pattern;
    ui8_data_pattern = (ui8_data_pattern + 1) & 0xff;
  }

  append_memory_copy(cl.command_list_, device_memory, host_memory1.data(),
                     size_in_bytes(host_memory1), nullptr, 0, nullptr);
  append_barrier(cl.command_list_, nullptr, 0, nullptr);
  append_memory_copy(cl.command_list_, host_memory2.data(), device_memory,
                     size_in_bytes(host_memory2), nullptr, 0, nullptr);
  append_barrier(cl.command_list_, nullptr, 0, nullptr);
  close(cl.command_list_);
  execute_command_lists(cq.command_queue_, 1, &cl.command_list_, nullptr);
  synchronize(cq.command_queue_, UINT32_MAX);

  ui8_data_pattern = 0;
  for (auto &ui : host_memory2) {
    EXPECT_EQ(ui, ui8_data_pattern);
    ui8_data_pattern = (ui8_data_pattern + 1) & 0xff;
  }

  free_memory(device_memory);
}

class xeCommandListAppendMemoryCopyRegionWithDataVerificationTests
    : public xeCommandListCommandQueueTests {
protected:
  void test_copy_region(bool sourceOnDevice, bool destOnDevice) {
    const size_t rows = 64;
    const size_t columns = 64;
    const size_t size = rows * columns;
    std::vector<uint8_t> host_source_1(size), host_source_2(size);

    // Write data patterns to the two host source buffers:
    {
      uint8_t ui8_data_pattern = 0;
      for (auto &ui : host_source_1) {
        ui = ui8_data_pattern;
        ui8_data_pattern = (ui8_data_pattern + 1) & 0xff;
      }
    }
    {
      uint8_t ui8_data_pattern = 0;
      for (auto &ui : host_source_2) {
        ui = ui8_data_pattern;
        ui8_data_pattern = (ui8_data_pattern - 1) & 0xff;
      }
    }
    // the_source_buff_1 and the_source_buff_2 represents the two source
    // buffers for the copy region commands, be they located on the host
    // or on the device.
    // Optionally mirror the data patterns onto two device buffers:
    void *the_source_buff_1 = nullptr, *the_source_buff_2 = nullptr;
    void *device_source_1 = nullptr, *device_source_2 = nullptr;
    if (sourceOnDevice) {
      device_source_1 = allocate_device_memory(size);
      device_source_2 = allocate_device_memory(size);
      append_memory_copy(cl.command_list_, device_source_1,
                         host_source_1.data(), size, nullptr, 0, nullptr);
      append_barrier(cl.command_list_, nullptr, 0, nullptr);
      append_memory_copy(cl.command_list_, device_source_2,
                         host_source_2.data(), size, nullptr, 0, nullptr);
      append_barrier(cl.command_list_, nullptr, 0, nullptr);
      the_source_buff_1 = device_source_1;
      the_source_buff_2 = device_source_2;
    } else {
      the_source_buff_1 = host_source_1.data();
      the_source_buff_2 = host_source_2.data();
    }
    // the_destination represents the destination for the copy region command.
    void *the_destination = nullptr;
    std::vector<uint8_t> host_verification_buff(size);
    void *device_dest = nullptr;
    // Optionally, allocate the destination buffer on the device
    if (destOnDevice) {
      device_dest = allocate_device_memory(size);
      the_destination = device_dest;
    } else {
      the_destination = host_verification_buff.data();
    }
    // Everything is set up for the copy region commands now:
    // the source of the copy region commands is stored in the two variables
    // the_source_buff_1 and the_source_buff_2 the destination is stored in
    // the_destination.

    // Copy the first  1K from the_source_buff_1 to the first 1K
    // of the_destination:
    xe_copy_region_t sr1;
    sr1.originX = 0;
    sr1.originY = 0;
    sr1.width = columns;
    sr1.height = 16;
    xe_copy_region_t dr1;
    dr1.originX = 0;
    dr1.originY = 0;
    dr1.width = columns;
    dr1.height = 16;
    append_memory_copy_region(cl.command_list_, the_destination, &dr1, columns,
                              the_source_buff_1, &sr1, columns, nullptr, 0,
                              nullptr);
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // Copy the second 2K from the_source_buff_2 to the second 2K of
    // the_destination:
    xe_copy_region_t sr2;
    sr2.originX = 0;
    sr2.originY = 16;
    sr2.width = columns;
    sr2.height = 32;
    xe_copy_region_t dr2;
    dr2.originX = 0;
    dr2.originY = 16;
    dr2.width = columns;
    dr2.height = 32;
    append_memory_copy_region(cl.command_list_, the_destination, &dr2, columns,
                              the_source_buff_2, &sr2, columns, nullptr, 0,
                              nullptr);
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // Copy the last   1K from the_source_buff_1 to the last   1K of
    // the_destination:
    xe_copy_region_t sr3;
    sr3.originX = 0;
    sr3.originY = 48;
    sr3.width = columns;
    sr3.height = 16;
    xe_copy_region_t dr3;
    dr3.originX = 0;
    dr3.originY = 48;
    dr3.width = columns;
    dr3.height = 16;
    append_memory_copy_region(cl.command_list_, the_destination, &dr3, columns,
                              the_source_buff_1, &sr3, columns, nullptr, 0,
                              nullptr);
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // If the_destination is on the device, we need to copy it back to the host
    // to verify the data pattern:
    if (destOnDevice) {
      append_memory_copy(cl.command_list_, host_verification_buff.data(),
                         the_destination, size, nullptr, 0, nullptr);
      append_barrier(cl.command_list_, nullptr, 0, nullptr);
    }

    // Execute all of the copies:
    execute_command_lists(cq.command_queue_, 1, &cl.command_list_, nullptr);
    synchronize(cq.command_queue_, UINT32_MAX);

    // Verify the contents of host_verification_buff:
    size_t errCnt = 0;
    for (unsigned int ui = 0; ui < size; ui++) {
      uint8_t cv = 0;
      if (ui >= 1024 && ui < (3 * 1024))
        cv = host_source_2[ui];
      else
        cv = host_source_1[ui];
      if (cv != host_verification_buff[ui]) {
        printf("Expected: %d in host_verification_buff[%d], but got: %d\n", cv,
               ui, host_verification_buff[ui]);
        errCnt++;
      }
    }
    EXPECT_EQ(0, errCnt);

    // Free all device memory used:
    if (device_source_1)
      free_memory(device_source_1);
    if (device_source_2)
      free_memory(device_source_2);
    if (device_dest)
      free_memory(device_dest);
  }
};

TEST_F(
    xeCommandListAppendMemoryCopyRegionWithDataVerificationTests,
    GivenHostMemoryDeviceMemoryWhenAppendingMemoryCopyRegionFromHostToHostThenSuccessIsReturnedAndCopyIsCorrect) {
  test_copy_region(false, false);
}

TEST_F(
    xeCommandListAppendMemoryCopyRegionWithDataVerificationTests,
    GivenHostMemoryDeviceMemoryWhenAppendingMemoryCopyRegionFromHostToDeviceThenSuccessIsReturnedAndCopyIsCorrect) {
  test_copy_region(false, true);
}

TEST_F(
    xeCommandListAppendMemoryCopyRegionWithDataVerificationTests,
    GivenHostMemoryDeviceMemoryWhenAppendingMemoryCopyRegionFromDeviceToHostThenSuccessIsReturnedAndCopyIsCorrect) {
  test_copy_region(true, false);
}

TEST_F(
    xeCommandListAppendMemoryCopyRegionWithDataVerificationTests,
    GivenHostMemoryDeviceMemoryWhenAppendingMemoryCopyRegionFromDeviceToDeviceThenSuccessIsReturnedAndCopyIsCorrect) {
  test_copy_region(true, true);
}

class xeCommandListAppendMemoryCopyTests : public xeEventPoolCommandListTests {
};

TEST_F(
    xeCommandListAppendMemoryCopyTests,
    GivenHostMemoryDeviceHostMemoryAndSizeWhenAppendingMemoryCopyThenSuccessIsReturned) {
  const size_t size = 16;
  const std::vector<char> host_memory(size, 123);
  void *memory = allocate_device_memory(size_in_bytes(host_memory));

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemoryCopy(
                cl.command_list_, memory, host_memory.data(),
                size_in_bytes(host_memory), nullptr, 0, nullptr));

  free_memory(memory);
}

TEST_F(
    xeCommandListAppendMemoryCopyTests,
    GivenHostMemoryDeviceHostMemoryAndSizeWhenAppendingMemoryCopyWithHEventThenSuccessIsReturned) {
  const size_t size = 16;
  const std::vector<char> host_memory(size, 123);
  void *memory = allocate_device_memory(size_in_bytes(host_memory));
  xe_event_handle_t hEvent = nullptr;

  ep.create_event(hEvent);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemoryCopy(
                cl.command_list_, memory, host_memory.data(),
                size_in_bytes(host_memory), hEvent, 0, nullptr));

  ep.destroy_event(hEvent);

  free_memory(memory);
}

TEST_F(
    xeCommandListAppendMemoryCopyTests,
    GivenHostMemoryDeviceHostMemoryAndSizeWhenAppendingMemoryCopyWithEventsThenSuccessIsReturned) {
  const size_t size = 16;
  const std::vector<char> host_memory(size, 123);
  void *memory = allocate_device_memory(size_in_bytes(host_memory));
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> hEvents(event_count, nullptr);

  ep.create_events(hEvents, event_count);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendMemoryCopy(
                                   cl.command_list_, memory, host_memory.data(),
                                   size_in_bytes(host_memory), nullptr,
                                   event_count, hEvents.data()));
  ep.destroy_events(hEvents);
  free_memory(memory);
}

TEST_F(
    xeCommandListAppendMemoryCopyTests,
    GivenHostMemoryDeviceHostMemoryAndSizeWhenAppendingMemoryCopyWithHEventAndEventsThenSuccessIsReturned) {
  const size_t size = 16;
  const std::vector<char> host_memory(size, 123);
  void *memory = allocate_device_memory(size_in_bytes(host_memory));
  xe_event_handle_t hEvent = nullptr;
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> hEvents(event_count, nullptr);

  ep.create_event(hEvent);
  ep.create_events(hEvents, event_count);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendMemoryCopy(
                                   cl.command_list_, memory, host_memory.data(),
                                   size_in_bytes(host_memory), hEvent,
                                   hEvents.size(), hEvents.data()));
  ep.destroy_event(hEvent);
  ep.destroy_events(hEvents);

  free_memory(memory);
}

class xeCommandListAppendMemoryPrefetchTests : public xeCommandListTests {};

TEST_F(
    xeCommandListAppendMemoryPrefetchTests,
    GivenDeviceMemoryAndSizeWhenAppendingMemoryPrefetchThenSuccessIsReturned) {
  const size_t size = 16;
  void *memory = allocate_device_memory(size);

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemoryPrefetch(cl.command_list_, memory, size));

  free_memory(memory);
}

class xeCommandListAppendMemAdviseTests
    : public xeCommandListAppendMemoryPrefetchTests {};

TEST_F(xeCommandListAppendMemAdviseTests,
       GivenDeviceMemoryAndSizeWhenAppendingMemAdviseThenSuccessIsReturned) {
  const size_t size = 16;
  void *memory = allocate_device_memory(size);

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendMemAdvise(
                cl.command_list_, xeDevice::get_instance()->get_device(),
                memory, size, XE_MEMORY_ADVICE_SET_READ_MOSTLY));

  free_memory(memory);
}

class xeCommandListAppendImageCopyFromMemoryTests : public ::testing::Test {
protected:
  xeEventPool ep;
  xeCommandList cl;
  xeImageCreateCommon img;
};

TEST_F(
    xeCommandListAppendImageCopyFromMemoryTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyThenSuccessIsReturned) {

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendImageCopyFromMemory(
                cl.command_list_, img.dflt_device_image_,
                img.dflt_host_image_.raw_data(), nullptr, nullptr, 0, nullptr));
}

TEST_F(
    xeCommandListAppendImageCopyFromMemoryTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyWithHEventThenSuccessIsReturned) {
  xe_event_handle_t hEvent = nullptr;

  ep.create_event(hEvent);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendImageCopyFromMemory(
                cl.command_list_, img.dflt_device_image_,
                img.dflt_host_image_.raw_data(), nullptr, hEvent, 0, nullptr));
  ep.destroy_event(hEvent);
}

TEST_F(
    xeCommandListAppendImageCopyFromMemoryTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyWithEventsThenSuccessIsReturned) {
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> hEvents(event_count, nullptr);

  ep.create_events(hEvents, event_count);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendImageCopyFromMemory(
                                   cl.command_list_, img.dflt_device_image_,
                                   img.dflt_host_image_.raw_data(), nullptr,
                                   nullptr, event_count, hEvents.data()));
  ep.destroy_events(hEvents);
}

TEST_F(
    xeCommandListAppendImageCopyFromMemoryTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyWithHEventAndEventsThenSuccessIsReturned) {
  xe_event_handle_t hEvent = nullptr;
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> hEvents(event_count, nullptr);

  ep.create_event(hEvent);
  ep.create_events(hEvents, event_count);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendImageCopyFromMemory(
                                   cl.command_list_, img.dflt_device_image_,
                                   img.dflt_host_image_.raw_data(), nullptr,
                                   hEvent, hEvents.size(), hEvents.data()));
  ep.destroy_event(hEvent);
  ep.destroy_events(hEvents);
}

class xeCommandListAppendImageCopyRegionTests
    : public xeCommandListAppendImageCopyFromMemoryTests {};

TEST_F(
    xeCommandListAppendImageCopyRegionTests,
    GivenDeviceImageAndDeviceImageWhenAppendingImageCopyRegionThenSuccessIsReturned) {
  xe_image_region_t source_region;
  xe_image_region_t dest_region;

  dest_region.originX = 0;
  dest_region.originY = 0;
  dest_region.originZ = 0;
  dest_region.width = img.dflt_host_image_.width() / 2;
  dest_region.height = img.dflt_host_image_.height() / 2;
  dest_region.depth = 1;

  source_region.originX = img.dflt_host_image_.width() / 2;
  source_region.originY = img.dflt_host_image_.height() / 2;
  source_region.originZ = 0;
  source_region.width = img.dflt_host_image_.width() / 2;
  source_region.height = img.dflt_host_image_.height() / 2;
  source_region.depth = 1;
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendImageCopyRegion(
                                   cl.command_list_, img.dflt_device_image_,
                                   img.dflt_device_image_2_, &dest_region,
                                   &source_region, nullptr, 0, nullptr));
}

TEST_F(
    xeCommandListAppendImageCopyRegionTests,
    GivenDeviceImageAndDeviceImageWhenAppendingImageCopyRegionWithHEventThenSuccessIsReturned) {
  xe_image_region_t source_region;
  xe_image_region_t dest_region;

  dest_region.originX = 0;
  dest_region.originY = 0;
  dest_region.originZ = 0;
  dest_region.width = img.dflt_host_image_.width() / 2;
  dest_region.height = img.dflt_host_image_.height() / 2;
  dest_region.depth = 1;

  source_region.originX = img.dflt_host_image_.width() / 2;
  source_region.originY = img.dflt_host_image_.height() / 2;
  source_region.originZ = 0;
  source_region.width = img.dflt_host_image_.width() / 2;
  source_region.height = img.dflt_host_image_.height() / 2;
  source_region.depth = 1;
  xe_event_handle_t hEvent = nullptr;

  ep.create_event(hEvent);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendImageCopyRegion(
                                   cl.command_list_, img.dflt_device_image_,
                                   img.dflt_device_image_2_, &dest_region,
                                   &source_region, hEvent, 0, nullptr));
  ep.destroy_event(hEvent);
}

TEST_F(
    xeCommandListAppendImageCopyRegionTests,
    GivenDeviceImageAndDeviceImageWhenAppendingImageCopyRegionWithEventsThenSuccessIsReturned) {
  xe_image_region_t source_region;
  xe_image_region_t dest_region;

  dest_region.originX = 0;
  dest_region.originY = 0;
  dest_region.originZ = 0;
  dest_region.width = img.dflt_host_image_.width() / 2;
  dest_region.height = img.dflt_host_image_.height() / 2;
  dest_region.depth = 1;

  source_region.originX = img.dflt_host_image_.width() / 2;
  source_region.originY = img.dflt_host_image_.height() / 2;
  source_region.originZ = 0;
  source_region.width = img.dflt_host_image_.width() / 2;
  source_region.height = img.dflt_host_image_.height() / 2;
  source_region.depth = 1;
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> hEvents(event_count, nullptr);

  ep.create_events(hEvents, event_count);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendImageCopyRegion(
                cl.command_list_, img.dflt_device_image_,
                img.dflt_device_image_2_, &dest_region, &source_region, nullptr,
                event_count, hEvents.data()));
  ep.destroy_events(hEvents);
}

TEST_F(
    xeCommandListAppendImageCopyRegionTests,
    GivenDeviceImageAndDeviceImageWhenAppendingImageCopyRegionWithHEventAndEventsThenSuccessIsReturned) {
  xe_image_region_t source_region;
  xe_image_region_t dest_region;

  dest_region.originX = 0;
  dest_region.originY = 0;
  dest_region.originZ = 0;
  dest_region.width = img.dflt_host_image_.width() / 2;
  dest_region.height = img.dflt_host_image_.height() / 2;
  dest_region.depth = 1;

  source_region.originX = img.dflt_host_image_.width() / 2;
  source_region.originY = img.dflt_host_image_.height() / 2;
  source_region.originZ = 0;
  source_region.width = img.dflt_host_image_.width() / 2;
  source_region.height = img.dflt_host_image_.height() / 2;
  source_region.depth = 1;
  xe_event_handle_t hEvent = nullptr;
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> hEvents(event_count, nullptr);

  ep.create_event(hEvent);
  ep.create_events(hEvents, event_count);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendImageCopyRegion(
                cl.command_list_, img.dflt_device_image_,
                img.dflt_device_image_2_, &dest_region, &source_region, hEvent,
                hEvents.size(), hEvents.data()));
  ep.destroy_event(hEvent);
  ep.destroy_events(hEvents);
}

class xeCommandListAppendImageCopyToMemoryTests
    : public xeCommandListAppendImageCopyFromMemoryTests {};

TEST_F(xeCommandListAppendImageCopyToMemoryTests,
       GivenDeviceImageWhenAppendingImageCopyToMemoryThenSuccessIsReturned) {
  void *device_memory =
      allocate_device_memory(size_in_bytes(img.dflt_host_image_));

  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendImageCopyToMemory(
                cl.command_list_, device_memory, img.dflt_device_image_,
                nullptr, nullptr, 0, nullptr));
  free_memory(device_memory);
}

TEST_F(
    xeCommandListAppendImageCopyToMemoryTests,
    GivenDeviceImageWhenAppendingImageCopyToMemoryWithHEventThenSuccessIsReturned) {
  void *device_memory =
      allocate_device_memory(size_in_bytes(img.dflt_host_image_));
  xe_event_handle_t hEvent = nullptr;

  ep.create_event(hEvent);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendImageCopyToMemory(
                cl.command_list_, device_memory, img.dflt_device_image_,
                nullptr, hEvent, 0, nullptr));
  ep.destroy_event(hEvent);
  free_memory(device_memory);
}

TEST_F(
    xeCommandListAppendImageCopyToMemoryTests,
    GivenDeviceImageWhenAppendingImageCopyToMemoryWithEventsThenSuccessIsReturned) {
  void *device_memory =
      allocate_device_memory(size_in_bytes(img.dflt_host_image_));
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> hEvents(event_count, nullptr);

  ep.create_events(hEvents, event_count);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendImageCopyToMemory(
                cl.command_list_, device_memory, img.dflt_device_image_,
                nullptr, nullptr, event_count, hEvents.data()));
  ep.destroy_events(hEvents);
  free_memory(device_memory);
}

TEST_F(
    xeCommandListAppendImageCopyToMemoryTests,
    GivenDeviceImageWhenAppendingImageCopyToMemoryWithHEventAndEventsThenSuccessIsReturned) {
  void *device_memory =
      allocate_device_memory(size_in_bytes(img.dflt_host_image_));
  xe_event_handle_t hEvent = nullptr;
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> hEvents(event_count, nullptr);

  ep.create_event(hEvent);
  ep.create_events(hEvents, event_count);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendImageCopyToMemory(
                cl.command_list_, device_memory, img.dflt_device_image_,
                nullptr, hEvent, hEvents.size(), hEvents.data()));
  ep.destroy_event(hEvent);
  ep.destroy_events(hEvents);
  free_memory(device_memory);
}

class xeCommandListAppendImageCopyTests
    : public xeCommandListAppendImageCopyFromMemoryTests {};

TEST_F(xeCommandListAppendImageCopyTests,
       GivenDeviceImageWhenAppendingImageCopyThenSuccessIsReturned) {
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendImageCopy(
                cl.command_list_, img.dflt_device_image_,
                img.dflt_device_image_2_, nullptr, 0, nullptr));
}

TEST_F(xeCommandListAppendImageCopyTests,
       GivenDeviceImageWhenAppendingImageCopyWithHEventThenSuccessIsReturned) {
  xe_event_handle_t hEvent = nullptr;

  ep.create_event(hEvent);
  EXPECT_EQ(XE_RESULT_SUCCESS,
            xeCommandListAppendImageCopy(
                cl.command_list_, img.dflt_device_image_,
                img.dflt_device_image_2_, hEvent, 0, nullptr));
  ep.destroy_event(hEvent);
}

TEST_F(xeCommandListAppendImageCopyTests,
       GivenDeviceImageWhenAppendingImageCopyWithEventsThenSuccessIsReturned) {
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> hEvents(event_count, nullptr);

  ep.create_events(hEvents, event_count);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendImageCopy(
                                   cl.command_list_, img.dflt_device_image_,
                                   img.dflt_device_image_2_, nullptr,
                                   event_count, hEvents.data()));
  ep.destroy_events(hEvents);
}

TEST_F(
    xeCommandListAppendImageCopyTests,
    GivenDeviceImageWhenAppendingImageCopyWithHEventAndEventsThenSuccessIsReturned) {
  xe_event_handle_t hEvent = nullptr;
  const size_t event_count = 2;
  std::vector<xe_event_handle_t> hEvents(event_count, nullptr);

  ep.create_event(hEvent);
  ep.create_events(hEvents, event_count);
  EXPECT_EQ(XE_RESULT_SUCCESS, xeCommandListAppendImageCopy(
                                   cl.command_list_, img.dflt_device_image_,
                                   img.dflt_device_image_2_, hEvent,
                                   hEvents.size(), hEvents.data()));
  ep.destroy_event(hEvent);
  ep.destroy_events(hEvents);
}

} // namespace

// TODO: Test memory copy
// TODO: Test memory set
// TODO: Test image copy
// TODO: Test image region copy
// TODO: Test image from/to memory copy
// TODO: Test memory prefetch
// TODO: Test memory advise
// TODO: Test copies on a device using OpenCL C kernels
