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

#include "pipelines/pipeline_step.hpp"
#include "reporting/reporting.h"
#include "settings/analze_settings_parser.hpp"
#include "settings/channel_settings.hpp"

namespace joda::pipeline {

///
/// \class      CountSpotInCells
/// \author     Joachim Danmayr
/// \brief
///
class CountSpotInCells : public PipelineStep
{
public:
  CountSpotInCells()
  {
  }
  /////////////////////////////////////////////////////
  void execute(const settings::json::AnalyzeSettings &, const std::map<int, joda::func::DetectionResponse> &,
               const std::string &detailoutputPath) override;

private:
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
  void generateReportHeader(joda::reporting::Table &report, const settings::json::ChannelSettings &spotChannelSettings,
                            bool intersect);
  void appendToReport(joda::reporting::Table &report, const func::ROI &spot, int cellIndex, bool intersect);
};

}    // namespace joda::pipeline
