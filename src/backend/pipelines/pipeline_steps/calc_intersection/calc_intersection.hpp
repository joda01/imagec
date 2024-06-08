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

#include "backend/pipelines/pipeline_step.hpp"

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
  CalcIntersection(joda::settings::ChannelIndex channelIndexMe,
                   const std::set<joda::settings::ChannelIndex> &indexesToIntersect, uint64_t minIntersection);

  auto execute(const settings::AnalyzeSettings &,
               const std::map<joda::settings::ChannelIndex, image::detect::DetectionResponse> &) const
      -> image::detect::DetectionResponse override;

private:
  /////////////////////////////////////////////////////
  joda::settings::ChannelIndex mChannelIndexMe;
  const std::vector<joda::settings::ChannelIndex> mIndexesToIntersect;
  const uint64_t mMinIntersection;
};
}    // namespace joda::pipeline
