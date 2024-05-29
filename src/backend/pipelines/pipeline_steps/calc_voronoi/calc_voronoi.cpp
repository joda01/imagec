///
/// \file      count_spot_in_cells.cpp
/// \author    Joachim Danmayr
/// \date      2023-07-31
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "calc_voronoi.hpp"
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/image_processing/detection/voronoi_grid/voronoi_grid.hpp"
#include "backend/image_processing/roi/roi.hpp"

namespace joda::pipeline {

auto CalcVoronoi::execute(
    const settings::AnalyzeSettings &settings,
    const std::map<joda::settings::ChannelIndex, joda::image::detect::DetectionResponse> &detectionResults) const
    -> joda::image::detect::DetectionResponse
{
  auto id = DurationCount::start("PipelineVoronoi");

  auto voronoiPoints = mVoronoiPointsChannelIndex;

  const joda::image::detect::DetectionResponse *mask = nullptr;
  if(detectionResults.contains(mOverlayMaskChannelIndex)) {
    mask = &detectionResults.at(mOverlayMaskChannelIndex);
  }

  //
  // Calculate a limited CalcVoronoi grid based on the center of nucleus
  //
  if(detectionResults.contains(voronoiPoints)) {
    auto voronoiPointsChannel = detectionResults.at(voronoiPoints);

    image::detect::VoronoiGrid grid(voronoiPointsChannel.result, mMaxVoronoiAreaSize);
    auto CalcVoronoiResult =
        grid.forward(voronoiPointsChannel.controlImage, voronoiPointsChannel.originalImage, mChannelIndexMe);

    //
    // Now mask the voronoi grid with an other channel
    //
    joda::image::detect::DetectionResponse response;
    response.controlImage = cv::Mat::zeros(mask->controlImage.rows, mask->controlImage.cols, CV_32FC3);

    auto filterVoronoiAreas = [this, &response, &voronoiPointsChannel, &CalcVoronoiResult,
                               &mask](std::optional<const image::ROI> toIntersect) {
      for(auto &voronoiArea : CalcVoronoiResult.result) {
        if(voronoiArea.isValid()) {
          //
          // Apply filter
          //
          auto applyFilter = [this, &response, &voronoiPointsChannel, &CalcVoronoiResult,
                              &mask](image::ROI &cutedVoronoiArea) {
            //
            // Areas without point are filtered out
            //
            if(mExcludeAreasWithoutPoint) {
              if(!doesAreaContainsPoint(cutedVoronoiArea, voronoiPointsChannel.result)) {
                cutedVoronoiArea.setValidity(image::ParticleValidityEnums::INVALID);
              }
            }

            //
            // Check area size
            //
            if(cutedVoronoiArea.getAreaSize() < mMinSize) {
              cutedVoronoiArea.setValidity(image::ParticleValidityEnums::TOO_SMALL);
            } else if(cutedVoronoiArea.getAreaSize() > mMaxSize) {
              cutedVoronoiArea.setValidity(image::ParticleValidityEnums::TOO_BIG);
            }

            //
            // Remove area at the edges if filter enabled
            //
            if(mExcludeAreasAtTheEdges) {
              auto box = cutedVoronoiArea.getBoundingBox();
              if(box.x <= 0 || box.y <= 0 || box.x + box.width >= CalcVoronoiResult.originalImage.cols ||
                 box.y + box.height >= CalcVoronoiResult.originalImage.rows) {
                cutedVoronoiArea.setValidity(image::ParticleValidityEnums::AT_THE_EDGE);
              }
            }
            response.result.push_back(cutedVoronoiArea);
          };

          //
          // Mask if enabled
          //
          if(mask != nullptr && toIntersect.has_value()) {
            auto [cutedVoronoiArea, ok] =
                voronoiArea.calcIntersection(toIntersect.value(),
                                             std::map<joda::settings::ChannelIndex, const cv::Mat *>{
                                                 {mOverlayMaskChannelIndex, &mask->originalImage}},
                                             0.0);
            if(ok) {
              applyFilter(cutedVoronoiArea);
            }
          } else {
            applyFilter(voronoiArea);
          }
        }
      }
    };

    if(mask != nullptr) {
      for(const auto &toIntersect : mask->result) {
        if(toIntersect.isValid()) {
          filterVoronoiAreas(toIntersect);
        }
      }
    } else {
      filterVoronoiAreas(std::nullopt);
    }
    joda::image::detect::DetectionFunction::paintBoundingBox(response.controlImage, response.result, {}, "#FF0000",
                                                             false, false);
    DurationCount::stop(id);
    return response;
  }
  DurationCount::stop(id);
  return joda::image::detect::DetectionResponse{};
  /*throw std::runtime_error("CalcVoronoi::execute: Channel with index >" + std::to_string(voronoiPoints) +
                           "< does not exist.");*/
}

bool CalcVoronoi::doesAreaContainsPoint(const image::ROI &voronoiArea,
                                        const joda::image::detect::DetectionResults &voronoiPoints)
{
  for(const auto &point : voronoiPoints) {
    if(voronoiArea.isIntersecting(point, 0.1)) {
      return true;
    }
  }
  return false;
}

}    // namespace joda::pipeline
