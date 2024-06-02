
#pragma once

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

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(IntersectionSettings, intersectingChannels);
  };

  struct IntersectionFiltering
  {
    //
    // Every particle with a diameter lower than that is ignored during analysis.
    // Value in [px]
    //
    uint64_t minParticleSize = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(IntersectionFiltering, minParticleSize);
  };

  ChannelSettingsMeta meta;
  CrossChannelSettings crossChannel;
  IntersectionSettings intersection;
  IntersectionFiltering objectFilter;

private:
  std::string configSchema = "https://imagec.org/schemas/v1/intersectrion-settings.json";

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VChannelIntersection, meta, crossChannel, intersection, configSchema);
};

}    // namespace joda::settings
