///
/// \file      count_spot_in_cells.hpp
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

#pragma once

#include <cstdint>
#include "backend/pipelines/pipeline_step.hpp"

namespace joda::pipeline {

///
/// \class      CalcVoronoi
/// \author     Joachim Danmayr
/// \brief
///
class CalcVoronoi : public PipelineStep
{
public:
  CalcVoronoi(joda::settings::ChannelIndex channelIndexMe, joda::settings::ChannelIndex nucleusChannelIndex,
              joda::settings::ChannelIndex cellChannelIndex, int32_t maxVoronoiAreaSize, bool excludeAreasWithoutPoint,
              bool excludeAreasAtEdges, uint64_t minSize, uint64_t maxSize) :
      mChannelIndexMe(channelIndexMe),
      mVoronoiPointsChannelIndex(nucleusChannelIndex), mOverlayMaskChannelIndex(cellChannelIndex),
      mMaxVoronoiAreaSize(maxVoronoiAreaSize), mExcludeAreasWithoutPoint(excludeAreasWithoutPoint),
      mExcludeAreasAtTheEdges(excludeAreasAtEdges), mMinSize(minSize), mMaxSize(maxSize)
  {
  }
  /////////////////////////////////////////////////////
  auto execute(const settings::AnalyzeSettings &,
               const std::map<joda::settings::ChannelIndex, joda::image::detect::DetectionResponse> &) const
      -> joda::image::detect::DetectionResponse override;

private:
  /////////////////////////////////////////////////////
  enum class ColumnIndexDetailedReport : int
  {
    CONFIDENCE             = 0,
    INTENSITY              = 1,
    INTENSITY_MIN          = 2,
    INTENSITY_MAX          = 3,
    AREA_SIZE              = 4,
    CIRCULARITY            = 5,
    VALIDITY               = 6,
    CELL_INTERSECTING_WITH = 7
  };

  static bool doesAreaContainsPoint(const image::ROI &voronoiArea,
                                    const std::unique_ptr<joda::image::detect::DetectionResults> &voronoiPoints);

  /////////////////////////////////////////////////////
  joda::settings::ChannelIndex mChannelIndexMe;
  joda::settings::ChannelIndex mVoronoiPointsChannelIndex;
  joda::settings::ChannelIndex mOverlayMaskChannelIndex;
  int32_t mMaxVoronoiAreaSize;
  bool mExcludeAreasWithoutPoint = true;
  bool mExcludeAreasAtTheEdges   = false;
  uint64_t mMinSize;
  uint64_t mMaxSize;
};

}    // namespace joda::pipeline
