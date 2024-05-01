
#pragma once

#include <cstdint>
#include <nlohmann/json.hpp>
#include "channel_settings_meta.hpp"

namespace joda::settings {

class ChannelImageFilter
{
public:
  enum FilterMode
  {
    OFF                    = 0,
    INVALIDATE_CHANNEL     = 1,
    INVALIDATE_WHOLE_IMAGE = 2
  };

  //
  // Filter mode
  //
  FilterMode filterMode = FilterMode::INVALIDATE_WHOLE_IMAGE;

  //
  // If this number of particles is exceeded the image is marked as noisy
  //
  int64_t maxParticleNumber = 50;

  //
  // Image threshold error detection
  //

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelImageFilter, filterMode, maxParticleNumber);
};
}    // namespace joda::settings
