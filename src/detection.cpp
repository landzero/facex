// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "detection.h"

FX::Detection::Detection(dlib::full_object_detection &det) {
  for (size_t i = 0; i < det.num_parts(); i++) {
    _points.emplace_back(det.part(i).x(), det.part(i).y());
  }
}

double FX::Detection::Dis(int i, int j) {
  return cv::norm(_points[i] - _points[j]);
}

double FX::Detection::Dis(std::vector<std::vector<int>> list) {
  if (list.empty())
    return 0;
  double total = 0;
  for (auto bi : list) {
    total += Dis(bi[0], bi[1]);
  }
  return total / list.size();
}

double FX::Detection::DisP(cv::Point2d p, std::vector<int> list) {
  if (list.empty())
    return 0;
  double total = 0;
  for (auto i : list) {
    total += cv::norm(p - _points[i]);
  }
  return total / list.size();
}

cv::Point2d FX::Detection::Mid(int i, int j) {
  return (_points[i] + _points[j]) / 2;
}

cv::Point2d &FX::Detection::operator[](const int index) {
  return _points[index];
}
