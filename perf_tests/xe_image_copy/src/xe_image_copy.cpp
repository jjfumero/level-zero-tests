/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2019-2020 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
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

#include "xe_image_copy.h"

XeImageCopy ::XeImageCopy() {

  benchmark = new XeApp();
  benchmark->singleDeviceInit();

  /*
   * For single device, create command Queue, command list_a, command_list_b and
   * image with xe_app
   */

  benchmark->commandQueueCreate(ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS,
                                &this->command_queue);
  benchmark->commandListCreate(&this->command_list);
  benchmark->commandListCreate(&this->command_list_a);
  benchmark->commandListCreate(&this->command_list_b);
}

XeImageCopy ::~XeImageCopy() {

  benchmark->commandQueueDestroy(this->command_queue);
  benchmark->commandListDestroy(this->command_list);
  benchmark->commandListDestroy(this->command_list_a);
  benchmark->commandListDestroy(this->command_list_b);
  benchmark->singleDeviceCleanup();

  delete benchmark;
}

bool XeImageCopy::is_json_output_enabled(void) {
  return JsonFileName.size() != 0;
}

// It is image copy measure from Host->Device->Host
void XeImageCopy::measureHost2Device2Host() {

  const size_t size = 4 * width * height * depth; /* 4 channels per pixel */

  Timer<std::chrono::microseconds::period> timer;
  long double total_time_usec;
  long double total_time_s;
  long double total_data_transfer;

  ze_result_t result = ZE_RESULT_SUCCESS;
  ze_image_region_t Region = {xOffset, yOffset, zOffset, width, height, depth};

  ze_image_format_desc_t formatDesc = {Imagelayout,
                                       Imageformat,
                                       ZE_IMAGE_FORMAT_SWIZZLE_R,
                                       ZE_IMAGE_FORMAT_SWIZZLE_0,
                                       ZE_IMAGE_FORMAT_SWIZZLE_0,
                                       ZE_IMAGE_FORMAT_SWIZZLE_1};

  ze_image_desc_t imageDesc = {ZE_IMAGE_DESC_VERSION_CURRENT,
                               Imageflags,
                               Imagetype,
                               formatDesc,
                               width,
                               height,
                               depth,
                               0,
                               0};

  uint8_t *srcBuffer = new uint8_t[size];
  uint8_t *dstBuffer = new uint8_t[size];
  for (size_t i = 0; i < size; ++i) {
    srcBuffer[i] = static_cast<uint8_t>(i);
    dstBuffer[i] = 0xff;
  }

  benchmark->imageCreate(&imageDesc, &this->image);

  // Copy from srcBuffer->Image->dstBuffer, so at the end dstBuffer = srcBuffer
  benchmark->commandListAppendImageCopyFromMemory(command_list, image,
                                                  srcBuffer, &Region);
  benchmark->commandListAppendBarrier(command_list);
  benchmark->commandListAppendImageCopyToMemory(command_list, dstBuffer, image,
                                                &Region);
  benchmark->commandListClose(command_list);

  /* Warm up */
  for (int i = 0; i < warm_up_iterations; i++) {
    benchmark->commandQueueExecuteCommandList(command_queue, 1, &command_list);
    benchmark->commandQueueSynchronize(command_queue);
  }

  // Measure the bandwidth of copy from host to device to host only
  timer.start();
  for (int i = 0; i < number_iterations; i++) {
    SUCCESS_OR_TERMINATE(zeCommandQueueExecuteCommandLists(
        command_queue, 1, &command_list, nullptr));
    SUCCESS_OR_TERMINATE(zeCommandQueueSynchronize(command_queue, UINT32_MAX));
  }
  timer.end();

  total_time_usec = timer.period_minus_overhead();
  total_time_s = total_time_usec / 1e6;

  /* Buffer size is multiplied by 2 to account for the round trip image copy */
  total_data_transfer = (2 * size * number_iterations) /
                        static_cast<long double>(1e9); /* Units in Gigabytes */

  gbps = total_data_transfer / total_time_s;

  std::cout << gbps << " GBPS\n";
  if (this->data_validation) {
    validRet = (0 == memcmp(srcBuffer, dstBuffer, size));
  }

  benchmark->imageDestroy(this->image);
  delete[] srcBuffer;
  delete[] dstBuffer;
}

void XeImageCopy::measureHost2Device() {

  const size_t size = 4 * width * height * depth; /* 4 channels per pixel */

  Timer<std::chrono::microseconds::period> timer;
  long double total_time_usec;
  long double total_time_s;
  long double total_data_transfer;

  ze_result_t result = ZE_RESULT_SUCCESS;
  ze_image_region_t Region = {xOffset, yOffset, zOffset, width, height, depth};
  ze_image_format_desc_t formatDesc = {Imagelayout,
                                       Imageformat,
                                       ZE_IMAGE_FORMAT_SWIZZLE_R,
                                       ZE_IMAGE_FORMAT_SWIZZLE_0,
                                       ZE_IMAGE_FORMAT_SWIZZLE_0,
                                       ZE_IMAGE_FORMAT_SWIZZLE_1};

  ze_image_desc_t imageDesc = {ZE_IMAGE_DESC_VERSION_CURRENT,
                               Imageflags,
                               Imagetype,
                               formatDesc,
                               width,
                               height,
                               depth,
                               0,
                               0};

  uint8_t *srcBuffer = new uint8_t[size];
  uint8_t *dstBuffer = new uint8_t[size];
  for (size_t i = 0; i < size; ++i) {
    srcBuffer[i] = static_cast<uint8_t>(i);
    dstBuffer[i] = 0xff;
  }

  benchmark->imageCreate(&imageDesc, &this->image);

  // Copy from srcBuffer->Image->dstBuffer, so at the end dstBuffer = srcBuffer
  benchmark->commandListReset(command_list_a);
  for (int i = 0; i < number_iterations; i++) {
    benchmark->commandListAppendImageCopyFromMemory(command_list_a, image,
                                                    srcBuffer, &Region);
  }
  benchmark->commandListClose(command_list_a);

  benchmark->commandListReset(command_list_b);
  benchmark->commandListAppendImageCopyToMemory(command_list_b, dstBuffer,
                                                image, &Region);
  benchmark->commandListClose(command_list_b);

  /* Warm up */
  benchmark->commandQueueExecuteCommandList(command_queue, 1, &command_list_a);
  benchmark->commandQueueSynchronize(command_queue);

  benchmark->commandQueueExecuteCommandList(command_queue, 1, &command_list_b);
  benchmark->commandQueueSynchronize(command_queue);

  // Measure the bandwidth of copy from host to device only
  timer.start();
  SUCCESS_OR_TERMINATE(zeCommandQueueExecuteCommandLists(
      command_queue, 1, &command_list_a, nullptr));
  SUCCESS_OR_TERMINATE(zeCommandQueueSynchronize(command_queue, UINT32_MAX));
  timer.end();

  // The below commands for command_list_b for final validation at the end
  benchmark->commandQueueExecuteCommandList(command_queue, 1, &command_list_b);
  benchmark->commandQueueSynchronize(command_queue);

  total_time_usec = timer.period_minus_overhead();
  total_time_s = total_time_usec / 1e6;

  total_data_transfer = (size * number_iterations) /
                        static_cast<long double>(1e9); /* Units in Gigabytes */

  gbps = total_data_transfer / total_time_s;
  std::cout << gbps << " GBPS\n";
  latency = total_time_usec / static_cast<long double>(number_iterations);
  std::cout << std::setprecision(11) << latency << " us"
            << " (Latency: Host->Device)" << std::endl;
  if (this->data_validation) {
    validRet = (0 == memcmp(srcBuffer, dstBuffer, size));
  }

  benchmark->imageDestroy(this->image);
  delete[] srcBuffer;
  delete[] dstBuffer;
}

void XeImageCopy::measureDevice2Host() {

  const size_t size = 4 * width * height * depth; /* 4 channels per pixel */

  Timer<std::chrono::microseconds::period> timer;
  long double total_time_usec;
  long double total_time_s;
  long double total_data_transfer;

  ze_result_t result = ZE_RESULT_SUCCESS;
  ze_image_region_t Region = {xOffset, yOffset, zOffset, width, height, depth};

  ze_image_format_desc_t formatDesc = {Imagelayout,
                                       Imageformat,
                                       ZE_IMAGE_FORMAT_SWIZZLE_R,
                                       ZE_IMAGE_FORMAT_SWIZZLE_0,
                                       ZE_IMAGE_FORMAT_SWIZZLE_0,
                                       ZE_IMAGE_FORMAT_SWIZZLE_1};

  ze_image_desc_t imageDesc = {ZE_IMAGE_DESC_VERSION_CURRENT,
                               Imageflags,
                               Imagetype,
                               formatDesc,
                               width,
                               height,
                               depth,
                               0,
                               0};
  uint8_t *srcBuffer = new uint8_t[size];
  uint8_t *dstBuffer = new uint8_t[size];
  for (size_t i = 0; i < size; ++i) {
    srcBuffer[i] = static_cast<uint8_t>(i);
    dstBuffer[i] = 0xff;
  }

  benchmark->imageCreate(&imageDesc, &this->image);

  // Copy from srcBuffer->Image->dstBuffer, so at the end dstBuffer = srcBuffer

  // commandListReset to make sure resetting the command_list_a from previous
  // operations on host2device
  benchmark->commandListReset(command_list_a);
  benchmark->commandListAppendImageCopyFromMemory(command_list_a, image,
                                                  srcBuffer, &Region);
  benchmark->commandListClose(command_list_a);

  // commandListReset to make sure resetting the command_list_b from previous
  // operations on host2device
  benchmark->commandListReset(command_list_b);
  for (int i = 0; i < number_iterations; i++) {
    benchmark->commandListAppendImageCopyToMemory(command_list_b, dstBuffer,
                                                  image, &Region);
  }
  benchmark->commandListClose(command_list_b);

  /* Warm up */
  benchmark->commandQueueExecuteCommandList(command_queue, 1, &command_list_a);
  benchmark->commandQueueSynchronize(command_queue);

  benchmark->commandQueueExecuteCommandList(command_queue, 1, &command_list_b);
  benchmark->commandQueueSynchronize(command_queue);

  // measure the bandwidth of copy from device to host only
  timer.start();
  SUCCESS_OR_TERMINATE(zeCommandQueueExecuteCommandLists(
      command_queue, 1, &command_list_b, nullptr));
  SUCCESS_OR_TERMINATE(zeCommandQueueSynchronize(command_queue, UINT32_MAX));
  timer.end();

  total_time_usec = timer.period_minus_overhead();
  total_time_s = total_time_usec / 1e6;

  total_data_transfer = (size * number_iterations) /
                        static_cast<long double>(1e9); /* Units in Gigabytes */

  gbps = total_data_transfer / total_time_s;
  std::cout << gbps << " GBPS\n";
  latency = total_time_usec / static_cast<long double>(number_iterations);
  std::cout << std::setprecision(11) << latency << " us"
            << " (Latency: Device->Host)" << std::endl;
  if (this->data_validation) {
    validRet = (0 == memcmp(srcBuffer, dstBuffer, size));
  }
  benchmark->imageDestroy(this->image);
  delete[] srcBuffer;
  delete[] dstBuffer;
}

XeImageCopyLatency::XeImageCopyLatency() {
  width = 1;
  height = 1;
  depth = 1;
  Imagelayout = ZE_IMAGE_FORMAT_LAYOUT_8;
  Imageformat = ZE_IMAGE_FORMAT_TYPE_UINT;
}

void measure_bandwidth_Host2Device2Host(XeImageCopy &Imagecopy,
                                        ptree *test_ptree) {

  if (Imagecopy.is_json_output_enabled()) {
    std::stringstream Image_dimensions;
    Image_dimensions << Imagecopy.width << "X" << Imagecopy.height << "X"
                     << Imagecopy.depth;
    test_ptree->put(
        "Name", "Host2Device2Host: Bandwidth copying from Host->Device->Host ");
    test_ptree->put("Image size", Image_dimensions.str());
  } else {
    std::cout << "Host2Device2Host: Measuring Bandwidth for copying the "
                 "image buffer size "
              << Imagecopy.width << "X" << Imagecopy.height << "X"
              << Imagecopy.depth << " from Host->Device->Host" << std::endl;
  }

  Imagecopy.measureHost2Device2Host();

  if (Imagecopy.is_json_output_enabled()) {
    test_ptree->put("GBPS", Imagecopy.gbps);
    if (Imagecopy.data_validation)
      test_ptree->put("Result", (Imagecopy.validRet ? "PASSED" : "FAILED"));
  } else {
    if (Imagecopy.data_validation) {
      std::cout << "  Results: Data validation "
                << (Imagecopy.validRet ? "PASSED" : "FAILED") << std::endl;
      std::cout << std::endl;
    }
  }
}

void measure_bandwidth_Host2Device(XeImageCopy &Imagecopy, ptree *test_ptree) {
  if (Imagecopy.is_json_output_enabled()) {
    std::stringstream Image_dimensions;
    Image_dimensions << Imagecopy.width << "X" << Imagecopy.height << "X"
                     << Imagecopy.depth;
    test_ptree->put("Name", "Host2Device: Bandwidth copying from Host->Device");
    test_ptree->put("Image size", Image_dimensions.str());
  } else {
    std::cout
        << "Host2Device: Measuring Bandwidth/Latency for copying the image "
           "buffer size "
        << Imagecopy.width << "X" << Imagecopy.height << "X" << Imagecopy.depth
        << " from Host->Device" << std::endl;
  }

  Imagecopy.measureHost2Device();

  if (Imagecopy.is_json_output_enabled()) {
    test_ptree->put("GBPS", Imagecopy.gbps);
    if (Imagecopy.data_validation) {
      test_ptree->put("Result", (Imagecopy.validRet ? "PASSED" : "FAILED"));
    }
  } else {
    if (Imagecopy.data_validation) {
      std::cout << "  Results: Data validation "
                << (Imagecopy.validRet ? "PASSED" : "FAILED") << std::endl;
      std::cout << std::endl;
    }
  }
}

void measure_bandwidth_Device2Host(XeImageCopy &Imagecopy, ptree *test_ptree) {
  if (Imagecopy.is_json_output_enabled()) {
    std::stringstream Image_dimensions;
    Image_dimensions << Imagecopy.width << "X" << Imagecopy.height << "X"
                     << Imagecopy.depth;
    test_ptree->put("Name", "Device2Host: Bandwidth copying from Device->Host");
    test_ptree->put("Image size", Image_dimensions.str());
  } else {
    std::cout
        << "Device2Host: Measurign Bandwidth/Latency for copying the image "
           "buffer size "
        << Imagecopy.width << "X" << Imagecopy.height << "X" << Imagecopy.depth
        << " from Device->Host" << std::endl;
  }

  Imagecopy.measureDevice2Host();

  if (Imagecopy.is_json_output_enabled()) {
    test_ptree->put("GBPS", Imagecopy.gbps);
    if (Imagecopy.data_validation)
      test_ptree->put("Result", (Imagecopy.validRet ? "PASSED" : "FAILED"));
  } else {
    if (Imagecopy.data_validation) {
      std::cout << "  Results: Data validation "
                << (Imagecopy.validRet ? "PASSED" : "FAILED") << std::endl;
      std::cout << std::endl;
    }
  }
}

void measure_bandwidth(XeImageCopy &Imagecopy) {
  ptree ptree_Host2Device2Host;
  ptree ptree_Host2Device;
  ptree ptree_Device2Host;
  ptree ptree_main;

  measure_bandwidth_Host2Device2Host(Imagecopy, &ptree_Host2Device2Host);
  measure_bandwidth_Host2Device(Imagecopy, &ptree_Host2Device);
  measure_bandwidth_Device2Host(Imagecopy, &ptree_Device2Host);

  if (Imagecopy.is_json_output_enabled()) {
    Imagecopy.param_array.push_back(std::make_pair("", ptree_Host2Device2Host));
    Imagecopy.param_array.push_back(std::make_pair("", ptree_Host2Device));
    Imagecopy.param_array.push_back(std::make_pair("", ptree_Device2Host));
    ptree_main.put_child("Performance Benchmark.bandwidth",
                         Imagecopy.param_array);
    pt::write_json(Imagecopy.JsonFileName.c_str(), ptree_main);
  }
}

void measure_latency_Host2Device(XeImageCopyLatency &imageCopyLatency,
                                 ptree *test_ptree) {
  if (imageCopyLatency.is_json_output_enabled()) {
    std::stringstream Image_dimensions;
    Image_dimensions << imageCopyLatency.width << "X" << imageCopyLatency.height
                     << "X" << imageCopyLatency.depth;
    test_ptree->put("Name",
                    "Host2Device: Measuring Latency for copying the image ");
    test_ptree->put("Image size", Image_dimensions.str());
    test_ptree->put("Image format", imageCopyLatency.Imageformat);
    test_ptree->put("Image Layout", imageCopyLatency.Imagelayout);
  } else {
    std::cout
        << "Host2Device: Measuring Bandwidth/Latency for copying the image "
           "buffer size "
        << imageCopyLatency.width << "X" << imageCopyLatency.height << "X"
        << imageCopyLatency.depth << "   Image format=  "
        << level_zero_tests::to_string(imageCopyLatency.Imageformat)
        << "  Image Layout=  "
        << level_zero_tests::to_string(imageCopyLatency.Imagelayout)
        << "  from Host->Device" << std::endl;
  }

  imageCopyLatency.measureHost2Device();

  if (imageCopyLatency.is_json_output_enabled()) {
    test_ptree->put("Latency", imageCopyLatency.latency);
    if (imageCopyLatency.data_validation)
      test_ptree->put("Result",
                      (imageCopyLatency.validRet ? "PASSED" : "FAILED"));
  }
}

void measure_latency_Device2Host(XeImageCopyLatency &imageCopyLatency,
                                 ptree *test_ptree) {
  if (imageCopyLatency.is_json_output_enabled()) {
    std::stringstream Image_dimensions;
    Image_dimensions << imageCopyLatency.width << "X" << imageCopyLatency.height
                     << "X" << imageCopyLatency.depth;
    test_ptree->put("Name",
                    "Device2Host: Measuring Latency for copying the image");
    test_ptree->put("Image size", Image_dimensions.str());
    test_ptree->put("Image format", imageCopyLatency.Imageformat);
    test_ptree->put("Image Layout", imageCopyLatency.Imagelayout);
  } else {
    std::cout
        << "Device2Host: Measuring Bandwidth/Latency for copying the image "
           "buffer size "
        << imageCopyLatency.width << "X" << imageCopyLatency.height << "X"
        << imageCopyLatency.depth << "   Image format=  "
        << level_zero_tests::to_string(imageCopyLatency.Imageformat)
        << "  Image Layout=  "
        << level_zero_tests::to_string(imageCopyLatency.Imagelayout)
        << "  from Device->Host" << std::endl;
  }

  imageCopyLatency.measureDevice2Host();

  if (imageCopyLatency.is_json_output_enabled()) {
    test_ptree->put("Latency", imageCopyLatency.latency);
    if (imageCopyLatency.data_validation)
      test_ptree->put("Result",
                      (imageCopyLatency.validRet ? "PASSED" : "FAILED"));
  }
}

// Measuring latency for 1x1x1 image from Dev->Host & Host->Dev
void measure_latency(XeImageCopyLatency &imageCopyLatency) {
  ptree ptree_Host2Device;
  ptree ptree_Device2Host;
  ptree ptree_main;
  ptree ptree_param_array;

  if (imageCopyLatency.is_json_output_enabled()) {
    pt::read_json(imageCopyLatency.JsonFileName, ptree_main);
  }
  measure_latency_Host2Device(imageCopyLatency, &ptree_Host2Device);
  measure_latency_Device2Host(imageCopyLatency, &ptree_Device2Host);

  if (imageCopyLatency.is_json_output_enabled()) {
    ptree_param_array.push_back(std::make_pair("", ptree_Host2Device));
    ptree_param_array.push_back(std::make_pair("", ptree_Device2Host));
    ptree_main.put_child("Performance Benchmark.latency", ptree_param_array);
    pt::write_json(imageCopyLatency.JsonFileName.c_str(), ptree_main);
  }
}

int main(int argc, char **argv) {
  XeImageCopy Imagecopy;
  SUCCESS_OR_TERMINATE(Imagecopy.parse_command_line(argc, argv));
  measure_bandwidth(Imagecopy);
  XeImageCopyLatency imageCopyLatency;
  imageCopyLatency.JsonFileName =
      Imagecopy.JsonFileName; // need to add latency values to the same file
  measure_latency(imageCopyLatency);
  return 0;
}
