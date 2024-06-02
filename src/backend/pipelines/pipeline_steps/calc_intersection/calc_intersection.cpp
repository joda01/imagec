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
/// \brief     A short description what happens here.
///

#include "calc_intersection.hpp"
#include <cstdint>
#include <string>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/image_processing/detection/detection.hpp"
#include "backend/image_processing/detection/object_segmentation/object_segmentation.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>

namespace joda::pipeline {

CalcIntersection::CalcIntersection(joda::settings::ChannelIndex channelIndexMe,
                                   const std::set<joda::settings::ChannelIndex> &indexesToIntersect,
                                   uint64_t minIntersection) :
    mChannelIndexMe(channelIndexMe),
    mIndexesToIntersect(indexesToIntersect.begin(), indexesToIntersect.end()), mMinIntersection(minIntersection)
{
}

auto CalcIntersection::execute(
    const settings::AnalyzeSettings &settings,
    const std::map<joda::settings::ChannelIndex, joda::image::detect::DetectionResponse> &detectionResultsIn) const
    -> joda::image::detect::DetectionResponse
{
  auto id = DurationCount::start("Intersection");

  if(mIndexesToIntersect.empty() || !detectionResultsIn.contains(*mIndexesToIntersect.begin())) {
    return joda::image::detect::DetectionResponse{};
  }

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
  joda::image::detect::DetectionResponse response = seg.forward(intersectingMask, originalImage, mChannelIndexMe);

  image::detect::DetectionFunction::paintBoundingBox(response.controlImage, response.result, {}, "#FFFF", false, false);

  DurationCount::stop(id);
  return response;
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
