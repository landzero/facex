// Copyright (c) 2018 LandZERO
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "capture.h"

#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/misc_api.h>
#include <dlib/opencv.h>

#define NUM_LANDMARKS 68
#define DOWNSCALE_RATIO 4

bool _isFaceDetectionValid(dlib::full_object_detection &det) {
  if (det.num_parts() != NUM_LANDMARKS)
    return false;
  for (size_t i = 0; i < det.num_parts(); i++)
    if (det.part(i) == dlib::OBJECT_PART_NOT_PRESENT)
      return false;
  return true;
}

bool _compareRectangleArea(dlib::rectangle lhs, dlib::rectangle rhs) {
  return lhs.area() < rhs.area();
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

void FX::Capture::SetResultStore(FX::ResultStore *resultStore) {
  _resultStore = resultStore;
}

void FX::Capture::Start() {
  _stop = false;
  // load face detector
  dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
  // load face predictor
  dlib::shape_predictor predictor;
  try {
    dlib::deserialize("res/shape_predictor_68_face_landmarks.dat") >> predictor;
  } catch (dlib::serialization_error &e) {
    std::cerr << "failed to load face landmarks model" << std::endl;
    std::cerr
        << "download url: "
           "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2"
        << std::endl;
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
  dlib::full_object_detection detection;

  // counter
  int count = 0;
  double t = (double)cv::getTickCount();

  // the result
  FX::Result result;

  // read first frame and allocate memory
  _cam >> im;
  cv::resize(im, imSmall, cv::Size(), 1.0 / DOWNSCALE_RATIO,
             1.0 / DOWNSCALE_RATIO);

  while (!_stop) {
    // initial tick count
    if (count == 0)
      t = cv::getTickCount();

    // read frame
    _cam >> im;

    // downscale image for face detection
    cv::resize(im, imSmall, cv::Size(), 1.0 / DOWNSCALE_RATIO,
               1.0 / DOWNSCALE_RATIO);

    // convert to dlib image, with copy
    dlib::cv_image<dlib::bgr_pixel> dim(im);
    dlib::cv_image<dlib::bgr_pixel> dimSmall(imSmall);

    // detect faces
    faces = detector(dimSmall);
    if (faces.size() == 0) {
      continue;
    }

    // find largest face
    face = *std::max_element(faces.begin(), faces.end(), _compareRectangleArea)
                .base();

    // upscale
    face.left() *= DOWNSCALE_RATIO;
    face.top() *= DOWNSCALE_RATIO;
    face.right() *= DOWNSCALE_RATIO;
    face.bottom() *= DOWNSCALE_RATIO;

    // detect face
    detection = predictor(dim, face);

    // broadcast
    if (_isFaceDetectionValid(detection)) {
      result.size.width = im.cols;
      result.size.height = im.rows;
      result.landmarks.clear();
      for (size_t i = 0; i < detection.num_parts(); i++) {
        cv::Point2d point(detection.part(i).x(), detection.part(i).y());
        result.landmarks.push_back(point);
      }
      if (_resultStore != nullptr) {
        _resultStore->Set(result);
      }
    }

    // calculate FPS
    count++;
    if (count == 100) {
      t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
      count = 0;
      std::cout << "camera: FPS " << 100.0 / t << std::endl;
    }
  }
}

void FX::Capture::Stop() { _stop = true; }
