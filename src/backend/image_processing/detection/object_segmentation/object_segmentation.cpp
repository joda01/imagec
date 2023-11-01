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

#include "object_segmentation.hpp"
#include <climits>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include "../detection.hpp"
#include "../detection_response.hpp"
#include "backend/duration_count/duration_count.h"
#include "backend/image_processing/functions/threshold/threshold_li.hpp"
#include "backend/image_processing/functions/threshold/threshold_manual.hpp"
#include "backend/image_processing/functions/threshold/threshold_min_error.hpp"
#include "backend/image_processing/functions/threshold/threshold_triangel.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::func::threshold {

ObjectSegmentation::ObjectSegmentation(const joda::settings::json::ChannelFiltering &filt, uint16_t thresholdValue,
                                       joda::settings::json::ThresholdSettings::Threshold method) :
    DetectionFunction(&filt)
{
  switch(method) {
    case joda::settings::json::ThresholdSettings::Threshold::LI:
      mThresoldMethod = std::make_shared<img::ThresholdLi>(thresholdValue);
      break;
    case joda::settings::json::ThresholdSettings::Threshold::TRIANGLE:
      mThresoldMethod = std::make_shared<img::ThresholdTriangle>(thresholdValue);
      break;
    case joda::settings::json::ThresholdSettings::Threshold::MIN_ERROR:
      mThresoldMethod = std::make_shared<img::ThresholdMin>(thresholdValue);
      break;
    default:
    case joda::settings::json::ThresholdSettings::Threshold::MANUAL:
      mThresoldMethod = std::make_shared<img::ThresholdManual>(thresholdValue);
      break;
  }
}

auto ObjectSegmentation::forward(const cv::Mat &srcImg, const cv::Mat &originalImage) -> DetectionResponse
{
  cv::Mat binaryImage;
  uint16_t usedThersholdVal = mThresoldMethod->execute(srcImg, binaryImage);

  DetectionResults response;
  cv::Mat grayImageFloat;
  srcImg.convertTo(grayImageFloat, CV_32F, (float) UCHAR_MAX / (float) UINT16_MAX);
  cv::Mat inputImage;
  cv::cvtColor(grayImageFloat, inputImage, cv::COLOR_GRAY2BGR);

  // Find contours in the binary image
  auto id = DurationCount::start("detection_find");

  binaryImage.convertTo(binaryImage, CV_8UC1);
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(binaryImage, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
  DurationCount::stop(id);

  if(contours.size() > 100000) {
    // throw std::runtime_error("Too much spots.");
    std::cout << "Too much: " << std::to_string(contours.size()) << std::endl;
  }
  id = DurationCount::start("detection_mask");

  cv::Mat boxMask = cv::Mat::zeros(binaryImage.size(), CV_8UC1);
  cv::fillPoly(boxMask, contours, cv::Scalar(255));

  // Create a mask for each contour and draw bounding boxes
  for(size_t i = 0; i < contours.size(); ++i) {
    // Find the bounding box for the contour
    auto box     = cv::boundingRect(contours[i]);
    cv::Mat mask = boxMask(box) >= 1;
    ROI detect(i, usedThersholdVal, 0, box, mask, originalImage, getFilterSettings());
    response.push_back(detect);
  }
  DurationCount::stop(id);

  id = DurationCount::start("detection_paint");

  paintBoundingBox(inputImage, response);
  DurationCount::stop(id);

  return {.result = response, .controlImage = inputImage};
}
}    // namespace joda::func::threshold
