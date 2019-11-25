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
    // new_host_image is used to validate that the above image copy operation(s)
    // were correct:
    lzt::ImagePNG32Bit new_host_image(img.dflt_host_image_.width(),
                                      img.dflt_host_image_.height());
    // Scribble a known incorrect data pattern to new_host_image to ensure we
    // are validating actual data from the L0 functionality:
    lzt::write_data_pattern(new_host_image, -1);

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
    // Finally copy the image from the device to the new_host_image for
    // validation:
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyToMemory(
                  cl.command_list_, new_host_image.raw_data(),
                  img.dflt_device_image_, nullptr, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    // Execute all of the commands involving copying of images
    close_command_list(cl.command_list_);
    execute_command_lists(cq.command_queue_, 1, &cl.command_list_, nullptr);
    synchronize(cq.command_queue_, UINT32_MAX);
    // Validate the result of the above operations:
    // If the operation is a straight image copy, or the second region is null
    // then the result should be the same:
    EXPECT_EQ(0, compare_data_pattern(new_host_image, img.dflt_host_image_, 0,
                                      0, img.dflt_host_image_.width(),
                                      img.dflt_host_image_.height(), 0, 0,
                                      img.dflt_host_image_.width(),
                                      img.dflt_host_image_.height()));
  }
  void test_image_copy_region(TEST_IMAGE_COPY_REGION_USE_TYPE ticrut) {
    ze_image_region_t source_region1, source_region2, *src_reg_1 = nullptr,
                                                      *src_reg_2 = nullptr;
    ze_image_region_t dest_region1, dest_region2, *dest_reg_1 = nullptr,
                                                  *dest_reg_2 = nullptr;
    // The host_image2 variable is also used on when the image copy test uses
    // regions:
    lzt::ImagePNG32Bit host_image2(img.dflt_host_image_.width(),
                                   img.dflt_host_image_.height());
    // The hTstImage variable is only used when the image copy test uses
    // regions:
    ze_image_handle_t hTstImage = nullptr;

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
      lzt::write_data_pattern(host_image2, -1);
    }

    ze_image_desc_t image_desc = img.get_dflt_ze_image_desc();
    create_ze_image(hTstImage, &image_desc);
    // new_host_image is used to validate that the above image copy operation(s)
    // were correct:
    lzt::ImagePNG32Bit new_host_image(img.dflt_host_image_.width(),
                                      img.dflt_host_image_.height());
    // Scribble a known incorrect data pattern to new_host_image to ensure we
    // are validating actual data from the L0 functionality:
    lzt::write_data_pattern(new_host_image, -1);
    // First, copy the default image from the host to the device:
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyFromMemory(
                  cl.command_list_, img.dflt_device_image_,
                  img.dflt_host_image_.raw_data(), nullptr, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    if (dest_reg_2 != nullptr) {
      // Next, copy the optional image from the host to the device:
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeCommandListAppendImageCopyFromMemory(
                    cl.command_list_, img.dflt_device_image_2_,
                    host_image2.raw_data(), nullptr, nullptr));
      append_barrier(cl.command_list_, nullptr, 0, nullptr);
    }
    // Copy the upper portion of the image first:
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyRegion(
                  cl.command_list_, hTstImage, img.dflt_device_image_,
                  dest_reg_1, src_reg_1, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    if (dest_reg_2 != nullptr) {
      // Copy the lower portion of the image next:
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeCommandListAppendImageCopyRegion(
                    cl.command_list_, hTstImage, img.dflt_device_image_2_,
                    dest_reg_2, src_reg_2, nullptr));
      append_barrier(cl.command_list_, nullptr, 0, nullptr);
    }
    // Finally, copy the image in hTstImage back to new_host_image for
    // validation:
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyToMemory(
                  cl.command_list_, new_host_image.raw_data(), hTstImage,
                  nullptr, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);

    // Execute all of the commands involving copying of images
    close_command_list(cl.command_list_);
    execute_command_lists(cq.command_queue_, 1, &cl.command_list_, nullptr);
    synchronize(cq.command_queue_, UINT32_MAX);
    // Validate the result of the above operations:
    if (dest_reg_1 == nullptr) {
      // If the operation is a straight image copy, or the second region is null
      // then the result should be the same:
      EXPECT_EQ(0, compare_data_pattern(new_host_image, img.dflt_host_image_, 0,
                                        0, img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height(), 0, 0,
                                        img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height()));
    } else {
      // Otherwise, the result of the operation should be the following:
      // Compare the upper half of the resulting image with the upper portion of
      // the source:
      EXPECT_EQ(0, compare_data_pattern(new_host_image, img.dflt_host_image_, 0,
                                        0, img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height() / 2, 0, 0,
                                        img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height() / 2));
      // Next, compare the lower half of the resulting image with the lower half
      // of the source:
      EXPECT_EQ(0, compare_data_pattern(
                       new_host_image, host_image2, 0, host_image2.height() / 2,
                       host_image2.width(), host_image2.height() / 2, 0, 0,
                       host_image2.width(), host_image2.height() / 2));
    }
    if (hTstImage)
      destroy_ze_image(hTstImage);
  }
  void test_image_copy_from_memory(TEST_IMAGE_COPY_MEMORY_TYPE tcmt,
                                   TEST_IMAGE_COPY_REGION_USE_TYPE ticrut) {
    // For the tests involving image copy from & to memory
    // hds_memory contains the allocation
    // for the host, or the device or shared memory, per the
    // TEST_IMAGE_COPY_MEMORY_TYPE specified
    void *hds_memory = nullptr;
    // For the tests involving image copy from memory
    // And a non-null region is used, hds_memory_2 contains the allocation
    // for the host, or the device or shared memory, per the
    // TEST_IMAGE_COPY_MEMORY_TYPE specified:
    void *hds_memory_2 = nullptr;
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
      hds_memory =
          lzt::allocate_host_memory(img.dflt_host_image_.size_in_bytes());
      if (ticrut == TICRUT_IMAGE_COPY_REGION_USE_REGIONS)
        hds_memory_2 =
            lzt::allocate_host_memory(img.dflt_host_image_.size_in_bytes());
      break;
    case TICMT_IMAGE_COPY_MEMORY_DEVICE:
      hds_memory =
          lzt::allocate_device_memory(img.dflt_host_image_.size_in_bytes());
      if (ticrut == TICRUT_IMAGE_COPY_REGION_USE_REGIONS)
        hds_memory_2 =
            lzt::allocate_device_memory(img.dflt_host_image_.size_in_bytes());
      break;
    case TICMT_IMAGE_COPY_MEMORY_SHARED:
      hds_memory =
          lzt::allocate_shared_memory(img.dflt_host_image_.size_in_bytes());
      if (ticrut == TICRUT_IMAGE_COPY_REGION_USE_REGIONS)
        hds_memory_2 =
            lzt::allocate_shared_memory(img.dflt_host_image_.size_in_bytes());
      break;
    }
    // Starting image #2 is optional, depending if the image copy will
    // copy regions, and is host_image2:
    // In region operations, host_image2 references the lower part of the
    // image:
    lzt::write_data_pattern(host_image2, -1);
    // new_host_image is used to validate that the above image copy operation(s)
    // were correct:
    lzt::ImagePNG32Bit new_host_image(img.dflt_host_image_.width(),
                                      img.dflt_host_image_.height());
    // Scribble a known incorrect data pattern to new_host_image to ensure we
    // are validating actual data from the L0 functionality:
    lzt::write_data_pattern(new_host_image, -1);
    // First, copy the entire host image to hds_memory:
    append_memory_copy(cl.command_list_, hds_memory,
                       img.dflt_host_image_.raw_data(),
                       img.dflt_host_image_.size_in_bytes());
    append_barrier(cl.command_list_);
    if (dest_reg_2 != nullptr) {
      // Next, copy the entire 2nd host image to hds_memory_2:
      append_memory_copy(cl.command_list_, hds_memory_2, host_image2.raw_data(),
                         host_image2.size_in_bytes());
      append_barrier(cl.command_list_);
    }
    // Now, copy the image from hds_memory to the device image:
    // (during a region copy, only the upper-half of hds_memory is copied to
    // the device image):
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyFromMemory(
                                     cl.command_list_, img.dflt_device_image_,
                                     hds_memory, dest_reg_1, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    if (dest_reg_2 != nullptr) {
      // During a region copy, copy the upper portion of the hds_memory_2
      // image to the device:
      EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendImageCopyFromMemory(
                                       cl.command_list_, img.dflt_device_image_,
                                       hds_memory_2, dest_reg_2, nullptr));
      append_barrier(cl.command_list_, nullptr, 0, nullptr);
    }
    // Next, copy the image from the device to the new_host_image for
    // validation:
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyToMemory(
                  cl.command_list_, new_host_image.raw_data(),
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
      EXPECT_EQ(0, compare_data_pattern(new_host_image, img.dflt_host_image_, 0,
                                        0, img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height(), 0, 0,
                                        img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height()));
    } else {
      // Otherwise, the result of the operation should be the following:
      // Compare the upper half of the resulting image with the upper portion of
      // the source:
      EXPECT_EQ(0, compare_data_pattern(new_host_image, img.dflt_host_image_, 0,
                                        0, img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height() / 2, 0, 0,
                                        img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height() / 2));
      // Next, compare the lower half of the resulting image with the lower half
      // of the source:
      EXPECT_EQ(0, compare_data_pattern(
                       new_host_image, host_image2, 0, host_image2.height() / 2,
                       host_image2.width(), host_image2.height() / 2, 0, 0,
                       host_image2.width(), host_image2.height() / 2));
    }
    if (hds_memory)
      lzt::free_memory(hds_memory);
    if (hds_memory_2)
      lzt::free_memory(hds_memory_2);
  }
  void test_image_copy_to_memory(TEST_IMAGE_COPY_MEMORY_TYPE tcmt,
                                 TEST_IMAGE_COPY_REGION_USE_TYPE ticrut) {
    // For the tests involving image copy from & to memory
    // hds_memory contains the allocation
    // for the host, or the device or shared memory, per the
    // TEST_IMAGE_COPY_MEMORY_TYPE specified
    void *hds_memory = nullptr;
    // For the tests involving image copy from & to memory
    // And a non-null region is used, hds_memory_2 contains the allocation
    // for the host, or the device or shared memory, per the
    // TEST_IMAGE_COPY_MEMORY_TYPE specified:
    void *hds_memory_2 = nullptr;
    // The following four regions are only used when the image copy test uses
    // regions (for the the case: TCT_COPY_REGION
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
      hds_memory =
          lzt::allocate_host_memory(img.dflt_host_image_.size_in_bytes());
      if (ticrut == TICRUT_IMAGE_COPY_REGION_USE_REGIONS)
        hds_memory_2 =
            lzt::allocate_host_memory(img.dflt_host_image_.size_in_bytes());
      break;
    case TICMT_IMAGE_COPY_MEMORY_DEVICE:
      hds_memory =
          lzt::allocate_device_memory(img.dflt_host_image_.size_in_bytes());
      if (ticrut == TICRUT_IMAGE_COPY_REGION_USE_REGIONS)
        hds_memory_2 =
            lzt::allocate_device_memory(img.dflt_host_image_.size_in_bytes());
      break;
    case TICMT_IMAGE_COPY_MEMORY_SHARED:
      hds_memory =
          lzt::allocate_shared_memory(img.dflt_host_image_.size_in_bytes());
      if (ticrut == TICRUT_IMAGE_COPY_REGION_USE_REGIONS)
        hds_memory_2 =
            lzt::allocate_shared_memory(img.dflt_host_image_.size_in_bytes());
      break;
    }
    // Starting image #2 is optional, depending if the image copy will
    // copy regions, and is host_image2:
    // In region operations, host_image2 references the lower part of the
    // image:
    lzt::write_data_pattern(host_image2, -1);
    // new_host_image is used to validate that the above image copy operation(s)
    // were correct:
    lzt::ImagePNG32Bit new_host_image(img.dflt_host_image_.width(),
                                      img.dflt_host_image_.height());
    // Scribble a known incorrect data pattern to new_host_image to ensure we
    // are validating actual data from the L0 functionality:
    lzt::write_data_pattern(new_host_image, -1);
    // Copy the image from the host to the device:
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyFromMemory(
                  cl.command_list_, img.dflt_device_image_,
                  img.dflt_host_image_.raw_data(), nullptr, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    if (dest_reg_2 != nullptr) {
      // Copy the second host image from the host to the device:
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeCommandListAppendImageCopyFromMemory(
                    cl.command_list_, img.dflt_device_image_2_,
                    host_image2.raw_data(), nullptr, nullptr));
      append_barrier(cl.command_list_, nullptr, 0, nullptr);
    }
    // Next, copy the image from the device to hds_memory:
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendImageCopyToMemory(cl.command_list_, hds_memory,
                                                   img.dflt_device_image_,
                                                   dest_reg_1, nullptr));
    append_barrier(cl.command_list_, nullptr, 0, nullptr);
    if (dest_reg_2 != nullptr) {
      // Copy the image from the host to the device:
      EXPECT_EQ(ZE_RESULT_SUCCESS,
                zeCommandListAppendImageCopyToMemory(
                    cl.command_list_, hds_memory, img.dflt_device_image_2_,
                    dest_reg_2, nullptr));
      append_barrier(cl.command_list_, nullptr, 0, nullptr);
    }
    // Finally, copy the data from hds_memory to new_host_image_ for
    // validation:
    append_memory_copy(cl.command_list_, new_host_image.raw_data(), hds_memory,
                       new_host_image.size_in_bytes());
    append_barrier(cl.command_list_);
    // Execute all of the commands involving copying of images
    close_command_list(cl.command_list_);
    execute_command_lists(cq.command_queue_, 1, &cl.command_list_, nullptr);
    synchronize(cq.command_queue_, UINT32_MAX);
    // Validate the result of the above operations:
    if ((dest_reg_1 == nullptr)) {
      // If the operation is a straight image copy, or the second region is null
      // then the result should be the same:
      EXPECT_EQ(0, compare_data_pattern(new_host_image, img.dflt_host_image_, 0,
                                        0, img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height(), 0, 0,
                                        img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height()));
    } else {
      // Otherwise, the result of the operation should be the following:
      // Compare the upper half of the resulting image with the upper portion of
      // the source:
      EXPECT_EQ(0, compare_data_pattern(new_host_image, img.dflt_host_image_, 0,
                                        0, img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height() / 2, 0, 0,
                                        img.dflt_host_image_.width(),
                                        img.dflt_host_image_.height() / 2));
      // Next, compare the lower half of the resulting image with the lower half
      // of the source:
      EXPECT_EQ(0, compare_data_pattern(
                       new_host_image, host_image2, 0, host_image2.height() / 2,
                       host_image2.width(), host_image2.height() / 2, 0, 0,
                       host_image2.width(), host_image2.height() / 2));
    }
    if (hds_memory)
      lzt::free_memory(hds_memory);
    if (hds_memory_2)
      lzt::free_memory(hds_memory_2);
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

// TEST_FAILS: For details on the failure, please see:
// https://jira.devtools.intel.com/browse/LOKI-458
TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyRegionWithNonNullRegionsThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy_region(TICRUT_IMAGE_COPY_REGION_USE_REGIONS);
}

TEST_F(
    zeCommandListAppendImageCopyTests,
    GivenDeviceImageAndHostImageWhenAppendingImageCopyRegionWithNullRegionsThenImageIsCorrectAndSuccessIsReturned) {
  test_image_copy_region(TICRUT_IMAGE_COPY_REGION_USE_NULL);
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

} // namespace
