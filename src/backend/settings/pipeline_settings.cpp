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
#include "backend/pipelines/pipeline_steps/calc_intersection/calc_intersection.hpp"
#include "backend/pipelines/pipeline_steps/calc_voronoi/calc_voronoi.hpp"
#include "backend/settings/channel_settings.hpp"

namespace joda::settings::json {

///
/// \brief      Creates the method instance for the configured pipeline step
/// \author     Joachim Danmayr
///
void PipelineStepSettings::interpretConfig(int pipelineIndex)
{
  if(voronoi.getPointsChannelIndex() >= 0) {
    mChannelSettings.index = voronoi.getPointsChannelIndex() + VORONOI_INDEX_OFFSET;
    mChannelSettings.name  = "Voronoi";
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
  if(voronoi.getPointsChannelIndex() >= 0) {
    joda::pipeline::CalcVoronoi function(voronoi.getPointsChannelIndex(), voronoi.getOverlayMaskChannelIndex(),
                                         voronoi.getMaxVoronoiAreaRadius());
    return {mChannelSettings, function.execute(settings, responseIn, detailoutputPath)};
  }

  return {ChannelSettings{}, joda::func::DetectionResponse{}};
}

}    // namespace joda::settings::json
