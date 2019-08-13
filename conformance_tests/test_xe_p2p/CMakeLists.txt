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

add_test_suite(test_xe_p2p
    SOURCE
    "src/test_xe_p2p.cpp"
    "src/main.cpp"
)
target_link_libraries(test_xe_p2p
    PUBLIC
    compute_samples::logging
    compute_samples::xe_utils
    compute_samples::utils
    compute_samples::xe_test_harness
    ${LevelZero_LIBRARIES}
)
add_kernels(test_xe_p2p "xe_atomic_access.cl")
add_kernels(test_xe_p2p "xe_concurrent_access.cl")
add_kernels(test_xe_p2p "xe_atomic_and_concurrent_access.cl")
add_kernels(test_xe_p2p "xe_p2p_test.cl")
install_resources(test_xe_p2p
    FILES
    "xe_atomic_access.spv"
)
install_resources(test_xe_p2p
    FILES
    "xe_concurrent_access.spv"
)
install_resources(test_xe_p2p
    FILES
    "xe_atomic_and_concurrent_access.spv"
)

install_resources(test_xe_p2p
    FILES
    "xe_p2p_test.spv"
)
