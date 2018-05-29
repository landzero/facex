// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef __FX_RESOLVER_H__
#define __FX_RESOLVER_H__

#include "result.h"

#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>

namespace FX {
class Resolver {
public:
  Resolver();

  void Resolve(int cols, int rows, dlib::full_object_detection &detection,
               Result &result);

private:
  std::vector<cv::Point3d> _referencePoints;
  cv::Mat _distCoeffs;
};
} // namespace FX

#endif //__FX_RESOLVER_H__
