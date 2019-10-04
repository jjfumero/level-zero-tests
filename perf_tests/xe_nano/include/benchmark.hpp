/*
 * INTEL CONFIDENTIAL
 * Copyright (c) 2016 - 2019 Intel Corporation. All Rights Reserved.
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

#ifndef _BENCHMARK_HPP_
#define _BENCHMARK_HPP_

#include "api_static_probe.hpp"
#include "xe_app.hpp"

namespace ze_api_benchmarks {
inline void init() { api_static_probe_init(); }
inline void cleanup() { api_static_probe_cleanup(); }

namespace latency {
#include "benchmark_template/command_list.hpp"
#include "benchmark_template/ipc.hpp"
#include "benchmark_template/set_parameter.hpp"
} /* namespace latency */
namespace hardware_counter {
#include "benchmark_template/command_list.hpp"
#include "benchmark_template/ipc.hpp"
#include "benchmark_template/set_parameter.hpp"
} /* namespace hardware_counter */
namespace fuction_call_rate {
#include "benchmark_template/command_list.hpp"
#include "benchmark_template/ipc.hpp"
#include "benchmark_template/set_parameter.hpp"
} /* namespace fuction_call_rate */
} /* namespace ze_api_benchmarks */

#endif /* _BENCHMARK_HPP_ */
