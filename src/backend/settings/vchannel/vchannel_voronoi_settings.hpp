
#pragma once

#include "../channel/channel_reporting_settings.hpp"
#include "../channel/channel_settings_cross.hpp"
#include "../channel/channel_settings_meta.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

class VChannelVoronoi
{
public:
  struct VoronoiGridSettings
  {
    // This is the index of the channel which contains the points using for calculating the voronoi grid
    int32_t gridPointsChannelIdx = -1;

    // Optional channel which is used to overlay with the voronoi grid (AND combination e.g. a cell area)
    int32_t overlayMaskChannelIdx = -1;

    // Maximum radius of a calculated voronoi area
    int32_t maxVoronoiAreaRadius = -1;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VoronoiGridSettings, gridPointsChannelIdx, overlayMaskChannelIdx,
                                                maxVoronoiAreaRadius);
  };

  ChannelSettingsMeta meta;
  CrossChannelSettings crossChannel;
  ChannelReportingSettings reporting;
  VoronoiGridSettings voronoi;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VChannelVoronoi, meta, crossChannel, reporting, voronoi);
};

}    // namespace joda::settings
