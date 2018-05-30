// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "result.h"

void FX::Result::Serialize(std::ostream &out) {
#define _PUT(V) out << #V << ":" << std::to_string(V) << ";"
  _PUT(w);
  _PUT(h);
  _PUT(t0);
  _PUT(t1);
  _PUT(t2);
  _PUT(r0);
  _PUT(r1);
  _PUT(r2);
  _PUT(le);
  _PUT(lb);
  _PUT(re);
  _PUT(rb);
  _PUT(mw);
  _PUT(mh);
  _PUT(base);
  out << std::endl;
}