///
/// \file      roi.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-30
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "roi.hpp"

namespace joda::func {

ROI::ROI(uint32_t index, Confidence confidence, ClassId classId, const Boxes &boundingBox, const cv::Mat &mask,
         const cv::Mat &imageOriginal, const joda::settings::json::ChannelFiltering *filter) :
    index(index),
    confidence(confidence), classId(classId), box(boundingBox), boxMask(mask)
{
  calculateMetrics(imageOriginal, filter);
}
ROI::ROI(uint32_t index, Confidence confidence, ClassId classId, const Boxes &boundingBox, const cv::Mat &mask,
         const cv::Mat &imageOriginal) :
    index(index),
    confidence(confidence), classId(classId), box(boundingBox), boxMask(mask)
{
  calculateMetrics(imageOriginal, nullptr);
}

///
/// \brief      Calculate metrics based on bounding box and mask
/// \author     Joachim Danmayr
///
void ROI::calculateMetrics(const cv::Mat &imageOriginal, const joda::settings::json::ChannelFiltering *filter)
{
  intensity    = 0;
  intensityMin = USHRT_MAX;
  intensityMax = 0;
  areaSize     = 0;
  circularity  = 0;

  // Calculate the intensity and area of the polygon ROI
  for(int x = 0; x < box.width; x++) {
    for(int y = 0; y < box.height; y++) {
      unsigned char maskPxl = boxMask.at<unsigned char>(y, x);    // Get the pixel value at (x, y)
      if(maskPxl > 0) {
        double pixelGrayScale = imageOriginal.at<unsigned short>(y, x);    // Get the pixel value at (x, y)
        if(pixelGrayScale < intensityMin) {
          intensityMin = pixelGrayScale;
        }
        if(pixelGrayScale > intensityMax) {
          intensityMax = pixelGrayScale;
        }
        intensity += pixelGrayScale;
        areaSize++;
      }
    }
  }
  float intensityAvg = 0.0f;
  if(areaSize > 0) {
    intensityAvg = intensity / static_cast<float>(areaSize);
  }
  intensity = intensityAvg;

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(boxMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  if(!contours.empty()) {
    double area      = cv::contourArea(contours[0]);
    double perimeter = cv::arcLength(contours[0], true);
    if(perimeter != 0) {
      circularity = (4 * M_PI * area) / (perimeter * perimeter);
    } else {
      circularity = 1;
    }
  }
  if(filter != nullptr) {
    applyParticleFilter(filter);
  } else {
    validity = ParticleValidity::VALID;
  }
}
///
/// \brief     Applies particle filter and sets the validity
///            based on the detection results
/// \author    Joachim Danmayr
///
void ROI::applyParticleFilter(const joda::settings::json::ChannelFiltering *filter)
{
  validity = ParticleValidity::UNKNOWN;
  if(areaSize > filter->getMaxParticleSize()) {
    validity = static_cast<ParticleValidity>(static_cast<int>(validity) | static_cast<int>(ParticleValidity::TOO_BIG));
  }
  if(areaSize < filter->getMinParticleSize()) {
    validity =
        static_cast<ParticleValidity>(static_cast<int>(validity) | static_cast<int>(ParticleValidity::TOO_SMALL));
  }
  if(circularity < filter->getMinCircularity()) {
    validity = static_cast<ParticleValidity>(static_cast<int>(validity) |
                                             static_cast<int>(ParticleValidity::TOO_LESS_CIRCULARITY));
  }
  if(validity == ParticleValidity::UNKNOWN) {
    validity = ParticleValidity::VALID;
  }

  // filter.getSnapAreaSize();
}
}    // namespace joda::func
