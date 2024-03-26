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
#include "backend/image_processing/functions/threshold/threshold_moments.hpp"
#include "backend/image_processing/functions/threshold/threshold_triangel.hpp"
#include "backend/logger/console_logger.hpp"
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
      mThresoldMethod = std::make_shared<img::ThresholdMinError>(thresholdValue);
      break;
    case joda::settings::json::ThresholdSettings::Threshold::MOMENTS:
      mThresoldMethod = std::make_shared<img::ThresholdMoments>(thresholdValue);
      break;
    default:
    case joda::settings::json::ThresholdSettings::Threshold::HUANG:
    case joda::settings::json::ThresholdSettings::Threshold::INTERMODES:
    case joda::settings::json::ThresholdSettings::Threshold::ISODATA:
    case joda::settings::json::ThresholdSettings::Threshold::MAX_ENTROPY:
    case joda::settings::json::ThresholdSettings::Threshold::MEAN:
    case joda::settings::json::ThresholdSettings::Threshold::MINIMUM:
    case joda::settings::json::ThresholdSettings::Threshold::OTSU:
    case joda::settings::json::ThresholdSettings::Threshold::PERCENTILE:
    case joda::settings::json::ThresholdSettings::Threshold::RENYI_ENTROPY:
    case joda::settings::json::ThresholdSettings::Threshold::SHANBHAG:
    case joda::settings::json::ThresholdSettings::Threshold::YEN:
      joda::log::logWarning("Not supported threshold algorithm selected. Using MANUAL as fallback.");
    case joda::settings::json::ThresholdSettings::Threshold::MANUAL:
      mThresoldMethod = std::make_shared<img::ThresholdManual>(thresholdValue);
      break;
  }
}

auto ObjectSegmentation::forward(const cv::Mat &srcImg, const cv::Mat &originalImage, int32_t channelIndex)
    -> DetectionResponse
{
  cv::Mat binaryImage;
  uint16_t usedThersholdVal = mThresoldMethod->execute(srcImg, binaryImage);

  DetectionResults response;

  // Find contours in the binary image
  auto id = DurationCount::start("detection_find");

  binaryImage.convertTo(binaryImage, CV_8UC1);
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(binaryImage, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
  DurationCount::stop(id);

  if(contours.size() > 100000) {
    joda::log::logWarning("Too much particles found >" + std::to_string(contours.size()) + "<, seems to be noise.");
  }
  id = DurationCount::start("detection_mask");

  cv::Mat boxMask = cv::Mat::zeros(binaryImage.size(), CV_8UC1);
  cv::fillPoly(boxMask, contours, cv::Scalar(255));

  // Create a mask for each contour and draw bounding boxes
  size_t idx = 0;
  for(size_t i = 0; i < contours.size(); ++i) {
    // Do not paint a contour for elements inside an element.
    // In other words if there is a particle with a hole, ignore the hole.
    // See https://docs.opencv.org/4.x/d9/d8b/tutorial_py_contours_hierarchy.html
    if(hierarchy[i][3] == -1) {
      // Find the bounding box for the contour
      auto box     = cv::boundingRect(contours[i]);
      cv::Mat mask = boxMask(box) >= 1;

      // Bring the contours box in the area of the bounding box
      for(auto &point : contours[i]) {
        point.x = point.x - box.x;
        point.y = point.y - box.y;
      }

      ROI detect(idx, usedThersholdVal, 0, box, mask, contours[i], originalImage, channelIndex, getFilterSettings());
      idx++;
      response.push_back(detect);
    }
  }
  DurationCount::stop(id);

  cv::Mat grayImageFloat;
  srcImg.convertTo(grayImageFloat, CV_32F, (float) UCHAR_MAX / (float) UINT16_MAX);
  cv::Mat inputImage;
  cv::cvtColor(grayImageFloat, inputImage, cv::COLOR_GRAY2BGR);
  return {.result = response, .controlImage = inputImage};
}
}    // namespace joda::func::threshold
