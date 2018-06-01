// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef __FX_CAPTURE_H__
#define __FX_CAPTURE_H__

#include "result.h"

#include <opencv2/opencv.hpp>

namespace FX {
class Capture {
public:
  Capture();

  ~Capture();

  void SetModelFile(std::string modelFile);

  void Open(int idx);

  bool IsOpened();

  void SetDebug(bool debug);

  void SetSize(double width, double height);

  void SetFPS(double fps);

  void SetResultStore(ResultStore *resultStore);

  void Start();

  void Stop();

private:
  // ResultStore to set and broadcast capture result
  ResultStore *_resultStore = nullptr;
  // debug mode, will show a opencv window
  bool _debug = false;
  // stop flag
  bool _stop = false;
  // cv::VideoCapture instance
  cv::VideoCapture _cam;
  // model file
  std::string _modelFile;
};
} // namespace FX

#endif
