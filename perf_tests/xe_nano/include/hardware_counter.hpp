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

#ifndef _HARDWARE_COUNTER_HPP_
#define _HARDWARE_COUNTER_HPP_
#include <iostream>

class HardwareCounter {
public:
  HardwareCounter();
  ~HardwareCounter();
  void start(void);
  void end(void);
  long long counter_instructions(void);
  long long counter_cycles(void);
  bool is_supported(void);
  static std::string support_warning(void);

private:
  static const unsigned int number_events = 2;
  inline void counter_asserts(void);

#ifdef WITH_PAPI
  bool _counter_enabled;
  int event_set;

  /*
   * It is used to check that at least on measurement was taken
   * by calling start() and end()
   */
  bool measurement_taken;

  /*
   * It is used to determine if user finished the measurement
   * by calling end() after start().
   */
  bool active_period;

  long long values[number_events];
#endif
};

#endif /* _HARDWARE_COUNTER_HPP_ */
