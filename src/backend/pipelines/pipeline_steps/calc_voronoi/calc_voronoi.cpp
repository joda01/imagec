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
#include <stdexcept>
#include <string>
#include "backend/image_processing/detection/voronoi_grid/voronoi_grid.hpp"

namespace joda::pipeline {

auto CalcVoronoi::execute(const settings::json::AnalyzeSettings &settings,
                          const std::map<int, joda::func::DetectionResponse> &detectionResults,
                          const std::string &detailoutputPath) const -> joda::func::DetectionResponse
{
  auto nucleusChannelIndex = mNucleusChannelIndex;

  //
  // Calculate a limited CalcVoronoi grid based on the center of nucleus
  //
  if(detectionResults.contains(nucleusChannelIndex)) {
    int32_t channelIndex = (nucleusChannelIndex + settings::json::PipelineStepSettings::VORONOI_INDEX_OFFSET);
    joda::func::img::VoronoiGrid grid(detectionResults.at(nucleusChannelIndex).result, mMaxVoronoiAreaSize);
    auto CalcVoronoiResult = grid.forward(detectionResults.at(nucleusChannelIndex).controlImage,
                                          detectionResults.at(nucleusChannelIndex).originalImage, channelIndex);
    return CalcVoronoiResult;
  }
  return joda::func::DetectionResponse{};
  /*throw std::runtime_error("CalcVoronoi::execute: Channel with index >" + std::to_string(nucleusChannelIndex) +
                           "< does not exist.");*/
}

}    // namespace joda::pipeline
