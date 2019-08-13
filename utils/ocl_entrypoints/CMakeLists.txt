#
# Copyright(c) 2019 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

add_core_library(ocl_entrypoints
    SOURCE
    "include/ocl_entrypoints/loader.hpp"
    "include/ocl_entrypoints/embargo/cl_intel_unified_shared_memory.h"
    "include/ocl_entrypoints/embargo/cl_intel_create_buffer_with_properties_intel.h"
    "include/ocl_entrypoints/embargo/cl_intel_aubs.h"
    "src/embargo/cl_intel_unified_shared_memory.cpp"
    "src/embargo/cl_intel_create_buffer_with_properties_intel.cpp"
    "src/embargo/cl_intel_aubs.cpp"
)
target_link_libraries(ocl_entrypoints
    PUBLIC
    OpenCL::OpenCL
)
