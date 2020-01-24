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

class zeCommandListAppendMemoryFillTests : public zeEventPoolCommandListTests {
};

TEST_F(
    zeCommandListAppendMemoryFillTests,
    GivenDeviceMemorySizeAndValueWhenAppendingMemoryFillThenSuccessIsReturned) {
  const size_t size = 4096;
  void *memory = allocate_device_memory(size);
  const uint8_t pattern = 0x00;
  const int pattern_size = 1;

  lzt::append_memory_fill(cl.command_list_, memory, &pattern, pattern_size,
                          size, nullptr);
  free_memory(memory);
}

TEST_F(
    zeCommandListAppendMemoryFillTests,
    GivenDeviceMemorySizeAndValueWhenAppendingMemoryFillWithHEventThenSuccessIsReturned) {
  const size_t size = 4096;
  void *memory = allocate_device_memory(size);
  const uint8_t pattern = 0x00;
  ze_event_handle_t hEvent = nullptr;
  const int pattern_size = 1;

  ep.create_event(hEvent);
  lzt::append_memory_fill(cl.command_list_, memory, &pattern, pattern_size,
                          size, hEvent);
  ep.destroy_event(hEvent);

  free_memory(memory);
}

class zeCommandListAppendMemoryFillVerificationTests : public ::testing::Test {
protected:
  zeCommandListAppendMemoryFillVerificationTests() {
    command_list = create_command_list();
    cq = create_command_queue();
  }

  ~zeCommandListAppendMemoryFillVerificationTests() {
    destroy_command_queue(cq);
    destroy_command_list(command_list);
  }
  ze_command_list_handle_t command_list;
  ze_command_queue_handle_t cq;
};

TEST_F(zeCommandListAppendMemoryFillVerificationTests,
       GivenHostMemoryWhenExecutingAMemoryFillThenMemoryIsSetCorrectly) {

  size_t size = 16;
  auto memory = allocate_host_memory(size);
  uint8_t pattern = 0xAB;
  const int pattern_size = 1;

  append_memory_fill(command_list, memory, &pattern, pattern_size, size,
                     nullptr);
  append_barrier(command_list, nullptr, 0, nullptr);
  close_command_list(command_list);
  execute_command_lists(cq, 1, &command_list, nullptr);
  synchronize(cq, UINT32_MAX);

  for (uint32_t i = 0; i < size; i++) {
    ASSERT_EQ(static_cast<uint8_t *>(memory)[i], pattern)
        << "Memory Fill did not match.";
  }

  free_memory(memory);
}

TEST_F(zeCommandListAppendMemoryFillVerificationTests,
       GivenSharedMemoryWhenExecutingAMemoryFillThenMemoryIsSetCorrectly) {

  size_t size = 16;
  auto memory = allocate_shared_memory(size);
  uint8_t pattern = 0xAB;
  const int pattern_size = 1;

  append_memory_fill(command_list, memory, &pattern, pattern_size, size,
                     nullptr);
  append_barrier(command_list, nullptr, 0, nullptr);
  close_command_list(command_list);
  execute_command_lists(cq, 1, &command_list, nullptr);
  synchronize(cq, UINT32_MAX);

  for (uint32_t i = 0; i < size; i++) {
    ASSERT_EQ(static_cast<uint8_t *>(memory)[i], pattern)
        << "Memory Fill did not match.";
  }

  free_memory(memory);
}

TEST_F(zeCommandListAppendMemoryFillVerificationTests,
       GivenDeviceMemoryWhenExecutingAMemoryFillThenMemoryIsSetCorrectly) {

  size_t size = 16;
  auto memory = allocate_device_memory(size);
  auto local_mem = allocate_host_memory(size);
  uint8_t pattern = 0xAB;
  const int pattern_size = 1;

  append_memory_fill(command_list, memory, &pattern, pattern_size, size,
                     nullptr);
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
    ASSERT_EQ(static_cast<uint8_t *>(local_mem)[i], pattern)
        << "Memory Fill did not match.";
  }

  free_memory(memory);
  free_memory(local_mem);
}

class zeCommandListAppendMemoryFillPatternVerificationTests
    : public zeCommandListAppendMemoryFillVerificationTests,
      public ::testing::WithParamInterface<size_t> {};

TEST_P(zeCommandListAppendMemoryFillPatternVerificationTests,
       GivenPatternSizeWhenExecutingAMemoryFillThenMemoryIsSetCorrectly) {

  const int pattern_size = GetParam();
  const size_t total_size = (pattern_size * 10) + 5;
  auto pattern = new uint8_t[pattern_size];
  auto target_memory = allocate_host_memory(total_size);

  for (uint32_t i = 0; i < pattern_size; i++) {
    pattern[i] = i;
  }

  append_memory_fill(command_list, target_memory, pattern, pattern_size,
                     total_size, nullptr);
  append_barrier(command_list, nullptr, 0, nullptr);
  close_command_list(command_list);
  execute_command_lists(cq, 1, &command_list, nullptr);
  synchronize(cq, UINT32_MAX);

  for (uint32_t i = 0; i < total_size; i++) {
    ASSERT_EQ(static_cast<uint8_t *>(target_memory)[i], i % pattern_size)
        << "Memory Fill did not match.";
  }
  free_memory(target_memory);
  delete[] pattern;
}

INSTANTIATE_TEST_CASE_P(VaryPatternSize,
                        zeCommandListAppendMemoryFillPatternVerificationTests,
                        ::testing::Values(1, 2, 4, 8, 16, 32, 64, 128));

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

class zeCommandListAppendImageCopyTests : public ::testing::Test {
protected:
  enum TEST_IMAGE_COPY_REGION_USE_TYPE {
    TICRUT_IMAGE_COPY_REGION_USE_NULL,
    TICRUT_IMAGE_COPY_REGION_USE_REGIONS
  };
  enum TEST_IMAGE_COPY_MEMORY_TYPE {
    TICMT_IMAGE_COPY_MEMORY_HOST,
    TICMT_IMAGE_COPY_MEMORY_DEVICE,
    TICMT_IMAGE_COPY_MEMORY_SHARED
  };
  void test_image_copy() {
    // dest_host_image_upper is used to validate that the above image copy
    // operation(s) were correct:
    lzt::ImagePNG32Bit dest_host_image_upper(img.dflt_host_image_.width(),
                                             img.dflt_host_image_.height());
    // Scribble a known incorrect data pattern to dest_host_image_upper to
    // ensure we are validating actual data from the L0 functionality:
    lzt::write_image_data_pattern(dest_host_image_upper, -1);

    // First, copy the image from the host to the device:
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyFromMemory(
                  cl.command_list_, img.dflt_device_image_2_,
                  img.dflt_host_image_.raw_data(), nullptr, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // Now, copy the image from the device to the device:
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopy(
                                     cl.command_list_, img.dflt_device_image_,
                                     img.dflt_device_image_2_, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // Finally copy the image from the device to the dest_host_image_upper for
    // validation:
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyToMemory(
                  cl.command_list_, dest_host_image_upper.raw_data(),
                  img.dflt_device_image_, nullptr, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // Execute all of the commands involving copying of images
    close_command_list(cl.command_list_);
    execute_command_lists(cq.command_queue_, 1, &cl.command_list_, nullptr);
    synchronize(cq.command_queue_, UINT32_MAX);
    // Validate the result of the above operations:
    // If the operation is a straight image copy, or the second region is null
    // then the result should be the same:
    EXPECT_EQ(
        0, compare_data_pattern(
               dest_host_image_upper, img.dflt_host_image_, 0, 0,
               img.dflt_host_image_.width(), img.dflt_host_image_.height(), 0,
               0, img.dflt_host_image_.width(), img.dflt_host_image_.height()));
  }
  void test_image_copy_region(const ze_image_region_t *region) {
    ze_image_handle_t h_dest_image = nullptr;
    ze_image_desc_t image_desc = zeImageCreateCommon::dflt_ze_image_desc;
    create_ze_image(h_dest_image, &image_desc);
    ze_image_handle_t h_source_image = nullptr;
    create_ze_image(h_source_image, &image_desc);
    // Define the three data patterns:
    const int8_t background_dp = 1;
    const int8_t foreground_dp = 2;
    const int8_t scribble_dp = 3;
    // The background image:
    lzt::ImagePNG32Bit background_image(img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height());
    // Initialize background image with background data pattern:
    lzt::write_image_data_pattern(background_image, background_dp);
    // The foreground image:
    lzt::ImagePNG32Bit foreground_image(img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height());
    // Initialize foreground image with foreground data pattern:
    lzt::write_image_data_pattern(foreground_image, foreground_dp);
    // new_host_image is used to validate that the image copy region
    // operation(s) were correct:
    lzt::ImagePNG32Bit new_host_image(img.dflt_host_image_.width(),
                                      img.dflt_host_image_.height());
    // Scribble a known incorrect data pattern to new_host_image to ensure we
    // are validating actual data from the L0 functionality:
    lzt::write_image_data_pattern(new_host_image, scribble_dp);
    // First, copy the background image from the host to the device:
    // This will serve as the BACKGROUND of the image.
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyFromMemory(
                  cl.command_list_, h_dest_image, background_image.raw_data(),
                  nullptr, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // Next, copy the foreground image from the host to the device:
    // This will serve as the FOREGROUND of the image.
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyFromMemory(
                  cl.command_list_, h_source_image, foreground_image.raw_data(),
                  nullptr, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // Copy the portion of the foreground image correspoding to the region:
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyRegion(
                                     cl.command_list_, h_dest_image,
                                     h_source_image, region, region, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // Finally, copy the image in hTstImage back to new_host_image for
    // validation:
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyToMemory(
                  cl.command_list_, new_host_image.raw_data(), h_dest_image,
                  nullptr, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // Execute all of the commands involving copying of images
    close_command_list(cl.command_list_);
    execute_command_lists(cq.command_queue_, 1, &cl.command_list_, nullptr);
    synchronize(cq.command_queue_, UINT32_MAX);

    EXPECT_EQ(0, lzt::compare_data_pattern(new_host_image, region,
                                           foreground_image, background_image));

    destroy_ze_image(h_dest_image);
    destroy_ze_image(h_source_image);
    reset_command_list(cl.command_list_);
  }
  void test_image_copy_from_memory(TEST_IMAGE_COPY_MEMORY_TYPE tcmt,
                                   TEST_IMAGE_COPY_REGION_USE_TYPE ticrut) {
    // For the tests involving image copy from memory
    // source_buff contains the allocation
    // for the host, or the device or shared memory, per the
    // TEST_IMAGE_COPY_MEMORY_TYPE specified
    void *source_buff = nullptr;
    // For the tests involving image copy from memory
    // And a non-null region is used, source_buff_2 contains the allocation
    // for the host, or the device or shared memory, per the
    // TEST_IMAGE_COPY_MEMORY_TYPE specified:
    void *source_buff_2 = nullptr;
    // The following four regions are only used when the image copy test uses
    // regions (for the case: TICRUT_IMAGE_COPY_REGION_USE_REGIONS)
    ze_image_region_t source_region1, source_region2, *src_reg_1 = nullptr,
                                                      *src_reg_2 = nullptr;
    ze_image_region_t dest_region1, dest_region2, *dest_reg_1 = nullptr,
                                                  *dest_reg_2 = nullptr;
    // The host_image2 variable is also used on when the image copy test uses
    // regions:
    lzt::ImagePNG32Bit host_image2(img.dflt_host_image_.width(),
                                   img.dflt_host_image_.height());
    if (ticrut == TICRUT_IMAGE_COPY_REGION_USE_REGIONS) {
      // source_region1 and dest_region1 reference the upper part of the image:
      source_region1.originX = 0;
      source_region1.originY = 0;
      source_region1.originZ = 0;
      source_region1.width = img.dflt_host_image_.width();
      source_region1.height = img.dflt_host_image_.height() / 2;
      source_region1.depth = 1;

      dest_region1.originX = 0;
      dest_region1.originY = 0;
      dest_region1.originZ = 0;
      dest_region1.width = img.dflt_host_image_.width();
      dest_region1.height = img.dflt_host_image_.height() / 2;
      dest_region1.depth = 1;

      // source_region2 and dest_region21 reference the lower part of the image:
      source_region2.originX = 0;
      source_region2.originY = img.dflt_host_image_.height() / 2;
      source_region2.originZ = 0;
      source_region2.width = img.dflt_host_image_.width();
      source_region2.height = img.dflt_host_image_.height() / 2;
      source_region2.depth = 1;

      dest_region2.originX = 0;
      dest_region2.originY = img.dflt_host_image_.height() / 2;
      dest_region2.originZ = 0;
      dest_region2.width = img.dflt_host_image_.width();
      dest_region2.height = img.dflt_host_image_.height() / 2;
      dest_region2.depth = 1;

      src_reg_1 = &source_region1;
      src_reg_2 = &source_region2;
      dest_reg_1 = &dest_region1;
      dest_reg_2 = &dest_region2;
    }
    switch (tcmt) {
    default:
      // Unexpected image copy test memory type tcmt.
      FAIL();
      break;
    case TICMT_IMAGE_COPY_MEMORY_HOST:
      source_buff =
          lzt::allocate_host_memory(img.dflt_host_image_.size_in_bytes());
      if (ticrut == TICRUT_IMAGE_COPY_REGION_USE_REGIONS)
        source_buff_2 =
            lzt::allocate_host_memory(img.dflt_host_image_.size_in_bytes());
      break;
    case TICMT_IMAGE_COPY_MEMORY_DEVICE:
      source_buff =
          lzt::allocate_device_memory(img.dflt_host_image_.size_in_bytes());
      if (ticrut == TICRUT_IMAGE_COPY_REGION_USE_REGIONS)
        source_buff_2 =
            lzt::allocate_device_memory(img.dflt_host_image_.size_in_bytes());
      break;
    case TICMT_IMAGE_COPY_MEMORY_SHARED:
      source_buff =
          lzt::allocate_shared_memory(img.dflt_host_image_.size_in_bytes());
      if (ticrut == TICRUT_IMAGE_COPY_REGION_USE_REGIONS)
        source_buff_2 =
            lzt::allocate_shared_memory(img.dflt_host_image_.size_in_bytes());
      break;
    }
    // Starting image #2 is optional, depending if the image copy will
    // copy regions, and is host_image2:
    // In region operations, host_image2 references the lower part of the
    // image:
    lzt::write_image_data_pattern(host_image2, -1);
    // dest_host_image_upper is used to validate that the above image copy
    // operation(s) were correct:
    lzt::ImagePNG32Bit dest_host_image_upper(img.dflt_host_image_.width(),
                                             img.dflt_host_image_.height());
    // Scribble a known incorrect data pattern to dest_host_image_upper to
    // ensure we are validating actual data from the L0 functionality:
    lzt::write_image_data_pattern(dest_host_image_upper, -1);
    // First, copy the entire host image to source_buff:
    append_memory_copy(cl.command_list_, source_buff,
                       img.dflt_host_image_.raw_data(),
                       img.dflt_host_image_.size_in_bytes());
    append_barrier(cl.command_list_);
    if (dest_reg_2 != nullptr) {
      // Next, copy the entire 2nd host image to source_buff_2:
      append_memory_copy(cl.command_list_, source_buff_2,
                         host_image2.raw_data(), host_image2.size_in_bytes());
      append_barrier(cl.command_list_);
    }
    // Now, copy the image from source_buff to the device image:
    // (during a region copy, only the upper-half of source_buff is copied to
    // the device image):
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyFromMemory(
                                     cl.command_list_, img.dflt_device_image_,
                                     source_buff, dest_reg_1, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    if (dest_reg_2 != nullptr) {
      // During a region copy, copy the upper portion of the source_buff_2
      // image to the device:
      EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyFromMemory(
                                       cl.command_list_, img.dflt_device_image_,
                                       source_buff_2, dest_reg_2, nullptr));
      append_barrier(cl.command_list_, nullptr, 0, nullptr);
    }
    // Next, copy the image from the device to the dest_host_image_upper for
    // validation:
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyToMemory(
                  cl.command_list_, dest_host_image_upper.raw_data(),
                  img.dflt_device_image_, nullptr, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // Execute all of the commands involving copying of images
    close_command_list(cl.command_list_);
    execute_command_lists(cq.command_queue_, 1, &cl.command_list_, nullptr);
    synchronize(cq.command_queue_, UINT32_MAX);
    // Validate the result of the above operations:
    if ((dest_reg_1 == nullptr)) {
      // If the operation is a straight image copy, or the second region is null
      // then the result should be the same:
      EXPECT_EQ(0, compare_data_pattern(dest_host_image_upper,
                                        img.dflt_host_image_, 0, 0,
                                        img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height(), 0, 0,
                                        img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height()));
    } else {
      // Otherwise, the result of the operation should be the following:
      // Compare the upper half of the resulting image with the upper portion of
      // the source:
      EXPECT_EQ(0, compare_data_pattern(dest_host_image_upper,
                                        img.dflt_host_image_, 0, 0,
                                        img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height() / 2, 0, 0,
                                        img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height() / 2));
      // Next, compare the lower half of the resulting image with the lower half
      // of the source:
      EXPECT_EQ(0, compare_data_pattern(
                       dest_host_image_upper, host_image2, 0,
                       host_image2.height() / 2, host_image2.width(),
                       host_image2.height() / 2, 0, 0, host_image2.width(),
                       host_image2.height() / 2));
    }
    if (source_buff)
      lzt::free_memory(source_buff);
    if (source_buff_2)
      lzt::free_memory(source_buff_2);
  }
  void test_image_copy_to_memory(TEST_IMAGE_COPY_MEMORY_TYPE tcmt,
                                 TEST_IMAGE_COPY_REGION_USE_TYPE ticrut) {
    // For the tests involving image copy to memory
    // dest_buff contains the allocation
    // for the host, or the device or shared memory, per the
    // TEST_IMAGE_COPY_MEMORY_TYPE specified
    void *dest_buff = nullptr;
    // For the tests involving image copy to memory
    // And a non-null region is used, dest_buff_2 contains the allocation
    // for the host, or the device or shared memory, per the
    // TEST_IMAGE_COPY_MEMORY_TYPE specified:
    void *dest_buff_2 = nullptr;
    // The following regions are only used when the image copy test uses
    // regions (for the the case: TCT_COPY_REGION
    ze_image_region_t upper_region, lower_region, *up_reg = nullptr,
                                                  *low_reg = nullptr;
    // The host_image2 variable is also used on when the image copy test uses
    // regions:
    lzt::ImagePNG32Bit host_image2(img.dflt_host_image_.width(),
                                   img.dflt_host_image_.height());
    if (ticrut == TICRUT_IMAGE_COPY_REGION_USE_REGIONS) {
      upper_region.originX = 0;
      upper_region.originY = 0;
      upper_region.originZ = 0;
      upper_region.width = img.dflt_host_image_.width();
      upper_region.height = img.dflt_host_image_.height() / 2;
      upper_region.depth = 1;

      lower_region.originX = 0;
      lower_region.originY = img.dflt_host_image_.height() / 2;
      lower_region.originZ = 0;
      lower_region.width = img.dflt_host_image_.width();
      lower_region.height = img.dflt_host_image_.height() / 2;
      lower_region.depth = 1;

      up_reg = &upper_region;
      low_reg = &lower_region;
    }

    switch (tcmt) {
    default:
      // Unexpected image copy test memory type tcmt.
      FAIL();
      break;
    case TICMT_IMAGE_COPY_MEMORY_HOST:
      dest_buff =
          lzt::allocate_host_memory(img.dflt_host_image_.size_in_bytes());
      if (ticrut == TICRUT_IMAGE_COPY_REGION_USE_REGIONS)
        dest_buff_2 =
            lzt::allocate_host_memory(img.dflt_host_image_.size_in_bytes());
      break;
    case TICMT_IMAGE_COPY_MEMORY_DEVICE:
      dest_buff =
          lzt::allocate_device_memory(img.dflt_host_image_.size_in_bytes());
      if (ticrut == TICRUT_IMAGE_COPY_REGION_USE_REGIONS)
        dest_buff_2 =
            lzt::allocate_device_memory(img.dflt_host_image_.size_in_bytes());
      break;
    case TICMT_IMAGE_COPY_MEMORY_SHARED:
      dest_buff =
          lzt::allocate_shared_memory(img.dflt_host_image_.size_in_bytes());
      if (ticrut == TICRUT_IMAGE_COPY_REGION_USE_REGIONS)
        dest_buff_2 =
            lzt::allocate_shared_memory(img.dflt_host_image_.size_in_bytes());
      break;
    }
    // Starting image #2 is optional, depending if the image copy will
    // copy regions, and is host_image2:
    // In region operations, host_image2 references the lower part of the
    // image:
    lzt::write_image_data_pattern(host_image2, -1);
    // dest_host_image_upper is used to validate that the above image copy
    // operation(s) were correct:
    lzt::ImagePNG32Bit dest_host_image_upper(img.dflt_host_image_.width(),
                                             img.dflt_host_image_.height());
    lzt::ImagePNG32Bit dest_host_image_lower(img.dflt_host_image_.width(),
                                             img.dflt_host_image_.height());
    // Scribble a known incorrect data pattern to dest_host_image_upper to
    // ensure we are validating actual data from the L0 functionality:
    lzt::write_image_data_pattern(dest_host_image_upper, -1);
    // Copy the image from the host to the device:
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyFromMemory(
                  cl.command_list_, img.dflt_device_image_,
                  img.dflt_host_image_.raw_data(), nullptr, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    if (low_reg != nullptr) {
      // Copy the second host image from the host to the device:
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeCommandListAppendImageCopyFromMemory(
                    cl.command_list_, img.dflt_device_image_2_,
                    host_image2.raw_data(), nullptr, nullptr));
      append_barrier(cl.command_list_, nullptr, 0, nullptr);
    }
    // Next, copy the image from the device to dest_buff:
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyToMemory(
                                     cl.command_list_, dest_buff,
                                     img.dflt_device_image_, up_reg, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    if (low_reg != nullptr) {
      // Copy the image from the host to the device:
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeCommandListAppendImageCopyToMemory(
                    cl.command_list_, dest_buff_2, img.dflt_device_image_2_,
                    low_reg, nullptr));
      append_barrier(cl.command_list_, nullptr, 0, nullptr);
    }
    // Finally, copy the data from dest_buff to dest_host_image_upper_ for
    // validation:
    append_memory_copy(cl.command_list_, dest_host_image_upper.raw_data(),
                       dest_buff, dest_host_image_upper.size_in_bytes());
    if (dest_buff_2) {
      append_memory_copy(cl.command_list_, dest_host_image_lower.raw_data(),
                         dest_buff_2, dest_host_image_upper.size_in_bytes());
    }
    append_barrier(cl.command_list_);
    // Execute all of the commands involving copying of images
    close_command_list(cl.command_list_);
    execute_command_lists(cq.command_queue_, 1, &cl.command_list_, nullptr);
    synchronize(cq.command_queue_, UINT32_MAX);
    // Validate the result of the above operations:
    if ((up_reg == nullptr)) {
      // If the operation is a straight image copy, or the second region is null
      // then the result should be the same:
      EXPECT_EQ(0, compare_data_pattern(dest_host_image_upper,
                                        img.dflt_host_image_, 0, 0,
                                        img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height(), 0, 0,
                                        img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height()));
    } else {
      // Otherwise, the result of the operation should be the following:
      // Compare the upper half of the resulting image with the upper portion of
      // the source:
      EXPECT_EQ(0, compare_data_pattern(dest_host_image_upper,
                                        img.dflt_host_image_, 0, 0,
                                        img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height() / 2, 0, 0,
                                        img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height() / 2));
      // Next, compare the lower half of the resulting image with the lower half
      // of the source:
      EXPECT_EQ(0, compare_data_pattern(
                       dest_host_image_lower, host_image2, 0, 0,
                       host_image2.width(), host_image2.height() / 2, 0,
                       host_image2.height() / 2, host_image2.width(),
                       host_image2.height() / 2));
    }
    if (dest_buff)
      lzt::free_memory(dest_buff);
    if (dest_buff_2)
      lzt::free_memory(dest_buff_2);
  }

  void image_region_copy(const ze_image_region_t &in_region,
                         const ze_image_region_t &out_region) {
    // Create and initialize input and output images.
    lzt::ImagePNG32Bit in_image =
        lzt::ImagePNG32Bit(in_region.width, in_region.height);
    lzt::ImagePNG32Bit out_image =
        lzt::ImagePNG32Bit(out_region.width, out_region.height);

    for (auto y = 0; y < in_region.height; y++)
      for (auto x = 0; x < in_region.width; x++)
        in_image.set_pixel(x, y, x + (y * in_region.width));

    for (auto y = 0; y < out_region.height; y++)
      for (auto x = 0; x < out_region.width; x++)
        out_image.set_pixel(x, y, 0xffffffff);

    // Copy from host image to to device image region
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyFromMemory(
                                     cl.command_list_, img.dflt_device_image_,
                                     in_image.raw_data(), &in_region, nullptr));

    append_barrier(cl.command_list_, nullptr, 0, nullptr);

    // Copy from image region to output host image
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyToMemory(
                  cl.command_list_, out_image.raw_data(),
                  img.dflt_device_image_, &out_region, nullptr));

    // Execute
    close_command_list(cl.command_list_);
    execute_command_lists(cq.command_queue_, 1, &cl.command_list_, nullptr);
    synchronize(cq.command_queue_, UINT32_MAX);

    // Verify output image matches initial host image.
    // Output image contains input image data shifted by in_region's origin
    // minus out_region's origin.  Some of the original data may not make it
    // to the output due to sizes and offests, and there may be junk data in
    // parts of the output image that don't have coresponding pixels in the
    // input; we will ignore those.
    // We may pass negative origin coordinates to compare_data_pattern; in that
    // case, it will skip over any negative-index pixels.
    EXPECT_EQ(0, compare_data_pattern(in_image, out_image, 0, 0,
                                      in_region.width, in_region.height,
                                      in_region.originX - out_region.originX,
                                      in_region.originY - out_region.originY,
                                      out_region.width, out_region.height));
  }

  zeEventPool ep;
  zeCommandList cl;
  zeCommandQueue cq;
  zeImageCreateCommon img;
};

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy();
}

static inline ze_image_region_t init_region(uint32_t originX, uint32_t originY,
                                            uint32_t originZ, uint32_t width,
                                            uint32_t height, uint32_t depth) {
  ze_image_region_t rv = {originX, originY, originZ, width, height, depth};
  return rv;
}

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyRegionWithVariousRegionsThenImageIsCorrectAndSuccessIsReturned) {
  //  (C 1)
  LOG_DEBUG << "Starting test of nullptr region." << std::endl;
  test_image_copy_region(nullptr);
  LOG_DEBUG << "Completed test of nullptr region" << std::endl;
  // Aliases to reduce widths of the following region initializers
  const uint32_t width = img.dflt_host_image_.width();
  const uint32_t height = img.dflt_host_image_.height();
  ze_image_region_t regions[] = {
      // Region correspond to the entire image (C 2) (0)
      init_region(0, 0, 0, width, height, 1),

      // Entire image less 1 pixel in height, top (region touches 3 out of 4
      // borders): (C 3) (1)  Does not touch the bottom
      init_region(0, 0, 0, width, height - 1, 1),
      // Entire image less 1 pixel in height, bottom (region touches 3 out of 4
      // borders): (C 4) (2) Does not touch the top
      init_region(0, 1, 0, width, height - 1, 1),
      // Entire image less 1 pixel in width, left (region touches 3 out of 4
      // borders): (C 5) (3) Does not touch the right
      init_region(0, 0, 0, width - 1, height, 1),
      // Entire image less 1 pixel in width, right (region touches 3 out of 4
      // borders): (C 6) (4) Does not touch the left
      init_region(1, 0, 0, width - 1, height, 1),

      // Entire image less (1 pixel in width and 1 pixel in height), top, left
      // (region touches 2 out of 4 borders): (C 7) (5)
      init_region(0, 0, 0, width - 1, height - 1, 1),
      // Entire image less (1 pixel in width and 1 pixel in height), top, right
      // (region touches 2 out of 4 borders): (C 8) (6)
      init_region(1, 0, 0, width - 1, height - 1, 1),
      // Entire image less (1 pixel in width and 1 pixel in height), bottom,
      // right (region touches 2 out of 4 borders): (C 9) (7)
      init_region(1, 1, 0, width - 1, height - 1, 1),
      // Entire image less (1 pixel in width and 1 pixel in height), bottom,
      // left (region touches 2 out of 4 borders): (C 10)
      init_region(0, 1, 0, width - 1, height - 1, 1),
      // Entire image less 2 pixels in width, top, bottom (region touches 2 out
      // of 4 borders): (C 11)
      init_region(0, 1, 0, width - 2, height, 1),
      // Entire image less 2 pixels in height, right, left (region touches 2 out
      // of 4 borders): (C 12)
      init_region(1, 0, 0, width, height - 2, 1),

      // Entire image less (2 pixels in width and 1 pixel in height), touches
      // only top border (region touches 1 out of 4 borders): (C 13)
      init_region(1, 0, 0, width - 2, height - 1, 1),
      // Entire image less (2 pixels in width and 1 pixel in height), touches
      // only bottom border (region touches 1 out of 4 borders): (C 14)
      init_region(1, 1, 0, width - 2, height - 1, 1),
      // Entire image less (1 pixel in width and 2 pixels in height), touches
      // only left border (region touches 1 out of 4 borders): (C 15)
      init_region(0, 1, 0, width - 1, height - 2, 1),
      // Entire image less (1 pixel in width and 2 pixels in height), touches
      // only right border (region touches 1 out of 4 borders): (C 16)
      init_region(1, 1, 0, width - 1, height - 2, 1),

      // Entire image less (2 pixels in width and 2 pixels in height), centered
      // (region touches no borders): (C 17)
      init_region(1, 1, 0, width - 2, height - 2, 1),
      // Column, with width 1, left-most: (C 18)
      init_region(0, 0, 0, 1, height, 1),
      // Column, with width 1, right-most: (C 19)
      init_region(width - 1, 0, 0, 1, height, 1),
      // Column, with width 1, center: (C 20)
      init_region(width / 2, 0, 0, 1, height, 1),
      // Row, with height 1, top: (C 21)
      init_region(0, 0, 0, width, 1, 1),
      // Row, with height 1, bottom: (C 22)
      init_region(0, height - 1, 0, width, 1, 1),
      // Row, with height 1, center: (C 23)
      init_region(0, height / 2, 0, width, 1, 1),

      // One pixel, at center: (C 24)
      init_region(width / 2, height / 2, 1, 1, 1, 1),

      // One pixel, at upper-left: (C 25)
      init_region(0, 0, 1, 1, 1, 1),
      // One pixel, at upper-right: (C 26)
      init_region(width - 1, 0, 1, 1, 1, 1),
      // One pixel, at lower-right: (C 27)
      init_region(width - 1, height - 1, 1, 1, 1, 1),
      // One pixel, at lower-leftt: (C 28)
      init_region(0, height - 1, 1, 1, 1, 1),
  };

  for (size_t i = 0; i < sizeof(regions) / sizeof(regions[0]); i++) {
    LOG_DEBUG << "Starting test of region: " << i << std::endl;
    test_image_copy_region(&(regions[i]));
    LOG_DEBUG << "Completed test of region: " << i << std::endl;
  }
}

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyFromMemoryUsingHostMemoryWithNonNullRegionsThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy_from_memory(TICMT_IMAGE_COPY_MEMORY_HOST,
                              TICRUT_IMAGE_COPY_REGION_USE_REGIONS);
}

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyFromMemoryUsingHostMemoryWithNullRegionsThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy_from_memory(TICMT_IMAGE_COPY_MEMORY_HOST,
                              TICRUT_IMAGE_COPY_REGION_USE_NULL);
}

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyFromMemoryUsingDeviceMemoryWithNonNullRegionsThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy_from_memory(TICMT_IMAGE_COPY_MEMORY_DEVICE,
                              TICRUT_IMAGE_COPY_REGION_USE_REGIONS);
}

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyFromMemoryUsingDeviceMemoryWithNullRegionsThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy_from_memory(TICMT_IMAGE_COPY_MEMORY_DEVICE,
                              TICRUT_IMAGE_COPY_REGION_USE_NULL);
}

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyFromMemoryUsingSharedMemoryWithNonNullRegionsThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy_from_memory(TICMT_IMAGE_COPY_MEMORY_SHARED,
                              TICRUT_IMAGE_COPY_REGION_USE_REGIONS);
}

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyFromMemoryUsingSharedMemoryWithNullRegionsThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy_from_memory(TICMT_IMAGE_COPY_MEMORY_SHARED,
                              TICRUT_IMAGE_COPY_REGION_USE_NULL);
}

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyToMemoryUsingHostMemoryWithNonNullRegionsThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy_to_memory(TICMT_IMAGE_COPY_MEMORY_HOST,
                            TICRUT_IMAGE_COPY_REGION_USE_REGIONS);
}

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyToMemoryUsingHostMemoryWithNullRegionsThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy_to_memory(TICMT_IMAGE_COPY_MEMORY_HOST,
                            TICRUT_IMAGE_COPY_REGION_USE_NULL);
}

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyToMemoryUsingDeviceMemoryWithNonNullRegionsThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy_to_memory(TICMT_IMAGE_COPY_MEMORY_DEVICE,
                            TICRUT_IMAGE_COPY_REGION_USE_REGIONS);
}

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyToMemoryUsingDeviceMemoryWithNullRegionsThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy_to_memory(TICMT_IMAGE_COPY_MEMORY_DEVICE,
                            TICRUT_IMAGE_COPY_REGION_USE_NULL);
}

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyToMemoryUsingSharedMemoryWithNonNullRegionsThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy_to_memory(TICMT_IMAGE_COPY_MEMORY_SHARED,
                            TICRUT_IMAGE_COPY_REGION_USE_REGIONS);
}

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyToMemoryUsingSharedMemoryWithNullRegionsThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy_to_memory(TICMT_IMAGE_COPY_MEMORY_SHARED,
                            TICRUT_IMAGE_COPY_REGION_USE_NULL);
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

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageWhenAppendingImageCopyToMemoryAndFromMemoryWithOffsetsImageIsCorrectAndSuccessIsReturned) {

  int full_width = img.dflt_host_image_.width();
  int full_height = img.dflt_host_image_.height();

  EXPECT_GE(full_width, 10);
  EXPECT_GE(full_height, 10);

  // To verify regions are respected, we use input and output host images
  // that are slightly smaller than the full width of the device image, and
  // small arbitrary offsets of the origin.  These should be chosen to fit
  // within the 10x10 minimum size we've checked for above.
  auto in_region = init_region(2, 5, 0, full_width - 8, full_height - 7, 1);
  auto out_region = init_region(3, 1, 0, full_width - 6, full_height - 2, 1);

  image_region_copy(in_region, out_region);
}

} // namespace
