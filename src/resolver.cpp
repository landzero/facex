// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "resolver.h"

#include <opencv/cv.hpp>

void FX::Resolver::Resolve(int cols, int rows,
                           dlib::full_object_detection &detection,
                           FX::Result &result) {
  result.width = cols;
  result.height = rows;

  // cameraPoints
  std::vector<cv::Point2d> cameraPoints;
  // nose tip
  cameraPoints.emplace_back(detection.part(30).x(), detection.part(30).y());
  // chin
  cameraPoints.emplace_back(detection.part(8).x(), detection.part(8).y());
  // left eye left corner
  cameraPoints.emplace_back(detection.part(36).x(), detection.part(36).y());
  // right eye right corner
  cameraPoints.emplace_back(detection.part(45).x(), detection.part(45).y());
  // left Mouth corner
  cameraPoints.emplace_back(detection.part(48).x(), detection.part(48).y());
  // right mouth corner
  cameraPoints.emplace_back(detection.part(54).x(), detection.part(54).y());

  // cameraMatrix
  cv::Mat_<double> cameraMatrix(3, 3);
  cameraMatrix << cols, 0, cols / 2.f, 0, cols, rows / 2.f, 0, 0, 1;

  // output
  cv::Mat rotationVector, translationVector;

  // solve
  cv::solvePnP(_referencePoints, cameraPoints, cameraMatrix, _distCoeffs,
               rotationVector, translationVector);

  // update result
  result.t0 = translationVector.at<double>(0, 0);
  result.t1 = translationVector.at<double>(0, 1);
  result.t2 = translationVector.at<double>(0, 2);

  result.r0 = rotationVector.at<double>(0, 0);
  result.r1 = rotationVector.at<double>(0, 1);
  result.r2 = rotationVector.at<double>(0, 2);

  result.m = (detection.part(62) - detection.part(66)).length();
  result.le = ((detection.part(37) - detection.part(40)).length() +
               (detection.part(38) - detection.part(41)).length()) /
              2.f;
  result.re = ((detection.part(43) - detection.part(46)).length() +
               (detection.part(44) - detection.part(47)).length()) /
              2.f;
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
