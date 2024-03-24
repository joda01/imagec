///
/// \file      calc_intensity.hpp
/// \author    Joachim Danmayr
/// \date      2024-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Measure intensity from ROI area of channel X in channel Y
///

#pragma once

#include <utility>

#include "backend/pipelines/pipeline_step.hpp"

namespace joda::pipeline {

///
/// \class      CalcVoronoi
/// \author     Joachim Danmayr
/// \brief
///
class CalcIntensity : public PipelineStep
{
public:
  CalcIntensity(int32_t referenceChannelIndex, std::set<int32_t> channelsToCalcIntensityIn) :
      mReferenceChannelIndex(referenceChannelIndex), mChannelsToCalcIntensityIn(std::move(channelsToCalcIntensityIn))
  {
  }
  /////////////////////////////////////////////////////
  auto execute(const settings::json::AnalyzeSettings &, const std::map<int, joda::func::DetectionResponse> &,
               const std::string &detailoutputPath) const -> joda::func::DetectionResponse override;

private:
  /////////////////////////////////////////////////////
  int32_t mReferenceChannelIndex;
  std::set<int32_t> mChannelsToCalcIntensityIn;
};

}    // namespace joda::pipeline
