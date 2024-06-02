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
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/image_processing/functions/threshold/threshold_li.hpp"
#include "backend/image_processing/functions/threshold/threshold_manual.hpp"
#include "backend/image_processing/functions/threshold/threshold_min_error.hpp"
#include "backend/image_processing/functions/threshold/threshold_moments.hpp"
#include "backend/image_processing/functions/threshold/threshold_otsu.hpp"
#include "backend/image_processing/functions/threshold/threshold_triangel.hpp"
#include "backend/settings/detection/detection_settings_threshold.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::image::segment {

ObjectSegmentation::ObjectSegmentation(const joda::settings::ChannelSettingsFilter &filt, uint16_t thresholdValue,
                                       joda::settings::ThresholdSettings::Mode method, bool doWatershed) :
    DetectionFunction(filt),
    mDoWatershed(doWatershed)
{
  switch(method) {
    case joda::settings::ThresholdSettings::Mode::LI:
      mThresoldMethod = std::make_shared<image::func::ThresholdLi>(thresholdValue);
      break;
    case joda::settings::ThresholdSettings::Mode::TRIANGLE:
      mThresoldMethod = std::make_shared<image::func::ThresholdTriangle>(thresholdValue);
      break;
    case joda::settings::ThresholdSettings::Mode::MIN_ERROR:
      mThresoldMethod = std::make_shared<image::func::ThresholdMinError>(thresholdValue);
      break;
    case joda::settings::ThresholdSettings::Mode::MOMENTS:
      mThresoldMethod = std::make_shared<image::func::ThresholdMoments>(thresholdValue);
      break;
    case joda::settings::ThresholdSettings::Mode::OTSU:
      mThresoldMethod = std::make_shared<image::func::ThresholdOtsu>(thresholdValue);
      break;

    default:
    case joda::settings::ThresholdSettings::Mode::HUANG:
    case joda::settings::ThresholdSettings::Mode::INTERMODES:
    case joda::settings::ThresholdSettings::Mode::ISODATA:
    case joda::settings::ThresholdSettings::Mode::MAX_ENTROPY:
    case joda::settings::ThresholdSettings::Mode::MEAN:
    case joda::settings::ThresholdSettings::Mode::MINIMUM:
    case joda::settings::ThresholdSettings::Mode::PERCENTILE:
    case joda::settings::ThresholdSettings::Mode::RENYI_ENTROPY:
    case joda::settings::ThresholdSettings::Mode::SHANBHAG:
    case joda::settings::ThresholdSettings::Mode::YEN:
      joda::log::logWarning("Not supported threshold algorithm selected. Using MANUAL as fallback.");
    case joda::settings::ThresholdSettings::Mode::MANUAL:
      mThresoldMethod = std::make_shared<image::func::ThresholdManual>(thresholdValue);
      break;
  }
}

auto ObjectSegmentation::forward(const cv::Mat &srcImg, const cv::Mat &originalImage,
                                 joda::settings::ChannelIndex channelIndex) -> image::detect::DetectionResponse
{
  auto id = DurationCount::start("ObjectSegmentation");

  cv::Mat binaryImage;
  uint16_t usedThersholdVal = mThresoldMethod->execute(srcImg, binaryImage);
  if(mDoWatershed) {
    joda::image::func::Watershed watershed;
    watershed.execute(binaryImage);
  }

  image::detect::DetectionResults response;

  // Find contours in the binary image

  auto fc = DurationCount::start("Find contours");
  binaryImage.convertTo(binaryImage, CV_8UC1);
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(binaryImage, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
  DurationCount::stop(fc);

  if(contours.size() > 50000) {
    joda::log::logWarning("Too much particles found >" + std::to_string(contours.size()) + "<, seems to be noise.");
    return {.result = response, .controlImage = {}};
  }

  cv::Mat boxMask = cv::Mat::zeros(binaryImage.size(), CV_8UC1);
  cv::fillPoly(boxMask, contours, cv::Scalar(255));

  auto ro = DurationCount::start("Add to ROI");

  // Create a mask for each contour and draw bounding boxes
  size_t idx = 0;
  size_t i   = 0;
  for(auto &contour : contours) {
    // Do not paint a contour for elements inside an element.
    // In other words if there is a particle with a hole, ignore the hole.
    // See https://docs.opencv.org/4.x/d9/d8b/tutorial_py_contours_hierarchy.html
    if(hierarchy[i][3] == -1) {
      auto boundingBox = cv::boundingRect(contour);
      cv::Mat mask     = cv::Mat::zeros(boundingBox.size(), CV_8UC1);
      // Bring the contours box in the area of the bounding box
      for(auto &point : contour) {
        point.x = point.x - boundingBox.x;
        point.y = point.y - boundingBox.y;
      }
      std::vector<std::vector<cv::Point>> contoursToPaint;
      contoursToPaint.push_back(contour);    // fillPoly expects a vector of contours
      fillPoly(mask, contoursToPaint, cv::Scalar(255));
      ROI detect(idx, usedThersholdVal, 0, boundingBox, mask, contour, originalImage, channelIndex,
                 getFilterSettings());
      idx++;
      response.push_back(detect);
    }
    i++;
  }
  DurationCount::stop(ro);

  cv::Mat grayImageFloat;
  srcImg.convertTo(grayImageFloat, CV_32F, (float) UCHAR_MAX / (float) UINT16_MAX);
  cv::Mat inputImage;
  cv::cvtColor(grayImageFloat, inputImage, cv::COLOR_GRAY2BGR);

  DurationCount::stop(id);
  return {.result = response, .controlImage = inputImage};
}
}    // namespace joda::image::segment
