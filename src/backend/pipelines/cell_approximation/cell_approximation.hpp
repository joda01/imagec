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

#include "../../reporting/reporting.h"
#include "../../settings/channel_settings.hpp"
#include "../pipeline_step.hpp"

namespace joda::pipeline {

///
/// \class      CellApproximation
/// \author     Joachim Danmayr
/// \brief
///
class CellApproximation : public PipelineStep
{
public:
  CellApproximation(int32_t nucleusChannelIndex, int32_t cellChannelIndex, int32_t maxCellRadius) :
      mNucleusChannelIndex(nucleusChannelIndex), mCellChannelIndex(cellChannelIndex), mMaxCellRadius(maxCellRadius)
  {
  }
  /////////////////////////////////////////////////////
  auto execute(const settings::json::AnalyzeSettings &, const std::map<int, joda::func::DetectionResponse> &,
               const std::string &detailoutputPath) const -> joda::func::DetectionResponse override;

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
  static const int NR_OF_COLUMNS_PER_CHANNEL_IN_DETAIL_REPORT = 8;

  /////////////////////////////////////////////////////

  int32_t mNucleusChannelIndex;
  int32_t mCellChannelIndex;
  int32_t mMaxCellRadius;
};

}    // namespace joda::pipeline