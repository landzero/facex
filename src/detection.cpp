// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "detection.h"

FX::Detection::Detection() { _invalid = true; }

FX::Detection::Detection(const dlib::full_object_detection &det) {
  if (det.num_parts() != 68) {
    _invalid = true;
    return;
  }
  for (size_t i = 0; i < det.num_parts(); i++) {
    dlib::point p = det.part(i);
    if (p == dlib::OBJECT_PART_NOT_PRESENT) {
      _invalid = true;
      return;
    }
    _points.emplace_back(p.x(), p.y());
  }
}

size_t FX::Detection::Count() { return _points.size(); }

bool FX::Detection::IsInvalid() { return _invalid; }

double FX::Detection::Dis(int i, int j) { return cv::norm(_points[i] - _points[j]); }

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

cv::Point2d FX::Detection::Mid(int i, int j) { return (_points[i] + _points[j]) / 2; }

cv::Point2d &FX::Detection::operator[](size_t index) { return _points[index]; }

double FX::Detection::operator-(Detection &rdet) {
  double total = 0;
  for (size_t i = 0; i < Count(); i++) {
    total += cv::norm(_points[i] - rdet[i]);
  }
  return cv::sqrt(total);
}
