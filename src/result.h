// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef __FX_RESULT_H__
#define __FX_RESULT_H__

#include <iostream>

#include <dlib/threads.h>
#include <opencv2/core.hpp>

#include "store.h"

namespace FX {
class Result {
public:
  // camera size
  double width = 0;
  double height = 0;
  // translation vector
  double t0 = 0;
  double t1 = 0;
  double t2 = 0;
  // rotation vector
  double r0 = 0;
  double r1 = 0;
  double r2 = 0;

  // serialize result to stream
  void Serialize(std::ostream &out);
};

// ResultStore with wait and broadcast
using ResultStore = Store<Result>;

} // namespace FX

#endif