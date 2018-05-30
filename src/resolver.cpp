// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "resolver.h"

#define NUM_LANDMARKS 68

bool _isFaceDetectionValid(dlib::full_object_detection &det) {
  if (det.num_parts() != NUM_LANDMARKS)
    return false;
  for (size_t i = 0; i < det.num_parts(); i++)
    if (det.part(i) == dlib::OBJECT_PART_NOT_PRESENT)
      return false;
  return true;
}

bool FX::Resolver::Resolve(int cols, int rows,
                           dlib::full_object_detection &detection,
                           FX::Result &res) {
  if (!_isFaceDetectionValid(detection))
    return false;
  FX::Detection det(detection);

  res.w = cols;
  res.h = rows;

  // cameraPoints
  std::vector<cv::Point2d> cp;
  // nose tip
  cp.push_back(det[30]);
  // chin
  cp.push_back(det[8]);
  // left eye left corner
  cp.push_back(det[36]);
  // right eye right corner
  cp.push_back(det[45]);
  // left Mouth corner
  cp.push_back(det[48]);
  // right mouth corner
  cp.push_back(det[54]);

  // camera matrix
  cv::Mat_<double> cameraMatrix(3, 3);
  cameraMatrix << cols, 0, cols / 2.f, 0, cols, rows / 2.f, 0, 0, 1;

  // rotation vector, translation vector
  cv::Mat rv, tv;

  // solve
  cv::solvePnP(_referencePoints, cp, cameraMatrix, _distCoeffs, rv, tv);

  // set rv, tv to result
  res.t0 = tv.at<double>(0, 0);
  res.t1 = tv.at<double>(0, 1);
  res.t2 = tv.at<double>(0, 2);

  res.r0 = rv.at<double>(0, 0);
  res.r1 = rv.at<double>(0, 1);
  res.r2 = rv.at<double>(0, 2);

  // use balanced nose length as base
  double base = det.Dis({{27, 31}, {27, 32}, {27, 33}, {27, 34}, {27, 35}});
  if (base == 0) {
    base = 1; // avoid divided by 0
  }
  res.base = base;

  // eye size
  res.le = det.Dis({{37, 40}, {38, 41}}) / base;
  res.re = det.Dis({{43, 46}, {44, 47}}) / base;

  // eyebrow height
  res.lb = det.DisP(det.Mid(36, 39), {17, 18, 19, 20, 21}) / base;
  res.rb = det.DisP(det.Mid(42, 45), {22, 23, 24, 25, 26}) / base;

  // mouth width/height
  res.mw = det.Dis(60, 64) / base;
  res.mh = det.Dis(62, 66) / base;

  return true;
}

FX::Resolver::Resolver() {
  // initialize reference points
  // The first must be (0,0,0) while using POSIT
  _referencePoints.emplace_back(0.0f, 0.0f, 0.0f);
  _referencePoints.emplace_back(0.0f, -330.0f, -65.0f);
  _referencePoints.emplace_back(-225.0f, 170.0f, -135.0f);
  _referencePoints.emplace_back(225.0f, 170.0f, -135.0f);
  _referencePoints.emplace_back(-150.0f, -150.0f, -125.0f);
  _referencePoints.emplace_back(150.0f, -150.0f, -125.0f);

  // initialize distCoeffs
  _distCoeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type);
}
