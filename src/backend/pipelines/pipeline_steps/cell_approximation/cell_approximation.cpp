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

#include "cell_approximation.hpp"
#include <stdexcept>
#include "backend/image_processing/detection/voronoi_grid/voronoi_grid.hpp"

namespace joda::pipeline {

auto CellApproximation::execute(const settings::json::AnalyzeSettings &settings,
                                const std::map<int, joda::func::DetectionResponse> &detectionResults,
                                const std::string &detailoutputPath) const -> joda::func::DetectionResponse
{
  auto nucleusChannelIndex = mNucleusChannelIndex;

  //
  // Calculate a limited voronoi grid based on the center of nucleus
  //
  joda::func::img::VoronoiGrid grid(detectionResults.at(nucleusChannelIndex).result, mMaxCellRadius);
  auto voronoiResult = grid.forward(detectionResults.at(nucleusChannelIndex).controlImage,
                                    detectionResults.at(nucleusChannelIndex).originalImage);
  return voronoiResult;
}

}    // namespace joda::pipeline
