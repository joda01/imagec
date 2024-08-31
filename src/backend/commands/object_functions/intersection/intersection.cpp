///
/// \file      Intersection.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "intersection.hpp"
#include <cstddef>
#include <optional>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"

namespace joda::cmd {

Intersection::Intersection(const settings::IntersectionSettings &settings) : mSettings(settings)
{
}

void Intersection::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &resultIn)
{
  auto &objectsInOut = context.loadObjectsFromCache(mSettings.objectsIn.objectIn)
                           ->at(context.getClusterId(mSettings.objectsIn.clusterIn));
  const auto &intersectWith = context.loadObjectsFromCache(mSettings.objectsInWith.objectIn)
                                  ->at(context.getClusterId(mSettings.objectsInWith.clusterIn));

  objectsInOut.calcIntersections(mSettings.mode, intersectWith, mSettings.objectsIn.classesIn,
                                 mSettings.objectsInWith.classesIn, mSettings.minIntersection, mSettings.newClassId);
}
/*
cv::Mat intersectingMask =
    cv::Mat(detectionResultsIn.at(*clustersToIntersect.begin()).controlImage.size(), CV_8UC1, cv::Scalar(255));
cv::Mat originalImage =
    cv::Mat::ones(detectionResultsIn.at(*clustersToIntersect.begin()).controlImage.size(), CV_16UC1) * 65535;

for(const auto idxToIntersect : clustersToIntersect) {
  if(detectionResultsIn.contains(idxToIntersect)) {
    cv::Mat binaryImage = cv::Mat::zeros(detectionResultsIn.at(idxToIntersect).originalImage.size(), CV_8UC1);
    detectionResultsIn.at(idxToIntersect).result.createBinaryImage(binaryImage);
    cv::bitwise_and(intersectingMask, binaryImage, intersectingMask);
    // Calculate the Intersection of the original images
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
// Calculate the Intersection
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

}    // namespace joda::cmd
