
#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include "channel_settings_meta.hpp"

namespace joda::settings {

class ChannelSettingsFilter
{
public:
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
  // Every particle with a circularity lower than this value is ignored during analysis.
  // Value in [0-1]
  //
  float minCircularity = 0.0;

  //
  // Used for coloc algorithm to define a tolerance around each particle.
  // Value in [px]
  //
  uint32_t snapAreaSize = 0;

  //
  // Index of the reference spot channel.
  // If bigger than 0 this channel will be used to remove reference spots from the channel
  //
  ChannelIndex referenceSpotChannelIndex = ChannelIndex::NONE;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelSettingsFilter, minParticleSize, maxParticleSize, minCircularity,
                                              snapAreaSize, referenceSpotChannelIndex);
};
}    // namespace joda::settings
