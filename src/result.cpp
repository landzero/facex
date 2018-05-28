// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "result.h"

#include <dlib/serialize.h>

void FX::Result::Serialize(std::ostream &out) {
  out << std::to_string(this->size.width) << ","
      << std::to_string(this->size.height) << ";";
  for (int i = 0; i < this->landmarks.size(); i++) {
    out << std::to_string(this->landmarks[i].x) << ","
        << std::to_string(this->landmarks[i].y) << ";";
  }
  out << std::to_string(this->location.x) << ","
      << std::to_string(this->location.y) << ","
      << std::to_string(this->location.z) << ";";

  out << std::to_string(this->direction[0]) << ","
      << std::to_string(this->direction[1]) << ","
      << std::to_string(this->direction[2]) << ";";

  out << std::endl;
}