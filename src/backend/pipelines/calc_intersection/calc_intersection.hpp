///
/// \file      calc_intersection.hpp
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

#pragma once

#include "../../reporting/reporting.h"
#include "../pipeline_step.hpp"

namespace joda::pipeline {
///
/// \class      CalcIntersection
/// \author     Joachim Danmayr
/// \brief      Calculate the intersection
///
class CalcIntersection : public PipelineStep
{
public:
  /////////////////////////////////////////////////////
  CalcIntersection(const std::set<int32_t> &indexesToIntersect);
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
  static void generateReportHeader(joda::reporting::Table &report,
                                   const settings::json::ChannelSettings &spotChannelSettings, bool intersect);
  static void appendToReport(joda::reporting::Table &report, const func::ROI &spot, int cellIndex, bool intersect);

  /////////////////////////////////////////////////////
  const std::set<int32_t> mIndexesToIntersect;
};
}    // namespace joda::pipeline
