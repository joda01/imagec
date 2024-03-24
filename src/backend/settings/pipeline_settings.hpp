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
/// \class      PipelineStepVoronoi
/// \author     Joachim Danmayr
/// \brief      Cell approximation settings
///
class PipelineStepVoronoi final
{
public:
  auto getName() const -> const std::string &
  {
    return name;
  }

  auto getPointsChannelIndex() const
  {
    return points_channel_index;
  }

  auto getColor() const -> const std::string &
  {
    return color;
  }

  auto getMaxVoronoiAreaRadius() const -> const int32_t
  {
    return max_voronoi_area_radius;
  }

  auto getColocGroups() const -> const std::set<std::string> &
  {
    return coloc_groups;
  }

  auto getMinColocArea() const
  {
    return min_coloc_area;
  }

  auto getCrossChannelIntensityChannels() const -> const std::set<int32_t> &
  {
    return cross_channel_intensity_channels;
  }

  int32_t getOverlayMaskChannelIndex() const
  {
    return overlay_mask_channel_index;
  }

private:
  // This is the index of the channel which contains the points using for calculating the voronoi grid
  std::string name;

  //
  // Color of the channel
  // [#B91717]
  //
  std::string color = "#B91717";

  // This is the index of the channel which contains the points using for calculating the voronoi grid
  int32_t points_channel_index = -1;

  // Optional channel which is used to overlay with the voronoi grid (AND combination e.g. a cell area)
  int32_t overlay_mask_channel_index = -1;

  // Maximum radius of a calculated voronoi area
  int32_t max_voronoi_area_radius = -1;

  // Cross channel intensity calculation
  std::set<int32_t> cross_channel_intensity_channels;

  // In which coloc groups this channel is part of
  std::set<std::string> coloc_groups;

  // Minimum area in [%] the areas in the coloc must overlap to be marked as valid coloc
  float min_coloc_area = 0;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PipelineStepVoronoi, name, color, points_channel_index,
                                              overlay_mask_channel_index, max_voronoi_area_radius,
                                              cross_channel_intensity_channels, min_coloc_area, coloc_groups);
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
  static constexpr int32_t VORONOI_INDEX_OFFSET      = 100;
  static constexpr int32_t INTERSECTION_INDEX_OFFSET = 200;
  struct ChannelSettings
  {
    int32_t index = NONE_PIPELINE_STEP;
    std::string name;
  };

  /////////////////////////////////////////////////////
  void interpretConfig(int pipelineIndex);

  ///
  /// \brief Returns the channel index of the pipeline step.
  ///        Each step has an unique index
  ///
  int32_t getChannelIndex();

  [[nodiscard]] auto getVoronoi() const -> const PipelineStepVoronoi *
  {
    if(voronoi.getPointsChannelIndex() >= 0) {
      return &voronoi;
    }
    return nullptr;
  }

private:
  /////////////////////////////////////////////////////
  PipelineStepVoronoi voronoi;
  ChannelSettings mChannelSettings;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(PipelineStepSettings, voronoi);
};
}    // namespace joda::settings::json
