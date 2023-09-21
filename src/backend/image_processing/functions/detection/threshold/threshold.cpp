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
#include <stdexcept>
#include <string>
#include "../../../functions/detection/detection.hpp"
#include "../../../functions/func_types.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::func::threshold {

ObjectSegmentation::ObjectSegmentation(const joda::settings::json::ChannelFiltering &filt, uint16_t thresholdValue) :
    DetectionFunction(&filt), mThresholdValue(thresholdValue)
{
}

auto ObjectSegmentation::forward(const cv::Mat &srcImg, const cv::Mat &originalImage) -> DetectionResponse
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
  cv::findContours(binaryImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

  if(contours.size() > 100000) {
    throw std::runtime_error("Too much spots.");
  }
  // Create a mask for each contour and draw bounding boxes
  for(size_t i = 0; i < contours.size(); ++i) {
    // Find the bounding box for the contour
    auto box = cv::boundingRect(contours[i]);
    // Create a mask for the current contour
    cv::Mat boxMask = cv::Mat::zeros(binaryImage.size(), CV_8UC1);
    cv::drawContours(boxMask, contours, static_cast<int>(i), cv::Scalar(UCHAR_MAX), cv::FILLED);
    boxMask = boxMask(box) >= 1;
    ROI detect(i, 1, 0, box, boxMask, originalImage, getFilterSettings());
    response.push_back(detect);
  }

  paintBoundingBox(inputImage, response);

  return {.result = response, .controlImage = inputImage};
}
}    // namespace joda::func::threshold
