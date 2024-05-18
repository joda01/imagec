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
#include <vector>
#include "../../detection/detection.hpp"
#include "../detection_response.hpp"
#include "backend/helper/onnx_parser/onnx_parser.hpp"
#include "backend/settings/channel/channel_settings_filter.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/opencv.hpp>

namespace joda::image::detect::ai {

///
/// \class      ObjectDetector
/// \author     Joachim Danmayr
/// \brief      Object detector using ONNX model
///
class ObjectDetector : public DetectionFunction
{
public:
  /////////////////////////////////////////////////////
  ObjectDetector(const joda::settings::ChannelSettingsFilter &filt, const joda::onnx::OnnxParser::Data &model);
  auto forward(const cv::Mat &inputImage, const cv::Mat &originalImage, joda::settings::ChannelIndex channelIndex)
      -> DetectionResponse override;

private:
  // void paintBoundingBox(cv::Mat &inputImage, const DetectionResults &detection);
  /////////////////////////////////////////////////////
  auto postProcessing(const cv::Mat &inputImage, const cv::Mat &originalImage,
                      const std::vector<cv::Mat> &predictionMatrix, joda::settings::ChannelIndex channelIndex)
      -> DetectionResults;
  void drawLabel(cv::Mat &input_image, const std::string &label, int left, int top);
  /////////////////////////////////////////////////////
  const float INPUT_WIDTH          = 640.0;
  const float INPUT_HEIGHT         = 640.0;
  const float SCORE_THRESHOLD      = 0.1;    // 0.5
  const float NMS_THRESHOLD        = 0.3;    // 0.45    // To prevent double bounding boxes
  const float CONFIDENCE_THRESHOLD = 0.3;    // 0.45

  // Colors.
  const cv::Scalar BLUE   = cv::Scalar(255, 178, 50);
  const cv::Scalar BLACK  = cv::Scalar(0, 0, 0);
  const cv::Scalar YELLOW = cv::Scalar(0, 255, 255);
  const cv::Scalar WHITE  = cv::Scalar(255, 255, 255);
  const cv::Scalar RED    = cv::Scalar(0, 0, 255);
  const std::vector<std::string> mClassNames;
  cv::dnn::Net mNet;
};

}    // namespace joda::image::detect::ai
