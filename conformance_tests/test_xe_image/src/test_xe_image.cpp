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
#include "xe_test_harness/xe_test_harness.hpp"
#include "logging/logging.hpp"

namespace lzt = level_zero_tests;

namespace {

class xeImageCreateDefaultTests : public ::testing::Test {};

class xeImage1DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t,
                     xe_image_format_swizzle_t>> {};

class xeImageArray1DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t,
                     xe_image_format_swizzle_t, size_t>> {};

class xeImage2DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t,
                     xe_image_format_swizzle_t, xe_image_format_swizzle_t>> {};

class xeImageArray2DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<std::tuple<
          xe_image_format_layout_t, xe_image_format_type_t,
          xe_image_format_swizzle_t, xe_image_format_swizzle_t, size_t>> {};

class xeImage3DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t,
                     xe_image_format_swizzle_t, xe_image_format_swizzle_t,
                     xe_image_format_swizzle_t>> {};

class xeImageArray3DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t,
                     xe_image_format_swizzle_t, xe_image_format_swizzle_t,
                     xe_image_format_swizzle_t, size_t>> {};

class xeImage4DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t,
                     xe_image_format_swizzle_t, xe_image_format_swizzle_t,
                     xe_image_format_swizzle_t, xe_image_format_swizzle_t>> {};

class xeImageArray4DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<std::tuple<
          xe_image_format_layout_t, xe_image_format_type_t,
          xe_image_format_swizzle_t, xe_image_format_swizzle_t,
          xe_image_format_swizzle_t, xe_image_format_swizzle_t, size_t>> {};

class xeImageMediaCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};

class xeImageArrayMediaCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<std::tuple<
          xe_image_format_layout_t, xe_image_format_type_t, size_t>> {};

TEST_F(
    xeImageCreateDefaultTests,
    GivenDefaultImageDescriptorWhenCreatingImageThenNotNullPointerIsReturned) {
  xe_image_handle_t image;
  lzt::create_xe_image(image);
  lzt::destroy_xe_image(image);
}

TEST_P(
    xeImage1DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DImageWith1DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {

      xe_image_handle_t image;
      xe_image_format_desc_t format_descriptor = {
          std::get<0>(GetParam()),   // layout
          std::get<1>(GetParam()),   // type
          std::get<2>(GetParam()),   // x
          XE_IMAGE_FORMAT_SWIZZLE_X, // y
          XE_IMAGE_FORMAT_SWIZZLE_X, // z
          XE_IMAGE_FORMAT_SWIZZLE_X  // w
      };

      xe_image_desc_t image_descriptor = {
          XE_IMAGE_DESC_VERSION_CURRENT, // version
          image_create_flags,            // flags
          XE_IMAGE_TYPE_1D,              // type
          format_descriptor,             // format
          image_width,                   // width
          0,                             // height
          0,                             // depth
          0,                             // arraylevels
          0};                            // miplevels

      lzt::print_image_descriptor(image_descriptor);
      lzt::create_xe_image(image, &image_descriptor);
      lzt::destroy_xe_image(image);
    }
  }
}

TEST_P(
    xeImage1DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DImageWith1DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {

        xe_image_handle_t image;
        xe_image_format_desc_t format_descriptor = {
            std::get<0>(GetParam()),   // layout
            std::get<1>(GetParam()),   // type
            std::get<2>(GetParam()),   // x
            XE_IMAGE_FORMAT_SWIZZLE_X, // y
            XE_IMAGE_FORMAT_SWIZZLE_X, // z
            XE_IMAGE_FORMAT_SWIZZLE_X  // w
        };

        xe_image_desc_t image_descriptor = {
            XE_IMAGE_DESC_VERSION_CURRENT, // version
            image_create_flags,            // flags
            XE_IMAGE_TYPE_2D,              // type
            format_descriptor,             // format
            image_width,                   // width
            image_height,                  // height
            0,                             // depth
            0,                             // arraylevels
            0};                            // miplevels
        lzt::print_image_descriptor(image_descriptor);
        lzt::create_xe_image(image, &image_descriptor);
        lzt::destroy_xe_image(image);
      }
    }
  }
}

TEST_P(
    xeImage1DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating3DImageWith1DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto image_depth : lzt::image_depths) {

          xe_image_handle_t image;
          xe_image_format_desc_t format_descriptor = {
              std::get<0>(GetParam()),   // layout
              std::get<1>(GetParam()),   // type
              std::get<2>(GetParam()),   // x
              XE_IMAGE_FORMAT_SWIZZLE_X, // y
              XE_IMAGE_FORMAT_SWIZZLE_X, // z
              XE_IMAGE_FORMAT_SWIZZLE_X  // w
          };

          xe_image_desc_t image_descriptor = {
              XE_IMAGE_DESC_VERSION_CURRENT, // version
              image_create_flags,            // flags
              XE_IMAGE_TYPE_3D,              // type
              format_descriptor,             // format
              image_width,                   // width
              image_height,                  // height
              image_depth,                   // depth
              0,                             // arraylevels
              0};                            // miplevels

          lzt::print_image_descriptor(image_descriptor);
          lzt::create_xe_image(image, &image_descriptor);
          lzt::destroy_xe_image(image);
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(1DSwizzleImageCreationCombinations,
                        xeImage1DSwizzleCreateTests,
                        ::testing::Combine(lzt::image_format_1d_swizzle_layouts,
                                           lzt::image_format_types,
                                           lzt::image_format_swizzles));

TEST_P(
    xeImageArray1DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DArrayImageWith1DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {

      xe_image_handle_t image;
      xe_image_format_desc_t format_descriptor = {
          std::get<0>(GetParam()),   // layout
          std::get<1>(GetParam()),   // type
          std::get<2>(GetParam()),   // x
          XE_IMAGE_FORMAT_SWIZZLE_X, // y
          XE_IMAGE_FORMAT_SWIZZLE_X, // z
          XE_IMAGE_FORMAT_SWIZZLE_X  // w
      };

      xe_image_desc_t image_descriptor = {
          XE_IMAGE_DESC_VERSION_CURRENT, // version
          image_create_flags,            // flags
          XE_IMAGE_TYPE_1DARRAY,         // type
          format_descriptor,             // format
          image_width,                   // width
          0,                             // height
          0,                             // depth
          std::get<3>(GetParam()),       // arraylevels
          0};                            // miplevels
      lzt::print_image_descriptor(image_descriptor);
      lzt::create_xe_image(image, &image_descriptor);
      lzt::destroy_xe_image(image);
    }
  }
}

TEST_P(
    xeImageArray1DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DArrayImageWith1DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {

        xe_image_handle_t image;
        xe_image_format_desc_t format_descriptor = {
            std::get<0>(GetParam()),   // layout
            std::get<1>(GetParam()),   // type
            std::get<2>(GetParam()),   // x
            XE_IMAGE_FORMAT_SWIZZLE_X, // y
            XE_IMAGE_FORMAT_SWIZZLE_X, // z
            XE_IMAGE_FORMAT_SWIZZLE_X  // w
        };

        xe_image_desc_t image_descriptor = {
            XE_IMAGE_DESC_VERSION_CURRENT, // version
            image_create_flags,            // flags
            XE_IMAGE_TYPE_2DARRAY,         // type
            format_descriptor,             // format
            image_width,                   // width
            image_height,                  // height
            0,                             // depth
            std::get<3>(GetParam()),       // arraylevels
            0};                            // miplevels
        lzt::print_image_descriptor(image_descriptor);
        lzt::create_xe_image(image, &image_descriptor);
        lzt::destroy_xe_image(image);
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(1DSwizzleArrayImageCreationCombinations,
                        xeImageArray1DSwizzleCreateTests,
                        ::testing::Combine(lzt::image_format_1d_swizzle_layouts,
                                           lzt::image_format_types,
                                           lzt::image_format_swizzles,
                                           lzt::image_array_levels));

TEST_P(
    xeImage2DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DImageWith2DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {

      xe_image_handle_t image;
      xe_image_format_desc_t format_descriptor = {
          std::get<0>(GetParam()),   // layout
          std::get<1>(GetParam()),   // type
          std::get<2>(GetParam()),   // x
          std::get<3>(GetParam()),   // y
          XE_IMAGE_FORMAT_SWIZZLE_X, // z
          XE_IMAGE_FORMAT_SWIZZLE_X  // w
      };

      xe_image_desc_t image_descriptor = {
          XE_IMAGE_DESC_VERSION_CURRENT, // version
          image_create_flags,            // flags
          XE_IMAGE_TYPE_1D,              // type
          format_descriptor,             // format
          image_width,                   // width
          0,                             // height
          0,                             // depth
          0,                             // arraylevels
          0};                            // miplevels

      lzt::print_image_descriptor(image_descriptor);
      lzt::create_xe_image(image, &image_descriptor);
      lzt::destroy_xe_image(image);
    }
  }
}

TEST_P(
    xeImage2DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DImageWith2DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {

        xe_image_handle_t image;
        xe_image_format_desc_t format_descriptor = {
            std::get<0>(GetParam()),   // layout
            std::get<1>(GetParam()),   // type
            std::get<2>(GetParam()),   // x
            std::get<3>(GetParam()),   // y
            XE_IMAGE_FORMAT_SWIZZLE_X, // z
            XE_IMAGE_FORMAT_SWIZZLE_X  // w
        };

        xe_image_desc_t image_descriptor = {
            XE_IMAGE_DESC_VERSION_CURRENT, // version
            image_create_flags,            // flags
            XE_IMAGE_TYPE_2D,              // type
            format_descriptor,             // format
            image_width,                   // width
            image_height,                  // height
            0,                             // depth
            0,                             // arraylevels
            0};                            // miplevels
        lzt::print_image_descriptor(image_descriptor);
        lzt::create_xe_image(image, &image_descriptor);
        lzt::destroy_xe_image(image);
      }
    }
  }
}

TEST_P(
    xeImage2DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating3DImageWith2DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto image_depth : lzt::image_depths) {

          xe_image_handle_t image;
          xe_image_format_desc_t format_descriptor = {
              std::get<0>(GetParam()),   // layout
              std::get<1>(GetParam()),   // type
              std::get<2>(GetParam()),   // x
              std::get<3>(GetParam()),   // y
              XE_IMAGE_FORMAT_SWIZZLE_X, // z
              XE_IMAGE_FORMAT_SWIZZLE_X  // w
          };

          xe_image_desc_t image_descriptor = {
              XE_IMAGE_DESC_VERSION_CURRENT, // version
              image_create_flags,            // flags
              XE_IMAGE_TYPE_3D,              // type
              format_descriptor,             // format
              image_width,                   // width
              image_height,                  // height
              image_depth,                   // depth
              0,                             // arraylevels
              0};                            // miplevels

          lzt::print_image_descriptor(image_descriptor);
          lzt::create_xe_image(image, &image_descriptor);
          lzt::destroy_xe_image(image);
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(2DSwizzleImageCreationCombinations,
                        xeImage2DSwizzleCreateTests,
                        ::testing::Combine(lzt::image_format_2d_swizzle_layouts,
                                           lzt::image_format_types,
                                           lzt::image_format_swizzles,
                                           lzt::image_format_swizzles));

TEST_P(
    xeImageArray2DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DArrayImageWith2DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {

      xe_image_handle_t image;
      xe_image_format_desc_t format_descriptor = {
          std::get<0>(GetParam()),   // layout
          std::get<1>(GetParam()),   // type
          std::get<2>(GetParam()),   // x
          std::get<3>(GetParam()),   // y
          XE_IMAGE_FORMAT_SWIZZLE_X, // z
          XE_IMAGE_FORMAT_SWIZZLE_X  // w
      };

      xe_image_desc_t image_descriptor = {
          XE_IMAGE_DESC_VERSION_CURRENT, // version
          image_create_flags,            // flags
          XE_IMAGE_TYPE_1DARRAY,         // type
          format_descriptor,             // format
          image_width,                   // width
          0,                             // height
          0,                             // depth
          std::get<4>(GetParam()),       // arraylevels
          0};                            // miplevels
      lzt::print_image_descriptor(image_descriptor);
      lzt::create_xe_image(image, &image_descriptor);
      lzt::destroy_xe_image(image);
    }
  }
}

TEST_P(
    xeImageArray2DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DArrayImageWith2DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {

        xe_image_handle_t image;
        xe_image_format_desc_t format_descriptor = {
            std::get<0>(GetParam()),   // layout
            std::get<1>(GetParam()),   // type
            std::get<2>(GetParam()),   // x
            std::get<3>(GetParam()),   // y
            XE_IMAGE_FORMAT_SWIZZLE_X, // z
            XE_IMAGE_FORMAT_SWIZZLE_X  // w
        };

        xe_image_desc_t image_descriptor = {
            XE_IMAGE_DESC_VERSION_CURRENT, // version
            image_create_flags,            // flags
            XE_IMAGE_TYPE_2DARRAY,         // type
            format_descriptor,             // format
            image_width,                   // width
            image_height,                  // height
            0,                             // depth
            std::get<4>(GetParam()),       // arraylevels
            0};                            // miplevels
        lzt::print_image_descriptor(image_descriptor);
        lzt::create_xe_image(image, &image_descriptor);
        lzt::destroy_xe_image(image);
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(
    2DSwizzleArrayImageCreationCombinations, xeImageArray2DSwizzleCreateTests,
    ::testing::Combine(lzt::image_format_2d_swizzle_layouts,
                       lzt::image_format_types, lzt::image_format_swizzles,
                       lzt::image_format_swizzles, lzt::image_array_levels));

TEST_P(
    xeImage3DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DImageWith3DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {

      xe_image_handle_t image;
      xe_image_format_desc_t format_descriptor = {
          std::get<0>(GetParam()),  // layout
          std::get<1>(GetParam()),  // type
          std::get<2>(GetParam()),  // x
          std::get<3>(GetParam()),  // y
          std::get<4>(GetParam()),  // z
          XE_IMAGE_FORMAT_SWIZZLE_X // w
      };

      xe_image_desc_t image_descriptor = {
          XE_IMAGE_DESC_VERSION_CURRENT, // version
          image_create_flags,            // flags
          XE_IMAGE_TYPE_1D,              // type
          format_descriptor,             // format
          image_width,                   // width
          0,                             // height
          0,                             // depth
          0,                             // arraylevels
          0};                            // miplevels

      lzt::print_image_descriptor(image_descriptor);
      lzt::create_xe_image(image, &image_descriptor);
      lzt::destroy_xe_image(image);
    }
  }
}

TEST_P(
    xeImage3DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DImageWith3DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {

        xe_image_handle_t image;
        xe_image_format_desc_t format_descriptor = {
            std::get<0>(GetParam()),  // layout
            std::get<1>(GetParam()),  // type
            std::get<2>(GetParam()),  // x
            std::get<3>(GetParam()),  // y
            std::get<4>(GetParam()),  // z
            XE_IMAGE_FORMAT_SWIZZLE_X // w
        };

        xe_image_desc_t image_descriptor = {
            XE_IMAGE_DESC_VERSION_CURRENT, // version
            image_create_flags,            // flags
            XE_IMAGE_TYPE_2D,              // type
            format_descriptor,             // format
            image_width,                   // width
            image_height,                  // height
            0,                             // depth
            0,                             // arraylevels
            0};                            // miplevels
        lzt::print_image_descriptor(image_descriptor);
        lzt::create_xe_image(image, &image_descriptor);
        lzt::destroy_xe_image(image);
      }
    }
  }
}

TEST_P(
    xeImage3DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating3DImageWith3DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto image_depth : lzt::image_depths) {

          xe_image_handle_t image;
          xe_image_format_desc_t format_descriptor = {
              std::get<0>(GetParam()),  // layout
              std::get<1>(GetParam()),  // type
              std::get<2>(GetParam()),  // x
              std::get<3>(GetParam()),  // y
              std::get<4>(GetParam()),  // z
              XE_IMAGE_FORMAT_SWIZZLE_X // w
          };

          xe_image_desc_t image_descriptor = {
              XE_IMAGE_DESC_VERSION_CURRENT, // version
              image_create_flags,            // flags
              XE_IMAGE_TYPE_3D,              // type
              format_descriptor,             // format
              image_width,                   // width
              image_height,                  // height
              image_depth,                   // depth
              0,                             // arraylevels
              0};                            // miplevels

          lzt::print_image_descriptor(image_descriptor);
          lzt::create_xe_image(image, &image_descriptor);
          lzt::destroy_xe_image(image);
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(
    3DSwizzleImageCreationCombinations, xeImage3DSwizzleCreateTests,
    ::testing::Combine(lzt::image_format_3d_swizzle_layouts,
                       lzt::image_format_types, lzt::image_format_swizzles,
                       lzt::image_format_swizzles, lzt::image_format_swizzles));

TEST_P(
    xeImageArray3DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DArrayImageWith3DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {

      xe_image_handle_t image;
      xe_image_format_desc_t format_descriptor = {
          std::get<0>(GetParam()),  // layout
          std::get<1>(GetParam()),  // type
          std::get<2>(GetParam()),  // x
          std::get<3>(GetParam()),  // y
          std::get<4>(GetParam()),  // z
          XE_IMAGE_FORMAT_SWIZZLE_X // w
      };

      xe_image_desc_t image_descriptor = {
          XE_IMAGE_DESC_VERSION_CURRENT, // version
          image_create_flags,            // flags
          XE_IMAGE_TYPE_1DARRAY,         // type
          format_descriptor,             // format
          image_width,                   // width
          0,                             // height
          0,                             // depth
          std::get<5>(GetParam()),       // arraylevels
          0};                            // miplevels
      lzt::print_image_descriptor(image_descriptor);
      lzt::create_xe_image(image, &image_descriptor);
      lzt::destroy_xe_image(image);
    }
  }
}

TEST_P(
    xeImageArray3DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DArrayImageWith3DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {

        xe_image_handle_t image;
        xe_image_format_desc_t format_descriptor = {
            std::get<0>(GetParam()),  // layout
            std::get<1>(GetParam()),  // type
            std::get<2>(GetParam()),  // x
            std::get<3>(GetParam()),  // y
            std::get<4>(GetParam()),  // z
            XE_IMAGE_FORMAT_SWIZZLE_X // w
        };

        xe_image_desc_t image_descriptor = {
            XE_IMAGE_DESC_VERSION_CURRENT, // version
            image_create_flags,            // flags
            XE_IMAGE_TYPE_2DARRAY,         // type
            format_descriptor,             // format
            image_width,                   // width
            image_height,                  // height
            0,                             // depth
            std::get<5>(GetParam()),       // arraylevels
            0};                            // miplevels
        lzt::print_image_descriptor(image_descriptor);
        lzt::create_xe_image(image, &image_descriptor);
        lzt::destroy_xe_image(image);
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(
    3DSwizzleArrayImageCreationCombinations, xeImageArray3DSwizzleCreateTests,
    ::testing::Combine(lzt::image_format_3d_swizzle_layouts,
                       lzt::image_format_types, lzt::image_format_swizzles,
                       lzt::image_format_swizzles, lzt::image_format_swizzles,
                       lzt::image_array_levels));

TEST_P(
    xeImage4DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DImageWith4DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {

      xe_image_handle_t image;
      xe_image_format_desc_t format_descriptor = {
          std::get<0>(GetParam()), // layout
          std::get<1>(GetParam()), // type
          std::get<2>(GetParam()), // x
          std::get<3>(GetParam()), // y
          std::get<4>(GetParam()), // z
          std::get<5>(GetParam())  // w
      };

      xe_image_desc_t image_descriptor = {
          XE_IMAGE_DESC_VERSION_CURRENT, // version
          image_create_flags,            // flags
          XE_IMAGE_TYPE_1D,              // type
          format_descriptor,             // format
          image_width,                   // width
          0,                             // height
          0,                             // depth
          0,                             // arraylevels
          0};                            // miplevels

      lzt::print_image_descriptor(image_descriptor);
      lzt::create_xe_image(image, &image_descriptor);
      lzt::destroy_xe_image(image);
    }
  }
}

TEST_P(
    xeImage4DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DImageWith4DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {

        xe_image_handle_t image;
        xe_image_format_desc_t format_descriptor = {
            std::get<0>(GetParam()), // layout
            std::get<1>(GetParam()), // type
            std::get<2>(GetParam()), // x
            std::get<3>(GetParam()), // y
            std::get<4>(GetParam()), // z
            std::get<5>(GetParam())  // w
        };

        xe_image_desc_t image_descriptor = {
            XE_IMAGE_DESC_VERSION_CURRENT, // version
            image_create_flags,            // flags
            XE_IMAGE_TYPE_2D,              // type
            format_descriptor,             // format
            image_width,                   // width
            image_height,                  // height
            0,                             // depth
            0,                             // arraylevels
            0};                            // miplevels
        lzt::print_image_descriptor(image_descriptor);
        lzt::create_xe_image(image, &image_descriptor);
        lzt::destroy_xe_image(image);
      }
    }
  }
}

TEST_P(
    xeImage4DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating3DImageWith4DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto image_depth : lzt::image_depths) {

          xe_image_handle_t image;
          xe_image_format_desc_t format_descriptor = {
              std::get<0>(GetParam()), // layout
              std::get<1>(GetParam()), // type
              std::get<2>(GetParam()), // x
              std::get<3>(GetParam()), // y
              std::get<4>(GetParam()), // z
              std::get<5>(GetParam())  // w
          };

          xe_image_desc_t image_descriptor = {
              XE_IMAGE_DESC_VERSION_CURRENT, // version
              image_create_flags,            // flags
              XE_IMAGE_TYPE_3D,              // type
              format_descriptor,             // format
              image_width,                   // width
              image_height,                  // height
              image_depth,                   // depth
              0,                             // arraylevels
              0};                            // miplevels

          lzt::print_image_descriptor(image_descriptor);
          lzt::create_xe_image(image, &image_descriptor);
          lzt::destroy_xe_image(image);
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(
    4DSwizzleImageCreationCombinations, xeImage4DSwizzleCreateTests,
    ::testing::Combine(lzt::image_format_4d_swizzle_layouts,
                       lzt::image_format_types, lzt::image_format_swizzles,
                       lzt::image_format_swizzles, lzt::image_format_swizzles,
                       lzt::image_format_swizzles));

TEST_P(
    xeImageArray4DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DArrayImageWith4DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {

      xe_image_handle_t image;
      xe_image_format_desc_t format_descriptor = {
          std::get<0>(GetParam()), // layout
          std::get<1>(GetParam()), // type
          std::get<2>(GetParam()), // x
          std::get<3>(GetParam()), // y
          std::get<4>(GetParam()), // z
          std::get<5>(GetParam())  // w
      };

      xe_image_desc_t image_descriptor = {
          XE_IMAGE_DESC_VERSION_CURRENT, // version
          image_create_flags,            // flags
          XE_IMAGE_TYPE_1DARRAY,         // type
          format_descriptor,             // format
          image_width,                   // width
          0,                             // height
          0,                             // depth
          std::get<6>(GetParam()),       // arraylevels
          0};                            // miplevels
      lzt::print_image_descriptor(image_descriptor);
      lzt::create_xe_image(image, &image_descriptor);
      lzt::destroy_xe_image(image);
    }
  }
}

TEST_P(
    xeImageArray4DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DArrayImageWith4DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {

        xe_image_handle_t image;
        xe_image_format_desc_t format_descriptor = {
            std::get<0>(GetParam()), // layout
            std::get<1>(GetParam()), // type
            std::get<2>(GetParam()), // x
            std::get<3>(GetParam()), // y
            std::get<4>(GetParam()), // z
            std::get<5>(GetParam())  // w
        };

        xe_image_desc_t image_descriptor = {
            XE_IMAGE_DESC_VERSION_CURRENT, // version
            image_create_flags,            // flags
            XE_IMAGE_TYPE_2DARRAY,         // type
            format_descriptor,             // format
            image_width,                   // width
            image_height,                  // height
            0,                             // depth
            std::get<6>(GetParam()),       // arraylevels
            0};                            // miplevels
        lzt::print_image_descriptor(image_descriptor);
        lzt::create_xe_image(image, &image_descriptor);
        lzt::destroy_xe_image(image);
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(
    4DSwizzleArrayImageCreationCombinations, xeImageArray4DSwizzleCreateTests,
    ::testing::Combine(lzt::image_format_4d_swizzle_layouts,
                       lzt::image_format_types, lzt::image_format_swizzles,
                       lzt::image_format_swizzles, lzt::image_format_swizzles,
                       lzt::image_format_swizzles, lzt::image_array_levels));

TEST_P(
    xeImageMediaCreateTests,
    GivenValidDescriptorWhenCreating1DImageWithMediaFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {

      xe_image_handle_t image;
      xe_image_format_desc_t format_descriptor = {
          std::get<0>(GetParam()),   // layout
          std::get<1>(GetParam()),   // type
          XE_IMAGE_FORMAT_SWIZZLE_X, // x
          XE_IMAGE_FORMAT_SWIZZLE_X, // y
          XE_IMAGE_FORMAT_SWIZZLE_X, // z
          XE_IMAGE_FORMAT_SWIZZLE_X  // w
      };

      xe_image_desc_t image_descriptor = {
          XE_IMAGE_DESC_VERSION_CURRENT, // version
          image_create_flags,            // flags
          XE_IMAGE_TYPE_1D,              // type
          format_descriptor,             // format
          image_width,                   // width
          0,                             // height
          0,                             // depth
          0,                             // arraylevels
          0};                            // miplevels
      lzt::print_image_descriptor(image_descriptor);
      lzt::create_xe_image(image, &image_descriptor);
      lzt::destroy_xe_image(image);
    }
  }
}

TEST_P(
    xeImageMediaCreateTests,
    GivenValidDescriptorWhenCreating2DImageWithMediaFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {

        xe_image_handle_t image;
        xe_image_format_desc_t format_descriptor = {
            std::get<0>(GetParam()),   // layout
            std::get<1>(GetParam()),   // type
            XE_IMAGE_FORMAT_SWIZZLE_X, // x
            XE_IMAGE_FORMAT_SWIZZLE_X, // y
            XE_IMAGE_FORMAT_SWIZZLE_X, // z
            XE_IMAGE_FORMAT_SWIZZLE_X  // w
        };

        xe_image_desc_t image_descriptor = {
            XE_IMAGE_DESC_VERSION_CURRENT, // version
            image_create_flags,            // flags
            XE_IMAGE_TYPE_2D,              // type
            format_descriptor,             // format
            image_width,                   // width
            image_height,                  // height
            0,                             // depth
            0,                             // arraylevels
            0};                            // miplevels
        lzt::print_image_descriptor(image_descriptor);
        lzt::create_xe_image(image, &image_descriptor);
        lzt::destroy_xe_image(image);
      }
    }
  }
}

TEST_P(
    xeImageMediaCreateTests,
    GivenValidDescriptorWhenCreating3DImageWithMediaFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto image_depth : lzt::image_depths) {

          xe_image_handle_t image;
          xe_image_format_desc_t format_descriptor = {
              std::get<0>(GetParam()),   // layout
              std::get<1>(GetParam()),   // type
              XE_IMAGE_FORMAT_SWIZZLE_X, // x
              XE_IMAGE_FORMAT_SWIZZLE_X, // y
              XE_IMAGE_FORMAT_SWIZZLE_X, // z
              XE_IMAGE_FORMAT_SWIZZLE_X  // w
          };

          xe_image_desc_t image_descriptor = {
              XE_IMAGE_DESC_VERSION_CURRENT, // version
              image_create_flags,            // flags
              XE_IMAGE_TYPE_3D,              // type
              format_descriptor,             // format
              image_width,                   // width
              image_height,                  // height
              image_depth,                   // depth
              0,                             // arraylevels
              0};                            // miplevels
          lzt::print_image_descriptor(image_descriptor);
          lzt::create_xe_image(image, &image_descriptor);
          lzt::destroy_xe_image(image);
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(MediaImageCreationCombinations, xeImageMediaCreateTests,
                        ::testing::Combine(lzt::image_format_media_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImageArrayMediaCreateTests,
    GivenValidDescriptorWhenCreating1DArrayImageWithMediaFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {

      xe_image_handle_t image;
      xe_image_format_desc_t format_descriptor = {
          std::get<0>(GetParam()),   // layout
          std::get<1>(GetParam()),   // type
          XE_IMAGE_FORMAT_SWIZZLE_X, // x
          XE_IMAGE_FORMAT_SWIZZLE_X, // y
          XE_IMAGE_FORMAT_SWIZZLE_X, // z
          XE_IMAGE_FORMAT_SWIZZLE_X  // w
      };

      xe_image_desc_t image_descriptor = {
          XE_IMAGE_DESC_VERSION_CURRENT, // version
          image_create_flags,            // flags
          XE_IMAGE_TYPE_1DARRAY,         // type
          format_descriptor,             // format
          image_width,                   // width
          0,                             // height
          0,                             // depth
          std::get<2>(GetParam()),       // arraylevels
          0};                            // miplevels
      lzt::print_image_descriptor(image_descriptor);
      lzt::create_xe_image(image, &image_descriptor);
      lzt::destroy_xe_image(image);
    }
  }
}

TEST_P(
    xeImageArrayMediaCreateTests,
    GivenValidDescriptorWhenCreating2DArrayImageWithMediaFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {

        xe_image_handle_t image;
        xe_image_format_desc_t format_descriptor = {
            std::get<0>(GetParam()),   // layout
            std::get<1>(GetParam()),   // type
            XE_IMAGE_FORMAT_SWIZZLE_X, // x
            XE_IMAGE_FORMAT_SWIZZLE_X, // y
            XE_IMAGE_FORMAT_SWIZZLE_X, // z
            XE_IMAGE_FORMAT_SWIZZLE_X  // w
        };

        xe_image_desc_t image_descriptor = {
            XE_IMAGE_DESC_VERSION_CURRENT, // version
            image_create_flags,            // flags
            XE_IMAGE_TYPE_2DARRAY,         // type
            format_descriptor,             // format
            image_width,                   // width
            image_height,                  // height
            0,                             // depth
            std::get<2>(GetParam()),       // arraylevels
            0};                            // miplevels
        lzt::print_image_descriptor(image_descriptor);
        lzt::create_xe_image(image, &image_descriptor);
        lzt::destroy_xe_image(image);
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(MediaArrayImageCreationCombinations,
                        xeImageArrayMediaCreateTests,
                        ::testing::Combine(lzt::image_format_media_layouts,
                                           lzt::image_format_types,
                                           lzt::image_array_levels));

} // namespace

// TODO: Test all image properties
// TODO: Test image queries
// TODO: Test different images with different samplers
// TODO: Test images on a device using OpenCL C kernels
