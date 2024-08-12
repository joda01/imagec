///
/// \file      calc_intersection.cpp
/// \author    Joachim Danmayr
/// \date      2023-09-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "calc_intersection.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/image_processing/detection/detection.hpp"
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/image_processing/detection/object_segmentation/object_segmentation.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>

namespace joda::pipeline {

CalcIntersection::CalcIntersection(joda::settings::ChannelIndex channelIndexMe,
                                   const std::set<joda::settings::ChannelIndex> &indexesToIntersect,
                                   float minIntersection) :
    mChannelIndexMe(channelIndexMe),
    mIndexesToIntersect(indexesToIntersect.begin(), indexesToIntersect.end()), mMinIntersection(minIntersection)
{
}

auto CalcIntersection::execute(
    const settings::AnalyzeSettings &settings,
    const std::map<joda::settings::ChannelIndex, image::detect::DetectionResponse> &detectionResultsIn) const
    -> image::detect::DetectionResponse
{
  auto id = DurationCount::start("Intersection");

  if(mIndexesToIntersect.empty() || !detectionResultsIn.contains(*mIndexesToIntersect.begin())) {
    return image::detect::DetectionResponse{};
  }
  auto it = mIndexesToIntersect.begin();

  image::detect::DetectionResponse response{.result               = detectionResultsIn.at(*it).result->clone(),
                                            .originalImage        = {},
                                            .responseValidity     = {},
                                            .invalidateWholeImage = false};

  std::map<joda::settings::ChannelIndex, const cv::Mat *> channelsToIntersectImages;

  for(const auto idxToIntersect : mIndexesToIntersect) {
    if(detectionResultsIn.contains(idxToIntersect)) {
      channelsToIntersectImages.emplace(idxToIntersect, &detectionResultsIn.at(idxToIntersect).originalImage);
    }
  }

  if(it != mIndexesToIntersect.end()) {
    ++it;
  }
  for(; it != mIndexesToIntersect.end(); ++it) {
    if(detectionResultsIn.contains(*it)) {
      const auto &element = detectionResultsIn.at(*it);
      response.result = response.result->calcIntersections(element.result, channelsToIntersectImages, mMinIntersection);

      // detectionResultsIn.at(idxToIntersect).
    }
  }

  DurationCount::stop(id);

  return response;
}
/*
cv::Mat intersectingMask =
    cv::Mat(detectionResultsIn.at(*mIndexesToIntersect.begin()).controlImage.size(), CV_8UC1, cv::Scalar(255));
cv::Mat originalImage =
    cv::Mat::ones(detectionResultsIn.at(*mIndexesToIntersect.begin()).controlImage.size(), CV_16UC1) * 65535;

for(const auto idxToIntersect : mIndexesToIntersect) {
  if(detectionResultsIn.contains(idxToIntersect)) {
    cv::Mat binaryImage = cv::Mat::zeros(detectionResultsIn.at(idxToIntersect).originalImage.size(), CV_8UC1);
    detectionResultsIn.at(idxToIntersect).result.createBinaryImage(binaryImage);
    cv::bitwise_and(intersectingMask, binaryImage, intersectingMask);
    // Calculate the intersection of the original images
    originalImage = cv::min(detectionResultsIn.at(idxToIntersect).originalImage, originalImage);
  }
}

joda::settings::ChannelSettingsFilter filter;
filter.maxParticleSize = INT64_MAX;
filter.minParticleSize = mMinIntersection;    ///\todo Add filtering
filter.snapAreaSize    = 0;
filter.minCircularity  = 0;
joda::image::segment::ObjectSegmentation seg(filter, 200, joda::settings::ThresholdSettings::Mode::MANUAL, false);
std::unique_ptr<image::detect::DetectionResponse> response = seg.forward(intersectingMask,
originalImage, mChannelIndexMe);

image::detect::DetectionFunction::paintBoundingBox(response.controlImage, response.result, {}, "#FFFF", false, false);

DurationCount::stop(id);
return response;
* /
}

//
// Calculate the intersection
//
/*
std::vector<image::detect::DetectionFunction::OverlaySettings> overlayPainting;
overlayPainting.push_back({.result          = &channelsToIntersect[0]->result,
                         .backgroundColor = cv::Scalar(255, 0, 0),
                         .borderColor     = cv::Scalar(0, 0, 0),
                         .paintRectangel  = false,
                         .opaque          = 0.3});


overlayPainting.push_back({.result          = &ch1->result,
                           .backgroundColor = cv::Scalar(0, 255, 0),
                           .borderColor     = cv::Scalar(0, 0, 0),
                           .paintRectangel  = false,
                           .opaque          = 0.3});
*/

/*overlayPainting.insert(overlayPainting.begin(),
                       image::detect::DetectionFunction::OverlaySettings{.result          = &response.result,
                                                                         .backgroundColor = cv::Scalar(0, 0, 255),
                                                                         .borderColor     = cv::Scalar(0, 0, 0),
                                                                         .paintRectangel  = false,
                                                                         .opaque          = 1});*/

// response.controlImage =
//     cv::Mat::zeros(channelsToIntersect[0]->originalImage.rows, channelsToIntersect[0]->originalImage.cols,
//     CV_32FC3);

// joda::image::detect::DetectionFunction::paintOverlay(response.controlImage, overlayPainting);

}    // namespace joda::pipeline
