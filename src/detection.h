// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef __FX_DETECTION_H__
#define __FX_DETECTION_H__

#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <opencv2/opencv.hpp>
#include <vector>

namespace FX {
class Detection {
public:
  Detection();

  Detection(const dlib::full_object_detection &det);

  bool IsInvalid();

  size_t Count();

  double Dis(int i, int j);

  double Dis(std::vector<std::vector<int>> list);

  double DisP(cv::Point2d p, std::vector<int> list);

  cv::Point2d Mid(int i, int j);

  cv::Point2d &operator[](size_t index);

private:
  std::vector<cv::Point2d> _points;
  bool _invalid = false;
};
} // namespace FX

#endif
