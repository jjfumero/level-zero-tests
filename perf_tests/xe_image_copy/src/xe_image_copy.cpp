/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2019 Intel Corporation. All Rights Reserved.
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

  /* For single device, create command Queue, command list and image with xe_app
   */

  benchmark->commandQueueCreate(ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS,
                                &this->command_queue);
  benchmark->commandListCreate(&this->command_list);
}

XeImageCopy ::~XeImageCopy() {

  benchmark->commandQueueDestroy(this->command_queue);
  benchmark->commandListDestroy(this->command_list);

  delete benchmark;
}

void XeImageCopy::measureHost2Device(bool &validRet) {

  const size_t size = 4 * width * height * depth; /* 4 channels per pixel */
  long double gbps;

  Timer<std::chrono::microseconds::period> timer;
  long double total_time_usec;
  long double total_time_s;
  long double total_data_transfer;

  ze_result_t result = ZE_RESULT_SUCCESS;
  ze_image_region_t Region = {xOffset, yOffset, zOffset, width, height, depth};

  uint8_t *srcBuffer = new uint8_t[size];
  uint8_t *dstBuffer = new uint8_t[size];
  for (size_t i = 0; i < size; ++i) {
    srcBuffer[i] = static_cast<uint8_t>(i);
    dstBuffer[i] = 0xff;
  }

  // create image with given width,height & depth
  benchmark->imageCreate(&this->image, width, height, depth);

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

  timer.start();
  for (int i = 0; i < number_iterations; i++) {

    SUCCESS_OR_TERMINATE(zeCommandQueueExecuteCommandLists(
        command_queue, 1, &command_list, nullptr));
    SUCCESS_OR_TERMINATE(zeCommandQueueSynchronize(command_queue, UINT32_MAX));
  }
  timer.end();

  total_time_usec = timer.period_minus_overhead();
  total_time_s = total_time_usec / 1e6;

  total_data_transfer = (size * number_iterations) /
                        static_cast<long double>(1e9); /* Units in Gigabytes */

  gbps = total_data_transfer / total_time_s;
  std::cout << gbps << " GBPS\n";

  validRet = (0 == memcmp(srcBuffer, dstBuffer, size));

  benchmark->imageDestroy(this->image);
  delete[] srcBuffer;
  delete[] dstBuffer;
}

int main(int argc, char **argv) {
  XeImageCopy Imagecopy;
  bool outputValidationSuccessful = false;

  Imagecopy.parse_arguments(argc, argv);

  std::cout << "Bandwidth copy the image buffer size " << Imagecopy.width << "X"
            << Imagecopy.height << "X" << Imagecopy.depth
            << " from host to device" << std::endl;

  Imagecopy.measureHost2Device(outputValidationSuccessful);
  std::cout << std::endl;

  std::cout << "\nZello Copy Image measure Bandwidth  " << Imagecopy.width
            << "X" << Imagecopy.height << "X" << Imagecopy.depth
            << "  Results validation "
            << (outputValidationSuccessful ? "PASSED" : "FAILED") << std::endl;

  return 0;
}
