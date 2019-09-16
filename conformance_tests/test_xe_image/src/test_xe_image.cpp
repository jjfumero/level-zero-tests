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
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};

class xeImage1DSwizzleGetPropertiesTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};

class xeImageArray1DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};

class xeImageArray1DSwizzleGetPropertiesTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};

class xeImage2DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};

class xeImage2DSwizzleGetPropertiesTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};

class xeImageArray2DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};

class xeImageArray2DSwizzleGetPropertiesTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};

class xeImage3DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};

class xeImage3DSwizzleGetPropertiesTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};

class xeImageArray3DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};
class xeImageArray3DSwizzleGetPropertiesTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};

class xeImage4DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};
class xeImage4DSwizzleGetPropertiesTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};
class xeImageArray4DSwizzleCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};
class xeImageArray4DSwizzleGetPropertiesTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};
class xeImageMediaCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};
class xeImageMediaGetPropertiesTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};
class xeImageArrayMediaCreateTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};
class xeImageArrayMediaGetPropertiesTests
    : public lzt::xeImageCreateCommonTests,
      public ::testing::WithParamInterface<
          std::tuple<xe_image_format_layout_t, xe_image_format_type_t>> {};

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
      for (auto swizzle_x : lzt::image_format_swizzles) {

        xe_image_handle_t image;
        xe_image_format_desc_t format_descriptor = {
            std::get<0>(GetParam()),   // layout
            std::get<1>(GetParam()),   // type
            swizzle_x,                 // x
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
}

TEST_P(
    xeImage1DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DImageWith1DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {

          xe_image_handle_t image;
          xe_image_format_desc_t format_descriptor = {
              std::get<0>(GetParam()),   // layout
              std::get<1>(GetParam()),   // type
              swizzle_x,                 // x
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
}

TEST_P(
    xeImage1DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating3DImageWith1DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto image_depth : lzt::image_depths) {
          for (auto swizzle_x : lzt::image_format_swizzles) {

            xe_image_handle_t image;
            xe_image_format_desc_t format_descriptor = {
                std::get<0>(GetParam()),   // layout
                std::get<1>(GetParam()),   // type
                swizzle_x,                 // x
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
}

INSTANTIATE_TEST_CASE_P(1DSwizzleImageCreationCombinations,
                        xeImage1DSwizzleCreateTests,
                        ::testing::Combine(lzt::image_format_1d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImageArray1DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DArrayImageWith1DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto array_level : lzt::image_array_levels) {

          xe_image_handle_t image;
          xe_image_format_desc_t format_descriptor = {
              std::get<0>(GetParam()),   // layout
              std::get<1>(GetParam()),   // type
              swizzle_x,                 // x
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
              array_level,                   // arraylevels
              0};                            // miplevels
          lzt::print_image_descriptor(image_descriptor);
          lzt::create_xe_image(image, &image_descriptor);
          lzt::destroy_xe_image(image);
        }
      }
    }
  }
}

TEST_P(
    xeImageArray1DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DArrayImageWith1DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {
          for (auto array_level : lzt::image_array_levels) {

            xe_image_handle_t image;
            xe_image_format_desc_t format_descriptor = {
                std::get<0>(GetParam()),   // layout
                std::get<1>(GetParam()),   // type
                swizzle_x,                 // x
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
                array_level,                   // arraylevels
                0};                            // miplevels
            lzt::print_image_descriptor(image_descriptor);
            lzt::create_xe_image(image, &image_descriptor);
            lzt::destroy_xe_image(image);
          }
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(1DSwizzleArrayImageCreationCombinations,
                        xeImageArray1DSwizzleCreateTests,
                        ::testing::Combine(lzt::image_format_1d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImage2DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DImageWith2DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto swizzle_y : lzt::image_format_swizzles) {

          xe_image_handle_t image;
          xe_image_format_desc_t format_descriptor = {
              std::get<0>(GetParam()),   // layout
              std::get<1>(GetParam()),   // type
              swizzle_x,                 // x
              swizzle_y,                 // y
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
  }
}

TEST_P(
    xeImage2DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DImageWith2DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {
          for (auto swizzle_y : lzt::image_format_swizzles) {

            xe_image_handle_t image;
            xe_image_format_desc_t format_descriptor = {
                std::get<0>(GetParam()),   // layout
                std::get<1>(GetParam()),   // type
                swizzle_x,                 // x
                swizzle_y,                 // y
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
  }
}

TEST_P(
    xeImage2DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating3DImageWith2DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto image_depth : lzt::image_depths) {
          for (auto swizzle_x : lzt::image_format_swizzles) {
            for (auto swizzle_y : lzt::image_format_swizzles) {

              xe_image_handle_t image;
              xe_image_format_desc_t format_descriptor = {
                  std::get<0>(GetParam()),   // layout
                  std::get<1>(GetParam()),   // type
                  swizzle_x,                 // x
                  swizzle_y,                 // y
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
  }
}

INSTANTIATE_TEST_CASE_P(2DSwizzleImageCreationCombinations,
                        xeImage2DSwizzleCreateTests,
                        ::testing::Combine(lzt::image_format_2d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImageArray2DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DArrayImageWith2DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto swizzle_y : lzt::image_format_swizzles) {
          for (auto array_level : lzt::image_array_levels) {

            xe_image_handle_t image;
            xe_image_format_desc_t format_descriptor = {
                std::get<0>(GetParam()),   // layout
                std::get<1>(GetParam()),   // type
                swizzle_x,                 // x
                swizzle_y,                 // y
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
                array_level,                   // arraylevels
                0};                            // miplevels
            lzt::print_image_descriptor(image_descriptor);
            lzt::create_xe_image(image, &image_descriptor);
            lzt::destroy_xe_image(image);
          }
        }
      }
    }
  }
}

TEST_P(
    xeImageArray2DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DArrayImageWith2DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {
          for (auto swizzle_y : lzt::image_format_swizzles) {
            for (auto array_level : lzt::image_array_levels) {

              xe_image_handle_t image;
              xe_image_format_desc_t format_descriptor = {
                  std::get<0>(GetParam()),   // layout
                  std::get<1>(GetParam()),   // type
                  swizzle_x,                 // x
                  swizzle_y,                 // y
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
                  array_level,                   // arraylevels
                  0};                            // miplevels
              lzt::print_image_descriptor(image_descriptor);
              lzt::create_xe_image(image, &image_descriptor);
              lzt::destroy_xe_image(image);
            }
          }
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(2DSwizzleArrayImageCreationCombinations,
                        xeImageArray2DSwizzleCreateTests,
                        ::testing::Combine(lzt::image_format_2d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImage3DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DImageWith3DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto swizzle_y : lzt::image_format_swizzles) {
          for (auto swizzle_z : lzt::image_format_swizzles) {

            xe_image_handle_t image;
            xe_image_format_desc_t format_descriptor = {
                std::get<0>(GetParam()),  // layout
                std::get<1>(GetParam()),  // type
                swizzle_x,                // x
                swizzle_y,                // y
                swizzle_z,                // z
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
    }
  }
}

TEST_P(
    xeImage3DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DImageWith3DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {
          for (auto swizzle_y : lzt::image_format_swizzles) {
            for (auto swizzle_z : lzt::image_format_swizzles) {

              xe_image_handle_t image;
              xe_image_format_desc_t format_descriptor = {
                  std::get<0>(GetParam()),  // layout
                  std::get<1>(GetParam()),  // type
                  swizzle_x,                // x
                  swizzle_y,                // y
                  swizzle_z,                // z
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
          for (auto swizzle_x : lzt::image_format_swizzles) {
            for (auto swizzle_y : lzt::image_format_swizzles) {
              for (auto swizzle_z : lzt::image_format_swizzles) {

                xe_image_handle_t image;
                xe_image_format_desc_t format_descriptor = {
                    std::get<0>(GetParam()),  // layout
                    std::get<1>(GetParam()),  // type
                    swizzle_x,                // x
                    swizzle_y,                // y
                    swizzle_z,                // z
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
    }
  }
}

INSTANTIATE_TEST_CASE_P(3DSwizzleImageCreationCombinations,
                        xeImage3DSwizzleCreateTests,
                        ::testing::Combine(lzt::image_format_3d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImageArray3DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DArrayImageWith3DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto swizzle_y : lzt::image_format_swizzles) {
          for (auto swizzle_z : lzt::image_format_swizzles) {
            for (auto array_level : lzt::image_array_levels) {

              xe_image_handle_t image;
              xe_image_format_desc_t format_descriptor = {
                  std::get<0>(GetParam()),  // layout
                  std::get<1>(GetParam()),  // type
                  swizzle_x,                // x
                  swizzle_y,                // y
                  swizzle_z,                // z
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
                  array_level,                   // arraylevels
                  0};                            // miplevels
              lzt::print_image_descriptor(image_descriptor);
              lzt::create_xe_image(image, &image_descriptor);
              lzt::destroy_xe_image(image);
            }
          }
        }
      }
    }
  }
}

TEST_P(
    xeImageArray3DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DArrayImageWith3DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {
          for (auto swizzle_y : lzt::image_format_swizzles) {
            for (auto swizzle_z : lzt::image_format_swizzles) {
              for (auto array_level : lzt::image_array_levels) {

                xe_image_handle_t image;
                xe_image_format_desc_t format_descriptor = {
                    std::get<0>(GetParam()),  // layout
                    std::get<1>(GetParam()),  // type
                    swizzle_x,                // x
                    swizzle_y,                // y
                    swizzle_z,                // z
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
                    array_level,                   // arraylevels
                    0};                            // miplevels
                lzt::print_image_descriptor(image_descriptor);
                lzt::create_xe_image(image, &image_descriptor);
                lzt::destroy_xe_image(image);
              }
            }
          }
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(3DSwizzleArrayImageCreationCombinations,
                        xeImageArray3DSwizzleCreateTests,
                        ::testing::Combine(lzt::image_format_3d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImage4DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DImageWith4DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto swizzle_y : lzt::image_format_swizzles) {
          for (auto swizzle_z : lzt::image_format_swizzles) {
            for (auto swizzle_w : lzt::image_format_swizzles) {

              xe_image_handle_t image;
              xe_image_format_desc_t format_descriptor = {
                  std::get<0>(GetParam()), // layout
                  std::get<1>(GetParam()), // type
                  swizzle_x,               // x
                  swizzle_y,               // y
                  swizzle_z,               // z
                  swizzle_w                // w
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
      }
    }
  }
}

TEST_P(
    xeImage4DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DImageWith4DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {
          for (auto swizzle_y : lzt::image_format_swizzles) {
            for (auto swizzle_z : lzt::image_format_swizzles) {
              for (auto swizzle_w : lzt::image_format_swizzles) {

                xe_image_handle_t image;
                xe_image_format_desc_t format_descriptor = {
                    std::get<0>(GetParam()), // layout
                    std::get<1>(GetParam()), // type
                    swizzle_x,               // x
                    swizzle_y,               // y
                    swizzle_z,               // z
                    swizzle_w                // w
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
          for (auto swizzle_x : lzt::image_format_swizzles) {
            for (auto swizzle_y : lzt::image_format_swizzles) {
              for (auto swizzle_z : lzt::image_format_swizzles) {
                for (auto swizzle_w : lzt::image_format_swizzles) {

                  xe_image_handle_t image;
                  xe_image_format_desc_t format_descriptor = {
                      std::get<0>(GetParam()), // layout
                      std::get<1>(GetParam()), // type
                      swizzle_x,               // x
                      swizzle_y,               // y
                      swizzle_z,               // z
                      swizzle_w                // w
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
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(4DSwizzleImageCreationCombinations,
                        xeImage4DSwizzleCreateTests,
                        ::testing::Combine(lzt::image_format_4d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImageArray4DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating1DArrayImageWith4DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto swizzle_y : lzt::image_format_swizzles) {
          for (auto swizzle_z : lzt::image_format_swizzles) {
            for (auto swizzle_w : lzt::image_format_swizzles) {
              for (auto array_level : lzt::image_array_levels) {

                xe_image_handle_t image;
                xe_image_format_desc_t format_descriptor = {
                    std::get<0>(GetParam()), // layout
                    std::get<1>(GetParam()), // type
                    swizzle_x,               // x
                    swizzle_y,               // y
                    swizzle_z,               // z
                    swizzle_w                // w
                };

                xe_image_desc_t image_descriptor = {
                    XE_IMAGE_DESC_VERSION_CURRENT, // version
                    image_create_flags,            // flags
                    XE_IMAGE_TYPE_1DARRAY,         // type
                    format_descriptor,             // format
                    image_width,                   // width
                    0,                             // height
                    0,                             // depth
                    array_level,                   // arraylevels
                    0};                            // miplevels
                lzt::print_image_descriptor(image_descriptor);
                lzt::create_xe_image(image, &image_descriptor);
                lzt::destroy_xe_image(image);
              }
            }
          }
        }
      }
    }
  }
}

TEST_P(
    xeImageArray4DSwizzleCreateTests,
    GivenValidDescriptorWhenCreating2DArrayImageWith4DSwizzleFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {
          for (auto swizzle_y : lzt::image_format_swizzles) {
            for (auto swizzle_z : lzt::image_format_swizzles) {
              for (auto swizzle_w : lzt::image_format_swizzles) {
                for (auto array_level : lzt::image_array_levels) {

                  xe_image_handle_t image;
                  xe_image_format_desc_t format_descriptor = {
                      std::get<0>(GetParam()), // layout
                      std::get<1>(GetParam()), // type
                      swizzle_x,               // x
                      swizzle_y,               // y
                      swizzle_z,               // z
                      swizzle_w                // w
                  };

                  xe_image_desc_t image_descriptor = {
                      XE_IMAGE_DESC_VERSION_CURRENT, // version
                      image_create_flags,            // flags
                      XE_IMAGE_TYPE_2DARRAY,         // type
                      format_descriptor,             // format
                      image_width,                   // width
                      image_height,                  // height
                      0,                             // depth
                      array_level,                   // arraylevels
                      0};                            // miplevels
                  lzt::print_image_descriptor(image_descriptor);
                  lzt::create_xe_image(image, &image_descriptor);
                  lzt::destroy_xe_image(image);
                }
              }
            }
          }
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(4DSwizzleArrayImageCreationCombinations,
                        xeImageArray4DSwizzleCreateTests,
                        ::testing::Combine(lzt::image_format_4d_swizzle_layouts,
                                           lzt::image_format_types));

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
      for (auto array_level : lzt::image_array_levels) {

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
            array_level,                   // arraylevels
            0};                            // miplevels
        lzt::print_image_descriptor(image_descriptor);
        lzt::create_xe_image(image, &image_descriptor);
        lzt::destroy_xe_image(image);
      }
    }
  }
}

TEST_P(
    xeImageArrayMediaCreateTests,
    GivenValidDescriptorWhenCreating2DArrayImageWithMediaFormatThenNotNullPointerIsReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto array_level : lzt::image_array_levels) {

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
              array_level,                   // arraylevels
              0};                            // miplevels
          lzt::print_image_descriptor(image_descriptor);
          lzt::create_xe_image(image, &image_descriptor);
          lzt::destroy_xe_image(image);
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(MediaArrayImageCreationCombinations,
                        xeImageArrayMediaCreateTests,
                        ::testing::Combine(lzt::image_format_media_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImage1DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor1DImageWith1DSwizzleThenValidPropertiesReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        xe_image_format_desc_t format_descriptor = {
            std::get<0>(GetParam()),   // layout
            std::get<1>(GetParam()),   // type
            swizzle_x,                 // x
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
        lzt::get_xe_image_properties(image_descriptor);
      }
    }
  }
}

TEST_P(
    xeImage1DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor2DImageWith1DSwizzleThenValidPropertiesReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {
          xe_image_format_desc_t format_descriptor = {
              std::get<0>(GetParam()),   // layout
              std::get<1>(GetParam()),   // type
              swizzle_x,                 // x
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
          lzt::get_xe_image_properties(image_descriptor);
        }
      }
    }
  }
}

TEST_P(
    xeImage1DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor3DImageWith1DSwizzleThenValidPropertiesReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto image_depth : lzt::image_depths) {
          for (auto swizzle_x : lzt::image_format_swizzles) {
            xe_image_format_desc_t format_descriptor = {
                std::get<0>(GetParam()),   // layout
                std::get<1>(GetParam()),   // type
                swizzle_x,                 // x
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
            lzt::get_xe_image_properties(image_descriptor);
          }
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(ImageGetPropertiesTest,
                        xeImage1DSwizzleGetPropertiesTests,
                        ::testing::Combine(lzt::image_format_1d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImageArray1DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor1DImageArrayWith1DSwizzleThenValidPropertiesReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto array_level : lzt::image_array_levels) {
          xe_image_format_desc_t format_descriptor = {
              std::get<0>(GetParam()),   // layout
              std::get<1>(GetParam()),   // type
              swizzle_x,                 // x
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
              array_level,                   // arraylevels
              0};                            // miplevels

          lzt::print_image_descriptor(image_descriptor);
          lzt::get_xe_image_properties(image_descriptor);
        }
      }
    }
  }
}

TEST_P(
    xeImageArray1DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor2DImageArrayWith1DSwizzleThenValidPropertiesReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {
          for (auto array_level : lzt::image_array_levels) {
            xe_image_format_desc_t format_descriptor = {
                std::get<0>(GetParam()),   // layout
                std::get<1>(GetParam()),   // type
                swizzle_x,                 // x
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
                array_level,                   // arraylevels
                0};                            // miplevels

            lzt::print_image_descriptor(image_descriptor);
            lzt::get_xe_image_properties(image_descriptor);
          }
        }
      }
    }
  }
}
INSTANTIATE_TEST_CASE_P(ImageGetPropertiesTest,
                        xeImageArray1DSwizzleGetPropertiesTests,
                        ::testing::Combine(lzt::image_format_1d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImage2DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor1DImageWith2DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto swizzle_y : lzt::image_format_swizzles) {
          xe_image_format_desc_t format_descriptor = {
              std::get<0>(GetParam()),   // layout
              std::get<1>(GetParam()),   // type
              swizzle_x,                 // x
              swizzle_y,                 // y
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
          lzt::get_xe_image_properties(image_descriptor);
        }
      }
    }
  }
}

TEST_P(
    xeImage2DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor2DImageWith2DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {
          for (auto swizzle_y : lzt::image_format_swizzles) {
            xe_image_format_desc_t format_descriptor = {
                std::get<0>(GetParam()),   // layout
                std::get<1>(GetParam()),   // type
                swizzle_x,                 // x
                swizzle_y,                 // y
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
            lzt::get_xe_image_properties(image_descriptor);
          }
        }
      }
    }
  }
}

TEST_P(
    xeImage2DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor3DImageWith2DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto image_depth : lzt::image_depths) {
          for (auto swizzle_x : lzt::image_format_swizzles) {
            for (auto swizzle_y : lzt::image_format_swizzles) {
              xe_image_format_desc_t format_descriptor = {
                  std::get<0>(GetParam()),   // layout
                  std::get<1>(GetParam()),   // type
                  swizzle_x,                 // x
                  swizzle_y,                 // y
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
              lzt::get_xe_image_properties(image_descriptor);
            }
          }
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(ImageGetPropertiesTest,
                        xeImage2DSwizzleGetPropertiesTests,
                        ::testing::Combine(lzt::image_format_1d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImageArray2DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor1DImageArrayWith2DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto array_level : lzt::image_array_levels) {
          xe_image_format_desc_t format_descriptor = {
              std::get<0>(GetParam()),   // layout
              std::get<1>(GetParam()),   // type
              swizzle_x,                 // x
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
              array_level,                   // arraylevels
              0};                            // miplevels

          lzt::print_image_descriptor(image_descriptor);
          lzt::get_xe_image_properties(image_descriptor);
        }
      }
    }
  }
}

TEST_P(
    xeImageArray2DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor2DImageArrayWith2DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {
          for (auto array_level : lzt::image_array_levels) {
            xe_image_format_desc_t format_descriptor = {
                std::get<0>(GetParam()),   // layout
                std::get<1>(GetParam()),   // type
                swizzle_x,                 // x
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
                array_level,                   // arraylevels
                0};                            // miplevels

            lzt::print_image_descriptor(image_descriptor);
            lzt::get_xe_image_properties(image_descriptor);
          }
        }
      }
    }
  }
}
INSTANTIATE_TEST_CASE_P(ImageGetPropertiesTest,
                        xeImageArray2DSwizzleGetPropertiesTests,
                        ::testing::Combine(lzt::image_format_1d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImage3DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor1DImageWith3DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto swizzle_y : lzt::image_format_swizzles) {
          for (auto swizzle_z : lzt::image_format_swizzles) {
            xe_image_format_desc_t format_descriptor = {
                std::get<0>(GetParam()),  // layout
                std::get<1>(GetParam()),  // type
                swizzle_x,                // x
                swizzle_y,                // y
                swizzle_z,                // z
                XE_IMAGE_FORMAT_SWIZZLE_X // w
            };

            xe_image_desc_t image_descriptor = {
                XE_IMAGE_DESC_VERSION_CURRENT, // version
                image_create_flags,            // flags
                XE_IMAGE_TYPE_3D,              // type
                format_descriptor,             // format
                image_width,                   // width
                0,                             // height
                0,                             // depth
                0,                             // arraylevels
                0};                            // miplevels

            lzt::print_image_descriptor(image_descriptor);
            lzt::get_xe_image_properties(image_descriptor);
          }
        }
      }
    }
  }
}
TEST_P(
    xeImage3DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor2DImageWith3DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {
          for (auto swizzle_y : lzt::image_format_swizzles) {
            for (auto swizzle_z : lzt::image_format_swizzles) {
              xe_image_format_desc_t format_descriptor = {
                  std::get<0>(GetParam()),  // layout
                  std::get<1>(GetParam()),  // type
                  swizzle_x,                // x
                  swizzle_y,                // y
                  swizzle_z,                // z
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
              lzt::get_xe_image_properties(image_descriptor);
            }
          }
        }
      }
    }
  }
}

TEST_P(
    xeImage3DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor3DImageWith3DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto image_depth : lzt::image_depths) {
          for (auto swizzle_x : lzt::image_format_swizzles) {
            for (auto swizzle_y : lzt::image_format_swizzles) {
              for (auto swizzle_z : lzt::image_format_swizzles) {
                xe_image_format_desc_t format_descriptor = {
                    std::get<0>(GetParam()),  // layout
                    std::get<1>(GetParam()),  // type
                    swizzle_x,                // x
                    swizzle_y,                // y
                    swizzle_z,                // z
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
                lzt::get_xe_image_properties(image_descriptor);
              }
            }
          }
        }
      }
    }
  }
}
INSTANTIATE_TEST_CASE_P(ImageGetPropertiesTest,
                        xeImage3DSwizzleGetPropertiesTests,
                        ::testing::Combine(lzt::image_format_3d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImageArray3DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor1DImageArrayWith3DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto swizzle_y : lzt::image_format_swizzles) {
          for (auto swizzle_z : lzt::image_format_swizzles) {
            for (auto array_level : lzt::image_array_levels) {
              xe_image_format_desc_t format_descriptor = {
                  std::get<0>(GetParam()),  // layout
                  std::get<1>(GetParam()),  // type
                  swizzle_x,                // x
                  swizzle_y,                // y
                  swizzle_z,                // z
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
                  array_level,                   // arraylevels
                  0};                            // miplevels

              lzt::print_image_descriptor(image_descriptor);
              lzt::get_xe_image_properties(image_descriptor);
            }
          }
        }
      }
    }
  }
}

TEST_P(
    xeImageArray3DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor2DImageArrayWith3DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {
          for (auto swizzle_y : lzt::image_format_swizzles) {
            for (auto swizzle_z : lzt::image_format_swizzles) {
              for (auto array_level : lzt::image_array_levels) {
                xe_image_format_desc_t format_descriptor = {
                    std::get<0>(GetParam()),  // layout
                    std::get<1>(GetParam()),  // type
                    swizzle_x,                // x
                    swizzle_y,                // y
                    swizzle_z,                // z
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
                    array_level,                   // arraylevels
                    0};                            // miplevels

                lzt::print_image_descriptor(image_descriptor);
                lzt::get_xe_image_properties(image_descriptor);
              }
            }
          }
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(ImageGetPropertiesTest,
                        xeImageArray3DSwizzleGetPropertiesTests,
                        ::testing::Combine(lzt::image_format_3d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImage4DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor1DImageWith4DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto swizzle_y : lzt::image_format_swizzles) {
          for (auto swizzle_z : lzt::image_format_swizzles) {
            for (auto swizzle_w : lzt::image_format_swizzles) {
              xe_image_format_desc_t format_descriptor = {
                  std::get<0>(GetParam()), // layout
                  std::get<1>(GetParam()), // type
                  swizzle_x,               // x
                  swizzle_y,               // y
                  swizzle_z,               // z
                  swizzle_w                // w
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
              lzt::get_xe_image_properties(image_descriptor);
            }
          }
        }
      }
    }
  }
}
TEST_P(
    xeImage4DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor2DImageWith4DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto swizzle_x : lzt::image_format_swizzles) {
          for (auto swizzle_y : lzt::image_format_swizzles) {
            for (auto swizzle_z : lzt::image_format_swizzles) {
              for (auto swizzle_w : lzt::image_format_swizzles) {
                xe_image_format_desc_t format_descriptor = {
                    std::get<0>(GetParam()), // layout
                    std::get<1>(GetParam()), // type
                    swizzle_x,               // x
                    swizzle_y,               // y
                    swizzle_z,               // z
                    swizzle_w                // w
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
                lzt::get_xe_image_properties(image_descriptor);
              }
            }
          }
        }
      }
    }
  }
}

TEST_P(
    xeImage4DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor3DImageWith4DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto image_depth : lzt::image_depths) {
          for (auto swizzle_x : lzt::image_format_swizzles) {
            for (auto swizzle_y : lzt::image_format_swizzles) {
              for (auto swizzle_z : lzt::image_format_swizzles) {
                for (auto swizzle_w : lzt::image_format_swizzles) {
                  xe_image_format_desc_t format_descriptor = {
                      std::get<0>(GetParam()), // layout
                      std::get<1>(GetParam()), // type
                      swizzle_x,               // x
                      swizzle_y,               // y
                      swizzle_z,               // z
                      swizzle_w                // w
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
                  lzt::get_xe_image_properties(image_descriptor);
                }
              }
            }
          }
        }
      }
    }
  }
}
INSTANTIATE_TEST_CASE_P(ImageGetPropertiesTest,
                        xeImage4DSwizzleGetPropertiesTests,
                        ::testing::Combine(lzt::image_format_4d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImageArray4DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor1DImageArrayWith4DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto swizzle_y : lzt::image_format_swizzles) {
          for (auto swizzle_z : lzt::image_format_swizzles) {
            for (auto swizzle_w : lzt::image_format_swizzles) {
              for (auto array_level : lzt::image_array_levels) {
                xe_image_format_desc_t format_descriptor = {
                    std::get<0>(GetParam()), // layout
                    std::get<1>(GetParam()), // type
                    swizzle_x,               // x
                    swizzle_y,               // y
                    swizzle_z,               // z
                    swizzle_w                // w
                };

                xe_image_desc_t image_descriptor = {
                    XE_IMAGE_DESC_VERSION_CURRENT, // version
                    image_create_flags,            // flags
                    XE_IMAGE_TYPE_1DARRAY,         // type
                    format_descriptor,             // format
                    image_width,                   // width
                    0,                             // height
                    0,                             // depth
                    array_level,                   // arraylevels
                    0};                            // miplevels

                lzt::print_image_descriptor(image_descriptor);
                lzt::get_xe_image_properties(image_descriptor);
              }
            }
          }
        }
      }
    }
  }
}

TEST_P(
    xeImageArray4DSwizzleGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor2DImageArrayWith4DSwizzleThenValidPropertiesReturned) {
  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto swizzle_x : lzt::image_format_swizzles) {
        for (auto swizzle_y : lzt::image_format_swizzles) {
          for (auto swizzle_z : lzt::image_format_swizzles) {
            for (auto swizzle_w : lzt::image_format_swizzles) {
              for (auto array_level : lzt::image_array_levels) {
                xe_image_format_desc_t format_descriptor = {
                    std::get<0>(GetParam()), // layout
                    std::get<1>(GetParam()), // type
                    swizzle_x,               // x
                    swizzle_y,               // y
                    swizzle_z,               // z
                    swizzle_w                // w
                };

                xe_image_desc_t image_descriptor = {
                    XE_IMAGE_DESC_VERSION_CURRENT, // version
                    image_create_flags,            // flags
                    XE_IMAGE_TYPE_2DARRAY,         // type
                    format_descriptor,             // format
                    image_width,                   // width
                    0,                             // height
                    0,                             // depth
                    array_level,                   // arraylevels
                    0};                            // miplevels

                lzt::print_image_descriptor(image_descriptor);
                lzt::get_xe_image_properties(image_descriptor);
              }
            }
          }
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(ImageGetPropertiesTest,
                        xeImageArray4DSwizzleGetPropertiesTests,
                        ::testing::Combine(lzt::image_format_4d_swizzle_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImageMediaGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor1DImageWithMediaFormatThenValidPropertiesReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
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
      lzt::get_xe_image_properties(image_descriptor);
    }
  }
}

TEST_P(
    xeImageMediaGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor2DImageWithMediaFormatThenValidPropertiesReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
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
        lzt::get_xe_image_properties(image_descriptor);
      }
    }
  }
}

TEST_P(
    xeImageMediaGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor3DImageWithMediaFormatThenValidPropertiesReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto image_depth : lzt::image_depths) {
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
          lzt::get_xe_image_properties(image_descriptor);
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(ImageGetPropertiesTest, xeImageMediaGetPropertiesTests,
                        ::testing::Combine(lzt::image_format_media_layouts,
                                           lzt::image_format_types));

TEST_P(
    xeImageArrayMediaGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor1DArrayImageWithMediaFormatThenValidPropertiesReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto array_level : lzt::image_array_levels) {
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
            array_level,                   // arraylevels
            0};                            // miplevels
        lzt::print_image_descriptor(image_descriptor);
        lzt::get_xe_image_properties(image_descriptor);
      }
    }
  }
}

TEST_P(
    xeImageArrayMediaGetPropertiesTests,
    GivenValidDescriptorWhenGettingPropertiesFor2DArrayImageWithMediaFormatThenValidPropertiesReturned) {

  for (auto image_create_flags : img.image_creation_flags_list_) {
    for (auto image_width : lzt::image_widths) {
      for (auto image_height : lzt::image_heights) {
        for (auto array_level : lzt::image_array_levels) {
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
              array_level,                   // arraylevels
              0};                            // miplevels
          lzt::print_image_descriptor(image_descriptor);
          lzt::get_xe_image_properties(image_descriptor);
        }
      }
    }
  }
}

INSTANTIATE_TEST_CASE_P(ImageGetPropertiesTest,
                        xeImageArrayMediaGetPropertiesTests,
                        ::testing::Combine(lzt::image_format_media_layouts,
                                           lzt::image_format_types));
} // namespace

// TODO: Test image queries
// TODO: Test different images with different samplers
// TODO: Test images on a device using OpenCL C kernels
