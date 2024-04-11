
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
  std::set<int32_t> crossChannelIntensityChannels;

  // Cross channel count calculation
  std::set<int32_t> crossChannelCoutChannels;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(CrossChannelSettings, crossChannelIntensityChannels,
                                              crossChannelCoutChannels);
};
}    // namespace joda::settings
