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

#ifndef _COMMON_HPP_
#define _COMMON_HPP_
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

template <typename T> class Timer {
public:
  Timer() { overhead(); }
  inline void start() {
    time_start = std::chrono::high_resolution_clock::now();
  }

  inline void end() { time_end = std::chrono::high_resolution_clock::now(); }

  inline int64_t period_minus_overhead() {
    return std::chrono::duration_cast<T>(time_end - time_start).count() -
           time_overhead;
  }

  inline bool has_it_been(long long int moment) {
    std::chrono::high_resolution_clock::time_point time_now =
        std::chrono::high_resolution_clock::now();
    auto period = std::chrono::duration_cast<T>(time_now - time_start);
    return (period.count() >= moment);
  }

private:
  std::chrono::high_resolution_clock::time_point time_start, time_end;
  int64_t time_overhead;

  inline void overhead() {
    start();
    end();
    auto period = std::chrono::duration_cast<T>(time_end - time_start);
    time_overhead = period.count();
  }
};

extern bool verbose;

template <bool TerminateOnFailure, typename ResulT>
inline void validate(ResulT result, const char *message) {
  if (result != 0) { /* assumption 0 is success */
    std::cerr << (TerminateOnFailure ? "ERROR : " : "WARNING : ") << message
              << " : " << result << std::endl;
    if (TerminateOnFailure) {
      std::terminate();
    }
    return;
  }

  if (verbose) {
    std::cerr << " SUCCESS : " << message << std::endl;
  }
}

#define SUCCESS_OR_TERMINATE(CALL) validate<true>(CALL, #CALL)

template <typename T> int size_in_bytes(const std::vector<T> &v) {
  return static_cast<int>(sizeof(T) * v.size());
}

#define ERROR_RETURN(retval)                                                   \
  {                                                                            \
    std::cerr << "ERROR : " << __FILE__ << ":" << __LINE__ << " " << ret       \
              << std::endl;                                                    \
    std::terminate();                                                          \
  }

#endif /* _COMMON_HPP_ */
