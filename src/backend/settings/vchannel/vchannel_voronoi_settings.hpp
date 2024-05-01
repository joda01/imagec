
#pragma once

#include "../channel/channel_reporting_settings.hpp"
#include "../channel/channel_settings_cross.hpp"
#include "../channel/channel_settings_meta.hpp"
#include "backend/settings/channel/channel_settings_filter.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

class VChannelVoronoi
{
public:
  struct VoronoiGridSettings
  {
    // This is the index of the channel which contains the points using for calculating the voronoi grid
    joda::settings::ChannelIndex gridPointsChannelIdx = joda::settings::ChannelIndex::NONE;

    // Optional channel which is used to overlay with the voronoi grid (AND combination e.g. a cell area)
    joda::settings::ChannelIndex overlayMaskChannelIdx = joda::settings::ChannelIndex::NONE;

    // Maximum radius of a calculated voronoi area
    int32_t maxVoronoiAreaRadius = -1;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VoronoiGridSettings, gridPointsChannelIdx, overlayMaskChannelIdx,
                                                maxVoronoiAreaRadius);
  };

  struct VoronoiFiltering
  {
    //
    // Every particle with a diameter lower than that is ignored during analysis.
    // Value in [px]
    //
    uint64_t minParticleSize = 0;

    //
    // Every particle with a diameter bigger than that is ignored during analysis.
    // Value in [px]
    //
    uint64_t maxParticleSize = UINT64_MAX;

    //
    // Filter out areas which have no center of mass
    //
    bool excludeAreasWithoutCenterOfMass = true;

    //
    // Filter out areas at the edges
    //
    bool excludeAreasAtEdges = false;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VoronoiFiltering, minParticleSize, maxParticleSize, excludeAreasAtEdges,
                                                excludeAreasWithoutCenterOfMass);
  };

  ChannelSettingsMeta meta;
  CrossChannelSettings crossChannel;
  ChannelReportingSettings reporting;
  VoronoiGridSettings voronoi;
  VoronoiFiltering objectFilter;

private:
  std::string configSchema = "https://imagec.org/schemas/v1/voronoi-settings.json";

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VChannelVoronoi, meta, crossChannel, reporting, voronoi, objectFilter,
                                              configSchema);
};

}    // namespace joda::settings
