///
/// \file      ai_object_detection.h
/// \author    Joachim Danmayr
/// \date      2023-02-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include <string>
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/opencv.hpp>

namespace ai {

struct DetectionResult
{
  std::vector<cv::Rect> boxes;
  std::vector<float> confidences;
  std::vector<int> classId;
};

///
/// \class      ObjectDetector
/// \author     Joachim Danmayr
/// \brief      Object detector using ONNX model
///
class ObjectDetector
{
public:
  /////////////////////////////////////////////////////
  ObjectDetector(const std::string &onnxNet, const std::vector<std::string> &classNames);
  auto forward(const cv::Mat &inputImage) -> DetectionResult;
  void paintBoundingBox(cv::Mat &inputImage, const DetectionResult &detection);

private:
  /////////////////////////////////////////////////////
  auto postProcessing(const cv::Mat &inputImage, std::vector<cv::Mat> &outputs) -> DetectionResult;
  void drawLabel(cv::Mat &input_image, std::string label, int left, int top);
  /////////////////////////////////////////////////////
  const float INPUT_WIDTH          = 640.0;
  const float INPUT_HEIGHT         = 640.0;
  const float SCORE_THRESHOLD      = 0.1;    // 0.5
  const float NMS_THRESHOLD        = 0.3;    // 0.45    // To prevent double bounding boxes
  const float CONFIDENCE_THRESHOLD = 0.1;    // 0.45
  const int THICKNESS              = 1;

  // Text parameters.
  const float FONT_SCALE = 0.7;
  const int FONT_FACE    = cv::FONT_HERSHEY_SIMPLEX;

  // Colors.
  const cv::Scalar BLUE   = cv::Scalar(255, 178, 50);
  const cv::Scalar BLACK  = cv::Scalar(0, 0, 0);
  const cv::Scalar YELLOW = cv::Scalar(0, 255, 255);
  const cv::Scalar RED    = cv::Scalar(0, 0, 255);
  const std::vector<std::string> mClassNames;
  cv::dnn::Net mNet;
};

}    // namespace ai
