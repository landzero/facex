// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "result.h"

#include <dlib/serialize.h>

void FX::Result::Serialize(std::ostream &out) {
  out << width << "," << height << ";";
  out << t0 << "," << t1 << "," << t2 << ";";
  out << r0 << "," << r1 << "," << r2 << ";";
  out << std::endl;
}