/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2019 Intel Corporation. All Rights Reserved.
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

#ifndef XE_IMAGE_COPY_H
#define XE_IMAGE_COPY_H

#include "common.hpp"
#include "ze_api.h"
#include "ze_cmdqueue.h"
#include "xe_app.hpp"

#include <assert.h>
#include <iomanip>
#include <iostream>
#include <boost/program_options.hpp>
#include "utils/utils.hpp"

namespace po = boost::program_options;
using namespace std;



class XeImageCopy {
public:
	uint32_t width = 2048;
	uint32_t height = 2048;
	uint32_t depth  = 1;
	uint32_t xOffset = 0;
	uint32_t yOffset = 0;
	uint32_t zOffset = 0;
    uint32_t number_iterations = 50;
    uint32_t warm_up_iterations = 10;
	ze_image_format_layout_t Imagelayout = ZE_IMAGE_FORMAT_LAYOUT_32;
	ze_image_flag_t Imageflags = ZE_IMAGE_FLAG_PROGRAM_READ;
	ze_image_type_t Imagetype = ZE_IMAGE_TYPE_2D;
	ze_image_format_type_t  Imageformat = ZE_IMAGE_FORMAT_TYPE_FLOAT;
  XeImageCopy();
  ~XeImageCopy();
  void measureHost2Device2Host(bool &validRet);
  void measureHost2Device(bool &validRet);
  void measureDevice2Host(bool &validRet);
  int parse_command_line(int argc, char **argv);
 

private:
  XeApp *benchmark;
  ze_command_queue_handle_t command_queue;
  ze_command_list_handle_t command_list;
  ze_command_list_handle_t command_list_a;
  ze_command_list_handle_t command_list_b;
  ze_image_handle_t image;
};

class XeImageCopyLatency : public XeImageCopy {
public:
  XeImageCopyLatency();
};

#endif  /* XE_IMAGE_COPY_H */
