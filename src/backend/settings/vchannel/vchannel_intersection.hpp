
#pragma once

#include "../channel/channel_reporting_settings.hpp"
#include "../channel/channel_settings_cross.hpp"
#include "../channel/channel_settings_meta.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

class VChannelIntersection
{
public:
  struct IntersectionSettings
  {
    //
    // List of channels to calc the intersection for
    //
    std::set<ChannelIndex> intersectingChannels;

    //
    // Minimum intersection in [0-1]
    //
    float minIntersection = 0.1F;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(IntersectionSettings, intersectingChannels, minIntersection);
  };

  ChannelSettingsMeta meta;
  CrossChannelSettings crossChannel;
  ChannelReportingSettings reporting;
  IntersectionSettings intersection;

private:
  std::string configSchema = "https://imagec.org/schemas/v1/intersectrion-settings.json";

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VChannelIntersection, meta, crossChannel, reporting, intersection,
                                              configSchema);
};

}    // namespace joda::settings
