///
/// \file      threshold.cpp
/// \author    Joachim Danmayr
/// \date      2023-07-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "threshold.hpp"
#include <climits>
#include <cstdint>
#include <string>
#include "image_processing/functions/func_types.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

namespace joda::func::threshold {

ObjectSegmentation::ObjectSegmentation(uint16_t thresholdValue) : mThresholdValue(thresholdValue)
{
}

auto ObjectSegmentation::forward(const cv::Mat &srcImg) -> DetectionResponse
{
  cv::Mat binaryImage;
  cv::threshold(srcImg, binaryImage, mThresholdValue, UINT16_MAX, cv::THRESH_BINARY);
  DetectionResults response;

  cv::Mat grayImageFloat;
  srcImg.convertTo(grayImageFloat, CV_32F, (float) UCHAR_MAX / (float) UINT16_MAX);
  cv::Mat inputImage;
  cv::cvtColor(grayImageFloat, inputImage, cv::COLOR_GRAY2BGR);

  // Find contours in the binary image
  binaryImage.convertTo(binaryImage, CV_8UC1);
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(binaryImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  // Create a mask for each contour and draw bounding boxes
  for(size_t i = 0; i < contours.size(); ++i) {
    Detection detect;
    detect.index      = i;
    detect.confidence = 1;

    // Find the bounding box for the contour
    detect.box = cv::boundingRect(contours[i]);

    // Create a mask for the current contour
    detect.boxMask = cv::Mat::zeros(binaryImage.size(), CV_8UC1);
    cv::drawContours(detect.boxMask, contours, static_cast<int>(i), cv::Scalar(UCHAR_MAX), cv::FILLED);
    detect.boxMask = detect.boxMask(detect.box);

    calculateMetrics(detect, srcImg, detect.box, detect.boxMask);
    response.push_back(detect);
  }

  paintBoundingBox(inputImage, response);

  return {.result = response, .controlImage = inputImage};
}
}    // namespace joda::func::threshold
