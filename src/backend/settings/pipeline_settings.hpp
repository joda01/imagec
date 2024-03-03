///
/// \file      pipeline_settings.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-28
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <cstddef>
#include "backend/image_processing/detection/detection_response.hpp"
#include <nlohmann/json.hpp>

namespace joda::pipeline {
class PipelineStep;
}    // namespace joda::pipeline

namespace joda::settings::json {

class AnalyzeSettings;

///
/// \class      PipelineStepCellApproximation
/// \author     Joachim Danmayr
/// \brief      Cell approximation settings
///
class PipelineStepCellApproximation final
{
public:
  int32_t nucleus_channel_index = -1;
  int32_t cell_channel_index    = -1;
  int32_t max_cell_radius       = -1;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PipelineStepCellApproximation, nucleus_channel_index, cell_channel_index,
                                              max_cell_radius);
};

///
/// \class      PipelineStepIntersection
/// \author     Joachim Danmayr
/// \brief      Intersection settings
///
class PipelineStepIntersection final
{
public:
  std::set<int32_t> channel_index;
  float min_intersection;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PipelineStepIntersection, channel_index, min_intersection);
};

///
/// \class      PipelineStepSettings
/// \author     Joachim Danmayr
/// \brief      Pipeline step
///
class PipelineStepSettings final
{
public:
  /////////////////////////////////////////////////////
  static constexpr int32_t NONE_PIPELINE_STEP        = -1;
  static constexpr int32_t CELL_APPROX_INDEX_OFFSET  = 100;
  static constexpr int32_t INTERSECTION_INDEX_OFFSET = 200;
  struct ChannelSettings
  {
    int32_t index = NONE_PIPELINE_STEP;
    std::string name;
  };

  /////////////////////////////////////////////////////
  void interpretConfig(int pipelineIndex);

  [[nodiscard]] auto execute(const settings::json::AnalyzeSettings &,
                             const std::map<int, joda::func::DetectionResponse> &,
                             const std::string &detailoutputPath) const
      -> std::tuple<ChannelSettings, joda::func::DetectionResponse>;

  ///
  /// \brief Returns the channel index of the pipeline step.
  ///        Each step has an unique index
  ///
  int32_t getChannelIndex();

  [[nodiscard]] auto getCellApproximation() const -> const PipelineStepCellApproximation *
  {
    if(cell_approximation.nucleus_channel_index >= 0 || cell_approximation.cell_channel_index >= 0) {
      return &cell_approximation;
    }
    return nullptr;
  }

  [[nodiscard]] auto getIntersection() const -> const PipelineStepIntersection *
  {
    if(!intersection.channel_index.empty()) {
      return &intersection;
    }
    return nullptr;
  }

private:
  /////////////////////////////////////////////////////
  PipelineStepCellApproximation cell_approximation;
  PipelineStepIntersection intersection;
  ChannelSettings mChannelSettings;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PipelineStepSettings, cell_approximation, intersection);
};
}    // namespace joda::settings::json
