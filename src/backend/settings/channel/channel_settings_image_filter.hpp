
#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include "channel_settings_meta.hpp"

namespace joda::settings {

class ChannelImageFilter
{
public:
  //
  // If this number of particles is exceeded the image is marked as noisy
  //
  uint64_t maxParticleNumber = 250;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelImageFilter, maxParticleNumber);
};
}    // namespace joda::settings
