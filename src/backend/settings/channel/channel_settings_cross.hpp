
#pragma once

#include <cstdint>
#include <set>
#include <nlohmann/json.hpp>
#include "channel_settings_meta.hpp"

namespace joda::settings {

class CrossChannelSettings
{
public:
  // Cross channel intensity calculation
  std::set<ChannelIndex> crossChannelIntensityChannels;

  // Cross channel count calculation
  std::set<ChannelIndex> crossChannelCountChannels;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(CrossChannelSettings, crossChannelIntensityChannels,
                                              crossChannelCountChannels);
};
}    // namespace joda::settings
