///
/// \file      pipeline_settings.cpp
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

#include "pipeline_settings.hpp"
#include "backend/pipelines/calc_intersection/calc_intersection.hpp"
#include "backend/pipelines/cell_approximation/cell_approximation.hpp"
#include "backend/settings/channel_settings.hpp"

namespace joda::settings::json {

///
/// \brief      Creates the method instance for the configured pipeline step
/// \author     Joachim Danmayr
///
void PipelineStepSettings::interpretConfig()
{
  if(cell_approximation.nucleus_channel_index >= 0 || cell_approximation.cell_channel_index >= 0) {
    mChannelSettings.index = PipelineStepIndex::CELL_APPROXIMATION;
    mChannelSettings.name  = "Approx. Cells";
    mPipelineStep = std::make_shared<joda::pipeline::CellApproximation>(cell_approximation.nucleus_channel_index,
                                                                        cell_approximation.cell_channel_index,
                                                                        cell_approximation.max_cell_radius);
  }

  if(!intersection.channel_index.empty()) {
    mChannelSettings.index = PipelineStepIndex::INTERSECTION;
    mChannelSettings.name  = "Intersection";
    mPipelineStep =
        std::make_shared<joda::pipeline::CalcIntersection>(intersection.channel_index, intersection.min_intersection);
  }
}

///
/// \brief      Executes the pipeline step
/// \author     Joachim Danmayr
///
auto PipelineStepSettings::execute(const settings::json::AnalyzeSettings &settings,
                                   const std::map<int, joda::func::DetectionResponse> &responseIn,
                                   const std::string &detailoutputPath) const
    -> std::tuple<ChannelSettings, joda::func::DetectionResponse>
{
  if(mPipelineStep) {
    return {mChannelSettings, mPipelineStep->execute(settings, responseIn, detailoutputPath)};
  }
  return {ChannelSettings{}, joda::func::DetectionResponse{}};
}

}    // namespace joda::settings::json
