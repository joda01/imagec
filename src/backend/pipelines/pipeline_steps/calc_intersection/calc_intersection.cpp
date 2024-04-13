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
#include <string>
#include "backend/duration_count/duration_count.h"
#include "backend/image_processing/detection/detection.hpp"
#include <opencv2/core.hpp>

namespace joda::pipeline {

CalcIntersection::CalcIntersection(const std::set<joda::settings::ChannelIndex> &indexesToIntersect,
                                   float minIntersection) :
    mIndexesToIntersect(indexesToIntersect.begin(), indexesToIntersect.end()),
    mMinIntersection(minIntersection)
{
}

auto CalcIntersection::execute(
    const settings::AnalyzeSettings &settings,
    const std::map<joda::settings::ChannelIndex, joda::func::DetectionResponse> &detectionResultsIn,
    const std::string &detailoutputPath) const -> joda::func::DetectionResponse
{
  if(mIndexesToIntersect.empty() || !detectionResultsIn.contains(*mIndexesToIntersect.begin())) {
    return joda::func::DetectionResponse{};
  }

  std::vector<const joda::func::DetectionResponse *> channelsToIntersect;
  std::map<joda::settings::ChannelIndex, const cv::Mat *> channelsToIntersectImages;

  joda::settings::ChannelIndex idx1 = *mIndexesToIntersect.begin();
  joda::settings::ChannelIndex idx2 = *(std::next(mIndexesToIntersect.begin()));

  for(const auto idxToIntersect : mIndexesToIntersect) {
    if(detectionResultsIn.contains(idxToIntersect)) {
      channelsToIntersect.push_back(&detectionResultsIn.at(idxToIntersect));
      channelsToIntersectImages.emplace(idxToIntersect, &detectionResultsIn.at(idxToIntersect).originalImage);
    }
  }

  // Sort in descending order (largest first)
  auto compareByX = [](const joda::func::DetectionResponse *a, const joda::func::DetectionResponse *b) -> bool {
    {
      return a->result.size() > b->result.size();
    }
  };

  std::sort(channelsToIntersect.begin(), channelsToIntersect.end(), compareByX);

  //
  // Calculate the intersection
  //
  std::vector<func::DetectionFunction::OverlaySettings> overlayPainting;
  joda::func::DetectionResponse response;
  response = *channelsToIntersect[0];
  overlayPainting.push_back({.result          = &channelsToIntersect[0]->result,
                             .backgroundColor = cv::Scalar(255, 0, 0),
                             .borderColor     = cv::Scalar(0, 0, 0),
                             .paintRectangel  = false,
                             .opaque          = 0.3});

  auto id = DurationCount::start("intersect");
  for(auto n = 1; n < channelsToIntersect.size(); n++) {
    const auto *ch1 = channelsToIntersect[n];

    overlayPainting.push_back({.result          = &ch1->result,
                               .backgroundColor = cv::Scalar(0, 255, 0),
                               .borderColor     = cv::Scalar(0, 0, 0),
                               .paintRectangel  = false,
                               .opaque          = 0.3});

    joda::func::DetectionResponse respTmp;
    for(auto const &roi01 : response.result) {
      for(auto const &roi02 : ch1->result) {
        if(roi01.isValid() && roi02.isValid()) {
          auto [colocROI, ok] = roi01.calcIntersection(roi02, channelsToIntersectImages, mMinIntersection);
          // We only log the first occurency of intersestion. Intersection over more particles is not logged yet
          if(ok) {
            respTmp.result.push_back(colocROI);
            break;
          }
        }
      }
    }

    response = respTmp;
  }

  //
  // Calculate the intersection of the original images
  //
  response.originalImage = cv::Mat::ones(channelsToIntersectImages.begin()->second->rows,
                                         channelsToIntersectImages.begin()->second->cols, CV_16UC1) *
                           65535;
  for(const auto &img : channelsToIntersectImages) {
    response.originalImage = cv::min(*img.second, response.originalImage);
  }

  DurationCount::stop(id);

  overlayPainting.insert(overlayPainting.begin(),
                         func::DetectionFunction::OverlaySettings{.result          = &response.result,
                                                                  .backgroundColor = cv::Scalar(0, 0, 255),
                                                                  .borderColor     = cv::Scalar(0, 0, 0),
                                                                  .paintRectangel  = false,
                                                                  .opaque          = 1});

  response.controlImage =
      cv::Mat::zeros(channelsToIntersect[0]->originalImage.rows, channelsToIntersect[0]->originalImage.cols, CV_32FC3);

  id = DurationCount::start("intersect paint");
  joda::func::DetectionFunction::paintOverlay(response.controlImage, overlayPainting);
  DurationCount::stop(id);

  return response;
}

}    // namespace joda::pipeline
