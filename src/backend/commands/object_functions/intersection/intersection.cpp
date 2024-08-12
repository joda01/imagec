///
/// \file      intersection.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "intersection.hpp"
#include <cstddef>
#include <optional>
#include "backend/global_enums.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/processor/process_step.hpp"

namespace joda::cmd::functions {

Intersection::Intersection(const IntersectionSettings &settings) : mSettings(settings)
{
}

void Intersection::execute(processor::ProcessContext &context, processor::ProcessorMemory &memory, cv::Mat &image,
                           ObjectsListMap &result)
{
  auto id = DurationCount::start("Intersection");

  const auto &indexesToIntersect = mSettings.intersectingSlots;
  size_t intersectCount          = indexesToIntersect.size();
  try {
    std::map<joda::enums::ImageChannelIndex, const cv::Mat *> channelsToIntersectImages;
    for(const auto [idxToIntersect, _] : indexesToIntersect) {
      auto channel = memory.load(idxToIntersect).context();
      channelsToIntersectImages.emplace(channel.channel, &channel.originalImage);
    }

    int idx = 0;
    auto it = indexesToIntersect.begin();
    if(it == indexesToIntersect.end()) {
      return;
    }

    if(intersectCount == 1) {
      result.cloneFromOther(memory.load(it->first).objects());
      DurationCount::stop(id);
      return;
    }

    const auto *firstDataBuffer     = &memory.load(it->first).objects();
    const roi::SpatialHash *working = firstDataBuffer;
    roi::SpatialHash *resultTemp    = nullptr;
    // Directly write to the output buffer
    roi::SpatialHash buffer01;
    roi::SpatialHash buffer02;
    if(intersectCount == 2) {
      resultTemp = &result;
    } else {
      resultTemp = &buffer01;
    }

    std::optional<std::set<joda::enums::ObjectClassId>> objectClassesMe = it->second.objectClasses;

    ++it;
    ++idx;

    for(; it != indexesToIntersect.end(); ++it) {
      const auto &objects02 = memory.load(it->first).objects();
      working->calcIntersections(objects02, *resultTemp, channelsToIntersectImages, objectClassesMe,
                                 it->second.objectClasses, mSettings.objectClass, mSettings.minIntersection);
      // In the second run, we have to ignore the object class filter of me, because this are still the filtered objects
      objectClassesMe.reset();
      idx++;
      if(idx >= intersectCount) {
        break;
      }
      const auto *tmpWorking = working;
      working                = resultTemp;
      if(idx + 1 >= intersectCount) {
        resultTemp = &result;
      } else {
        if(tmpWorking == firstDataBuffer) {
          // In the first run the working pointer was the loaded data we must change to buffer
          resultTemp = &buffer02;
        } else {
          // Swap the buffer. We know what  we do.
          resultTemp = const_cast<roi::SpatialHash *>(tmpWorking);
        }
      }
      resultTemp->clear();
    }

  } catch(const std::exception &) {
    joda::log::logWarning("Slot does not exist!");
  }

  DurationCount::stop(id);
}
/*
cv::Mat intersectingMask =
    cv::Mat(detectionResultsIn.at(*indexesToIntersect.begin()).controlImage.size(), CV_8UC1, cv::Scalar(255));
cv::Mat originalImage =
    cv::Mat::ones(detectionResultsIn.at(*indexesToIntersect.begin()).controlImage.size(), CV_16UC1) * 65535;

for(const auto idxToIntersect : indexesToIntersect) {
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

}    // namespace joda::cmd::functions
