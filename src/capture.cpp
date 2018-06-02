// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "capture.h"
#include "resolver.h"

#include <dlib/opencv.h>

#define DOWNSCALE_RATIO 4

bool _compareRectangleArea(dlib::rectangle lhs, dlib::rectangle rhs) { return lhs.area() < rhs.area(); }

void _cvDrawLine(cv::Mat &im, FX::Detection &det, size_t i, size_t j) { cv::line(im, det[i], det[j], cv::Scalar(255, 0, 0)); }

void _cvDrawLine(cv::Mat &im, FX::Detection &det, size_t i1, size_t i2, size_t j) { cv::line(im, (det[i1] + det[i2]) / 2, det[j], cv::Scalar(255, 0, 0)); }

void _cvDrawDetection(cv::Mat &im, FX::Detection &det, FX::Result &res) {
  (void)res;
  // mouth width
  _cvDrawLine(im, det, 60, 64);
  // mouth height
  _cvDrawLine(im, det, 62, 66);
  // nose size
  _cvDrawLine(im, det, 27, 30);
  // left eye
  _cvDrawLine(im, det, 37, 41);
  _cvDrawLine(im, det, 38, 40);
  // right eye
  _cvDrawLine(im, det, 43, 47);
  _cvDrawLine(im, det, 44, 46);
  // left eyebrow
  _cvDrawLine(im, det, 36, 39, 17);
  _cvDrawLine(im, det, 36, 39, 18);
  _cvDrawLine(im, det, 36, 39, 19);
  _cvDrawLine(im, det, 36, 39, 20);
  _cvDrawLine(im, det, 36, 39, 21);
  // right eyebrow
  _cvDrawLine(im, det, 42, 45, 22);
  _cvDrawLine(im, det, 42, 45, 23);
  _cvDrawLine(im, det, 42, 45, 24);
  _cvDrawLine(im, det, 42, 45, 25);
  _cvDrawLine(im, det, 42, 45, 26);
  // all points
  for (size_t i = 0; i < det.Count(); i++) {
    cv::Point2d p = det[i];
    cv::circle(im, p, 2, cv::Scalar(255, 0, 0), cv::FILLED);
  }
}

FX::Capture::Capture() {}

FX::Capture::~Capture() {}

void FX::Capture::Open(int idx) { _cam.open(idx); }

bool FX::Capture::IsOpened() { return _cam.isOpened(); }

void FX::Capture::SetSize(double width, double height) {
  _cam.set(cv::CAP_PROP_FRAME_WIDTH, width);
  _cam.set(cv::CAP_PROP_FRAME_HEIGHT, height);
}

void FX::Capture::SetFPS(double fps) { _cam.set(cv::CAP_PROP_FPS, fps); }

void FX::Capture::SetResultStore(FX::ResultStore *resultStore) { _resultStore = resultStore; }

void FX::Capture::Start() {
  _stop = false;
  // load face detector
  dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
  // load face predictor
  dlib::shape_predictor predictor;
  try {
    dlib::deserialize(_modelFile) >> predictor;
  } catch (dlib::serialization_error &e) {
    std::cerr << "failed to load face landmarks model" << std::endl;
    std::cerr << e.what() << std::endl;
    abort();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    abort();
  }

  // variables
  cv::Mat im, imSmall;
  std::vector<dlib::rectangle> faces;
  dlib::rectangle face;
  dlib::full_object_detection rawDet;

  // resolver
  FX::Detection det;
  FX::Resolver resolver;

  // counter
  int count = 0;
  double t = cv::getTickCount();

  // the result
  FX::Result result;

  // read first frame and allocate memory
  _cam >> im;
  cv::resize(im, imSmall, cv::Size(), 1.0 / DOWNSCALE_RATIO, 1.0 / DOWNSCALE_RATIO);
  std::cout << "capture: size " << im.cols << "x" << im.rows << std::endl;

  // window
  if (_debug)
    cv::namedWindow("FaceX", cv::WINDOW_AUTOSIZE);

  while (!_stop) {
    // initial tick count
    if (count == 0)
      t = cv::getTickCount();

    // read frame
    _cam >> im;

    // downscale image for face detection
    cv::resize(im, imSmall, cv::Size(), 1.0 / DOWNSCALE_RATIO, 1.0 / DOWNSCALE_RATIO);

    // convert to dlib image, with copy
    dlib::cv_image<dlib::bgr_pixel> dim(im);
    dlib::cv_image<dlib::bgr_pixel> dimSmall(imSmall);

    // detect faces
    faces = detector(dimSmall);
    if (faces.empty()) {
      goto loop_end;
    }

    // find largest face
    face = *std::max_element(faces.begin(), faces.end(), _compareRectangleArea).base();

    // upscale
    face.left() *= DOWNSCALE_RATIO;
    face.top() *= DOWNSCALE_RATIO;
    face.right() *= DOWNSCALE_RATIO;
    face.bottom() *= DOWNSCALE_RATIO;

    // detect face
    rawDet = predictor(dim, face);
    det = FX::Detection(rawDet);

    // resolve the detection and broadcast
    if (resolver.Resolve(im.cols, im.rows, det, result)) {
      if (_resultStore != nullptr) {
        _resultStore->Set(result);
      }
      if (_debug) {
        _cvDrawDetection(im, det, result);
        cv::imshow("FaceX", im);
        cv::waitKey(1);
      }
    }

  loop_end:
    // calculate FPS
    count++;
    if (count == 100) {
      t = (cv::getTickCount() - t) / cv::getTickFrequency();
      count = 0;
      std::cout << "capture: FPS " << 100.f / t << std::endl;
    }
  }
}

void FX::Capture::Stop() { _stop = true; }

void FX::Capture::SetModelFile(std::string modelFile) { _modelFile = modelFile; }

void FX::Capture::SetDebug(bool debug) { _debug = debug; }
