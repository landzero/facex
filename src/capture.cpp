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

bool _IsFaceDetectionValid(dlib::full_object_detection &det) {
  if (det.num_parts() != NUM_LANDMARKS)
    return false;
  for (size_t i = 0; i < det.num_parts(); i++)
    if (det.part(i) == dlib::OBJECT_PART_NOT_PRESENT)
      return false;
  return true;
}

bool _CompareRectangleArea(dlib::rectangle lhs, dlib::rectangle rhs) {
  return lhs.area() < rhs.area();
}

std::vector<cv::Point3d> _ReferencePosePoints() {
  std::vector<cv::Point3d> points;
  points.push_back(cv::Point3d(
      0.0f, 0.0f, 0.0f)); // The first must be (0,0,0) while using POSIT
  points.push_back(cv::Point3d(0.0f, -330.0f, -65.0f));
  points.push_back(cv::Point3d(-225.0f, 170.0f, -135.0f));
  points.push_back(cv::Point3d(225.0f, 170.0f, -135.0f));
  points.push_back(cv::Point3d(-150.0f, -150.0f, -125.0f));
  points.push_back(cv::Point3d(150.0f, -150.0f, -125.0f));
  return points;
}

std::vector<cv::Point2d> _ExtractCameraPoints(dlib::full_object_detection &d) {
  std::vector<cv::Point2d> points;
  points.push_back(cv::Point2d(d.part(30).x(), d.part(30).y())); // Nose tip
  points.push_back(cv::Point2d(d.part(8).x(), d.part(8).y()));   // Chin
  points.push_back(
      cv::Point2d(d.part(36).x(), d.part(36).y())); // Left eye left corner
  points.push_back(
      cv::Point2d(d.part(45).x(), d.part(45).y())); // Right eye right corner
  points.push_back(
      cv::Point2d(d.part(48).x(), d.part(48).y())); // Left Mouth corner
  points.push_back(
      cv::Point2d(d.part(54).x(), d.part(54).y())); // Right mouth corner
  return points;
}

cv::Mat _ExtractCameraMatrix(float focalLength, cv::Point2d center) {
  return (cv::Mat_<double>(3, 3) << focalLength, 0, center.x, 0, focalLength,
          center.y, 0, 0, 1);
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
  std::vector<cv::Point3d> referencePoints = _ReferencePosePoints();
  std::vector<cv::Point2d> cameraPoints;
  cv::Mat cameraMatrix;
  cv::Mat distCoeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type);
  cv::Mat rotationVector;
  cv::Mat rotationMatrix;
  cv::Mat translationVector;

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
    if (faces.empty()) {
      goto loop_end;
    }

    // find largest face
    face = *std::max_element(faces.begin(), faces.end(), _CompareRectangleArea)
                .base();

    // upscale
    face.left() *= DOWNSCALE_RATIO;
    face.top() *= DOWNSCALE_RATIO;
    face.right() *= DOWNSCALE_RATIO;
    face.bottom() *= DOWNSCALE_RATIO;

    // detect face
    detection = predictor(dim, face);

    // calculate head pose
    if (_IsFaceDetectionValid(detection)) {
      cameraPoints = _ExtractCameraPoints(detection);
      cameraMatrix = _ExtractCameraMatrix(
          im.cols, cv::Point2d(im.cols / 2.f, im.rows / 2.f));

      cv::solvePnP(referencePoints, cameraPoints, cameraMatrix, distCoeffs,
                   rotationVector, translationVector);

      result.width = im.cols;
      result.height = im.rows;
      result.t0 = translationVector.at<double>(0, 0);
      result.t1 = translationVector.at<double>(0, 1);
      result.t2 = translationVector.at<double>(0, 2);

      result.r0 = rotationVector.at<double>(0, 0);
      result.r1 = rotationVector.at<double>(0, 1);
      result.r2 = rotationVector.at<double>(0, 2);

      if (_resultStore != nullptr) {
        _resultStore->Set(result);
      }
    }

  loop_end:
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
