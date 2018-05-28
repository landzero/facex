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
  cv::Size2d size;
  // 68-points standard face landmarks
  std::vector<cv::Point2d> landmarks;
  // calculated location of head
  cv::Point3d location;
  // calculated face direction
  cv::Vec3d direction;

  // serialize result to stream
  void Serialize(std::ostream &out);
};

// ResultStore with wait and broadcast
using ResultStore = Store<Result>;

} // namespace FX

#endif