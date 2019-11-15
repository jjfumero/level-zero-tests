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
#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"
#include "logging/logging.hpp"

namespace lzt = level_zero_tests;

#include "ze_api.h"

using namespace level_zero_tests;

namespace {

class zeCommandListAppendMemorySetTests : public zeEventPoolCommandListTests {};

TEST_F(
    zeCommandListAppendMemorySetTests,
    GivenDeviceMemorySizeAndValueWhenAppendingMemorySetThenSuccessIsReturned) {
  const size_t size = 4096;
  void *memory = allocate_device_memory(size);
  const int value = 0x00;

  append_memory_set(cl.command_list_, memory, value, size);
  free_memory(memory);
}

TEST_F(
    zeCommandListAppendMemorySetTests,
    GivenDeviceMemorySizeAndValueWhenAppendingMemorySetWithHEventThenSuccessIsReturned) {
  const size_t size = 4096;
  void *memory = allocate_device_memory(size);
  const int value = 0x00;
  ze_event_handle_t hEvent = nullptr;

  ep.create_event(hEvent);
  lzt::append_memory_set(cl.command_list_, memory, value, size, hEvent);
  ep.destroy_event(hEvent);

  free_memory(memory);
}

class zeCommandListAppendMemorySetVerificationTests : public ::testing::Test {
protected:
  zeCommandListAppendMemorySetVerificationTests() {
    command_list = create_command_list();
    cq = create_command_queue();
  }

  ~zeCommandListAppendMemorySetVerificationTests() {
    destroy_command_queue(cq);
    destroy_command_list(command_list);
  }
  ze_command_list_handle_t command_list;
  ze_command_queue_handle_t cq;
};

TEST_F(zeCommandListAppendMemorySetVerificationTests,
       GivenHostMemoryWhenExecutingAMemorySetThenMemoryIsSetCorrectly) {

  size_t size = 16;
  auto memory = allocate_host_memory(size);
  uint8_t value = 0xAB;

  append_memory_set(command_list, memory, value, size);
  append_barrier(command_list, nullptr, 0, nullptr);
  close_command_list(command_list);
  execute_command_lists(cq, 1, &command_list, nullptr);
  synchronize(cq, UINT32_MAX);

  for (uint32_t i = 0; i < size; i++) {
    ASSERT_EQ(static_cast<uint8_t *>(memory)[i], value)
        << "Memory Set did not match.";
  }

  free_memory(memory);
}

TEST_F(zeCommandListAppendMemorySetVerificationTests,
       GivenSharedMemoryWhenExecutingAMemorySetThenMemoryIsSetCorrectly) {

  size_t size = 16;
  auto memory = allocate_shared_memory(size);
  uint8_t value = 0xAB;

  append_memory_set(command_list, memory, value, size);
  append_barrier(command_list, nullptr, 0, nullptr);
  close_command_list(command_list);
  execute_command_lists(cq, 1, &command_list, nullptr);
  synchronize(cq, UINT32_MAX);

  for (uint32_t i = 0; i < size; i++) {
    ASSERT_EQ(static_cast<uint8_t *>(memory)[i], value)
        << "Memory Set did not match.";
  }

  free_memory(memory);
}

TEST_F(zeCommandListAppendMemorySetVerificationTests,
       GivenDeviceMemoryWhenExecutingAMemorySetThenMemoryIsSetCorrectly) {

  size_t size = 16;
  auto memory = allocate_device_memory(size);
  auto local_mem = allocate_host_memory(size);
  uint8_t value = 0xAB;

  append_memory_set(command_list, memory, value, size);
  append_barrier(command_list, nullptr, 0, nullptr);
  close_command_list(command_list);
  execute_command_lists(cq, 1, &command_list, nullptr);
  synchronize(cq, UINT32_MAX);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListReset(command_list));
  append_memory_copy(command_list, local_mem, memory, size, nullptr);
  append_barrier(command_list, nullptr, 0, nullptr);
  close_command_list(command_list);
  execute_command_lists(cq, 1, &command_list, nullptr);
  synchronize(cq, UINT32_MAX);

  for (uint32_t i = 0; i < size; i++) {
    ASSERT_EQ(static_cast<uint8_t *>(local_mem)[i], value)
        << "Memory Set did not match.";
  }

  free_memory(memory);
}

class zeCommandListCommandQueueTests : public ::testing::Test {
protected:
  zeCommandList cl;
  zeCommandQueue cq;
};

class zeCommandListAppendMemoryCopyWithDataVerificationTests
    : public zeCommandListCommandQueueTests {};

TEST_F(
    zeCommandListAppendMemoryCopyWithDataVerificationTests,
    GivenHostMemoryDeviceMemoryAndSizeWhenAppendingMemoryCopyThenSuccessIsReturnedAndCopyIsCorrect) {
  const size_t size = 4 * 1024;
  std::vector<uint8_t> host_memory1(size), host_memory2(size, 0);
  void *device_memory = allocate_device_memory(size_in_bytes(host_memory1));

  lzt::write_data_pattern(host_memory1.data(), size, 1);

  append_memory_copy(cl.command_list_, device_memory, host_memory1.data(),
                     size_in_bytes(host_memory1), nullptr);
  append_barrier(cl.command_list_, nullptr, 0, nullptr);
  append_memory_copy(cl.command_list_, host_memory2.data(), device_memory,
                     size_in_bytes(host_memory2), nullptr);
  append_barrier(cl.command_list_, nullptr, 0, nullptr);
  close_command_list(cl.command_list_);
  execute_command_lists(cq.command_queue_, 1, &cl.command_list_, nullptr);
  synchronize(cq.command_queue_, UINT32_MAX);

  lzt::validate_data_pattern(host_memory2.data(), size, 1);
  free_memory(device_memory);
}

TEST_F(
    zeCommandListAppendMemoryCopyWithDataVerificationTests,
    GivenDeviceMemoryToDeviceMemoryAndSizeWhenAppendingMemoryCopyThenSuccessIsReturnedAndCopyIsCorrect) {
  const size_t size = 1024;
  // number of transfers to device memory
  const size_t num_dev_mem_copy = 8;
  // byte address alignment
  const size_t addr_alignment = 1;
  std::vector<uint8_t> host_memory1(size), host_memory2(size, 0);
  void *device_memory = allocate_device_memory(
      num_dev_mem_copy * (size_in_bytes(host_memory1) + addr_alignment));

  lzt::write_data_pattern(host_memory1.data(), size, 1);

  uint8_t *char_src_ptr = static_cast<uint8_t *>(device_memory);
  uint8_t *char_dst_ptr = char_src_ptr;
  append_memory_copy(cl.command_list_, static_cast<void *>(char_dst_ptr),
                     host_memory1.data(), size, nullptr);
  append_barrier(cl.command_list_, nullptr, 0, nullptr);
  // Must assert here to prevent Fulsim crash.  Ref: LOKI-473
  FAIL();
  for (uint32_t i = 1; i < num_dev_mem_copy; i++) {
    char_src_ptr = char_dst_ptr;
    char_dst_ptr += (size + addr_alignment);
    append_memory_copy(cl.command_list_, static_cast<void *>(char_dst_ptr),
                       static_cast<void *>(char_src_ptr), size, nullptr);
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
  }
  append_memory_copy(cl.command_list_, host_memory2.data(),
                     static_cast<void *>(char_dst_ptr), size, nullptr);
  append_barrier(cl.command_list_, nullptr, 0, nullptr);
  close_command_list(cl.command_list_);
  execute_command_lists(cq.command_queue_, 1, &cl.command_list_, nullptr);
  synchronize(cq.command_queue_, UINT32_MAX);

  lzt::validate_data_pattern(host_memory2.data(), size, 1);

  free_memory(device_memory);
}

class zeCommandListAppendMemoryCopyRegionWithDataVerificationTests
    : public zeCommandListCommandQueueTests {
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
                         host_source_1.data(), size, nullptr);
      append_barrier(cl.command_list_, nullptr, 0, nullptr);
      append_memory_copy(cl.command_list_, device_source_2,
                         host_source_2.data(), size, nullptr);
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
    ze_copy_region_t sr1;
    sr1.originX = 0;
    sr1.originY = 0;
    sr1.width = columns;
    sr1.height = 16;
    ze_copy_region_t dr1;
    dr1.originX = 0;
    dr1.originY = 0;
    dr1.width = columns;
    dr1.height = 16;
    append_memory_copy_region(cl.command_list_, the_destination, &dr1, columns,
                              the_source_buff_1, &sr1, columns, nullptr);
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // Copy the second 2K from the_source_buff_2 to the second 2K of
    // the_destination:
    ze_copy_region_t sr2;
    sr2.originX = 0;
    sr2.originY = 16;
    sr2.width = columns;
    sr2.height = 32;
    ze_copy_region_t dr2;
    dr2.originX = 0;
    dr2.originY = 16;
    dr2.width = columns;
    dr2.height = 32;
    append_memory_copy_region(cl.command_list_, the_destination, &dr2, columns,
                              the_source_buff_2, &sr2, columns, nullptr);
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // Copy the last   1K from the_source_buff_1 to the last   1K of
    // the_destination:
    ze_copy_region_t sr3;
    sr3.originX = 0;
    sr3.originY = 48;
    sr3.width = columns;
    sr3.height = 16;
    ze_copy_region_t dr3;
    dr3.originX = 0;
    dr3.originY = 48;
    dr3.width = columns;
    dr3.height = 16;
    append_memory_copy_region(cl.command_list_, the_destination, &dr3, columns,
                              the_source_buff_1, &sr3, columns, nullptr);
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // If the_destination is on the device, we need to copy it back to the host
    // to verify the data pattern:
    if (destOnDevice) {
      append_memory_copy(cl.command_list_, host_verification_buff.data(),
                         the_destination, size, nullptr);
      append_barrier(cl.command_list_, nullptr, 0, nullptr);
    }

    // Close the command list:
    close_command_list(cl.command_list_);
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
    zeCommandListAppendMemoryCopyRegionWithDataVerificationTests,
    GivenHostMemoryDeviceMemoryWhenAppendingMemoryCopyRegionFromHostToHostThenSuccessIsReturnedAndCopyIsCorrect) {
  test_copy_region(false, false);
}

TEST_F(
    zeCommandListAppendMemoryCopyRegionWithDataVerificationTests,
    GivenHostMemoryDeviceMemoryWhenAppendingMemoryCopyRegionFromHostToDeviceThenSuccessIsReturnedAndCopyIsCorrect) {
  test_copy_region(false, true);
}

TEST_F(
    zeCommandListAppendMemoryCopyRegionWithDataVerificationTests,
    GivenHostMemoryDeviceMemoryWhenAppendingMemoryCopyRegionFromDeviceToHostThenSuccessIsReturnedAndCopyIsCorrect) {
  test_copy_region(true, false);
}

TEST_F(
    zeCommandListAppendMemoryCopyRegionWithDataVerificationTests,
    GivenHostMemoryDeviceMemoryWhenAppendingMemoryCopyRegionFromDeviceToDeviceThenSuccessIsReturnedAndCopyIsCorrect) {
  test_copy_region(true, true);
}

class zeCommandListAppendMemoryCopyTests : public zeEventPoolCommandListTests {
};

TEST_F(
    zeCommandListAppendMemoryCopyTests,
    GivenHostMemoryDeviceHostMemoryAndSizeWhenAppendingMemoryCopyThenSuccessIsReturned) {
  const size_t size = 16;
  const std::vector<char> host_memory(size, 123);
  void *memory = allocate_device_memory(size_in_bytes(host_memory));

  append_memory_copy(cl.command_list_, memory, host_memory.data(),
                     size_in_bytes(host_memory), nullptr);

  free_memory(memory);
}

TEST_F(
    zeCommandListAppendMemoryCopyTests,
    GivenHostMemoryDeviceHostMemoryAndSizeWhenAppendingMemoryCopyWithHEventThenSuccessIsReturned) {
  const size_t size = 16;
  const std::vector<char> host_memory(size, 123);
  void *memory = allocate_device_memory(size_in_bytes(host_memory));
  ze_event_handle_t hEvent = nullptr;

  ep.create_event(hEvent);
  append_memory_copy(cl.command_list_, memory, host_memory.data(),
                     size_in_bytes(host_memory), hEvent);

  ep.destroy_event(hEvent);

  free_memory(memory);
}

class zeCommandListAppendMemoryPrefetchTests : public zeCommandListTests {};

TEST_F(
    zeCommandListAppendMemoryPrefetchTests,
    GivenDeviceMemoryAndSizeWhenAppendingMemoryPrefetchThenSuccessIsReturned) {
  const size_t size = 16;
  void *memory = allocate_device_memory(size);

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendMemoryPrefetch(cl.command_list_, memory, size));

  free_memory(memory);
}

class zeCommandListAppendMemAdviseTests
    : public zeCommandListAppendMemoryPrefetchTests {};

TEST_F(zeCommandListAppendMemAdviseTests,
       GivenDeviceMemoryAndSizeWhenAppendingMemAdviseThenSuccessIsReturned) {
  const size_t size = 16;
  void *memory = allocate_device_memory(size);

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendMemAdvise(
                cl.command_list_, zeDevice::get_instance()->get_device(),
                memory, size, ZE_MEMORY_ADVICE_SET_READ_MOSTLY));

  free_memory(memory);
}

class zeCommandListAppendImageCopyFromMemoryTests : public ::testing::Test {
protected:
  zeEventPool ep;
  zeCommandList cl;
  zeImageCreateCommon img;
};

TEST_F(
    zeCommandListAppendImageCopyFromMemoryTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyThenSuccessIsReturned) {

  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendImageCopyFromMemory(
                cl.command_list_, img.dflt_device_image_,
                img.dflt_host_image_.raw_data(), nullptr, nullptr));
}

TEST_F(
    zeCommandListAppendImageCopyFromMemoryTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyWithHEventThenSuccessIsReturned) {
  ze_event_handle_t hEvent = nullptr;

  ep.create_event(hEvent);
  EXPECT_EQ(ZE_RESULT_SUCCESS,
            zeCommandListAppendImageCopyFromMemory(
                cl.command_list_, img.dflt_device_image_,
                img.dflt_host_image_.raw_data(), nullptr, hEvent));
  ep.destroy_event(hEvent);
}

class zeCommandListAppendImageCopyRegionTests
    : public zeCommandListAppendImageCopyFromMemoryTests {};

TEST_F(
    zeCommandListAppendImageCopyRegionTests,
    GivenDeviceImageAndDeviceImageWhenAppendingImageCopyRegionThenSuccessIsReturned) {
  ze_image_region_t source_region;
  ze_image_region_t dest_region;

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
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyRegion(
                                   cl.command_list_, img.dflt_device_image_,
                                   img.dflt_device_image_2_, &dest_region,
                                   &source_region, nullptr));
}

TEST_F(
    zeCommandListAppendImageCopyRegionTests,
    GivenDeviceImageAndDeviceImageWhenAppendingImageCopyRegionWithHEventThenSuccessIsReturned) {
  ze_image_region_t source_region;
  ze_image_region_t dest_region;

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
  ze_event_handle_t hEvent = nullptr;

  ep.create_event(hEvent);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyRegion(
                                   cl.command_list_, img.dflt_device_image_,
                                   img.dflt_device_image_2_, &dest_region,
                                   &source_region, hEvent));
  ep.destroy_event(hEvent);
}

class zeCommandListAppendImageCopyToMemoryTests
    : public zeCommandListAppendImageCopyFromMemoryTests {};

TEST_F(zeCommandListAppendImageCopyToMemoryTests,
       GivenDeviceImageWhenAppendingImageCopyToMemoryThenSuccessIsReturned) {
  void *device_memory =
      allocate_device_memory(size_in_bytes(img.dflt_host_image_));

  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyToMemory(
                                   cl.command_list_, device_memory,
                                   img.dflt_device_image_, nullptr, nullptr));
  free_memory(device_memory);
}

TEST_F(
    zeCommandListAppendImageCopyToMemoryTests,
    GivenDeviceImageWhenAppendingImageCopyToMemoryWithHEventThenSuccessIsReturned) {
  void *device_memory =
      allocate_device_memory(size_in_bytes(img.dflt_host_image_));
  ze_event_handle_t hEvent = nullptr;

  ep.create_event(hEvent);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyToMemory(
                                   cl.command_list_, device_memory,
                                   img.dflt_device_image_, nullptr, hEvent));
  ep.destroy_event(hEvent);
  free_memory(device_memory);
}

class zeCommandListAppendImageCopyTests
    : public zeCommandListAppendImageCopyFromMemoryTests {};

TEST_F(zeCommandListAppendImageCopyTests,
       GivenDeviceImageWhenAppendingImageCopyThenSuccessIsReturned) {
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopy(
                                   cl.command_list_, img.dflt_device_image_,
                                   img.dflt_device_image_2_, nullptr));
}

TEST_F(zeCommandListAppendImageCopyTests,
       GivenDeviceImageWhenAppendingImageCopyWithHEventThenSuccessIsReturned) {
  ze_event_handle_t hEvent = nullptr;

  ep.create_event(hEvent);
  EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopy(
                                   cl.command_list_, img.dflt_device_image_,
                                   img.dflt_device_image_2_, hEvent));
  ep.destroy_event(hEvent);
}

} // namespace
