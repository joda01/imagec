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
  CalcIntersection(const std::set<int32_t> &indexesToIntersect, float minIntersection);
  auto execute(const settings::json::AnalyzeSettings &, const std::map<int, joda::func::DetectionResponse> &,
               const std::string &detailoutputPath) const -> joda::func::DetectionResponse override;

private:
  /////////////////////////////////////////////////////
  const std::set<int32_t> mIndexesToIntersect;
  const float mMinIntersection;
};
}    // namespace joda::pipeline
