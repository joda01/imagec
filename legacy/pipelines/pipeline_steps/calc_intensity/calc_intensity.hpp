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
  CalcIntensity(joda::settings::ChannelIndex referenceChannelIndex,
                std::set<joda::settings::ChannelIndex> channelsToCalcIntensityIn) :
      mReferenceChannelIndex(referenceChannelIndex),
      mChannelsToCalcIntensityIn(std::move(channelsToCalcIntensityIn))
  {
  }
  /////////////////////////////////////////////////////
  auto execute(const settings::AnalyzeSettings &,
               const std::map<joda::settings::ChannelIndex, joda::image::detect::DetectionResponse> &) const
      -> joda::image::detect::DetectionResponse override;

private:
  /////////////////////////////////////////////////////
  joda::settings::ChannelIndex mReferenceChannelIndex;
  std::set<joda::settings::ChannelIndex> mChannelsToCalcIntensityIn;
};

}    // namespace joda::pipeline
