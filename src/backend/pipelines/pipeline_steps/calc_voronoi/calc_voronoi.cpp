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
#include <stdexcept>
#include <string>
#include "backend/duration_count/duration_count.h"
#include "backend/image_processing/detection/voronoi_grid/voronoi_grid.hpp"

namespace joda::pipeline {

auto CalcVoronoi::execute(const settings::AnalyzeSettings &settings,
                          const std::map<joda::settings::ChannelIndex, joda::func::DetectionResponse> &detectionResults,
                          const std::string &detailoutputPath) const -> joda::func::DetectionResponse
{
  auto id = DurationCount::start("PipelineVoronoi");

  auto voronoiPoints = mVoronoiPointsChannelIndex;

  const joda::func::DetectionResponse *mask = nullptr;
  if(detectionResults.contains(mOverlayMaskChannelIndex)) {
    mask = &detectionResults.at(mOverlayMaskChannelIndex);
  }

  //
  // Calculate a limited CalcVoronoi grid based on the center of nucleus
  //
  if(detectionResults.contains(voronoiPoints)) {
    joda::func::img::VoronoiGrid grid(detectionResults.at(voronoiPoints).result, mMaxVoronoiAreaSize);
    auto CalcVoronoiResult = grid.forward(detectionResults.at(voronoiPoints).controlImage,
                                          detectionResults.at(voronoiPoints).originalImage, mChannelIndexMe);

    if(!CalcVoronoiResult.controlImage.empty()) {
      static const std::string separator(1, std::filesystem::path::preferred_separator);
      std::vector<int> compression_params;
      compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
      compression_params.push_back(0);
      cv::imwrite(detailoutputPath + separator + "control_voronoi_" +
                      joda::settings::to_string(mVoronoiPointsChannelIndex) + "_" + std::to_string(0) + ".png",
                  CalcVoronoiResult.controlImage, compression_params);
    }

    //
    // Now mask the voronoi grid with an other channel
    //
    if(mask != nullptr) {
      joda::func::DetectionResponse response;
      response.controlImage = cv::Mat::zeros(mask->controlImage.rows, mask->controlImage.cols, CV_32FC3);

      for(const auto &toIntersect : mask->result) {
        if(toIntersect.isValid()) {
          for(const auto &voronoiArea : CalcVoronoiResult.result) {
            auto [colocROI, ok] = voronoiArea.calcIntersection(toIntersect,
                                                               std::map<joda::settings::ChannelIndex, const cv::Mat *>{
                                                                   {mOverlayMaskChannelIndex, &mask->originalImage}},
                                                               0.0);
            if(ok) {
              response.result.push_back(colocROI);
            }
          }
        }
      }
      joda::func::DetectionFunction::paintBoundingBox(response.controlImage, response.result, {}, "#FF0000", false,
                                                      false);
      DurationCount::stop(id);
      return response;
    } else {
      DurationCount::stop(id);
      return CalcVoronoiResult;
    }
  }
  DurationCount::stop(id);
  return joda::func::DetectionResponse{};
  /*throw std::runtime_error("CalcVoronoi::execute: Channel with index >" + std::to_string(voronoiPoints) +
                           "< does not exist.");*/
}

}    // namespace joda::pipeline
