
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
  FilterMode filterMode = FilterMode::OFF;

  //
  // If this number of particles is exceeded the image is marked as noisy
  //
  int64_t maxObjects = -1;

  //
  // If the min threshold is lower than the value at the maximum of the histogram multiplied with this factor the filter
  // will be applied.
  // Example: Min Threshold = 50
  //          Hist. Max is at 40
  //          histMinThresholdFilterFactor = 1.3  --> HistogramFilterThreshold = 65 --> Filter will be applied
  //
  // Example: Min Threshold = 50
  //          Hist. Max is at 10
  //          histMinThresholdFilterFactor = 1.3  --> HistogramFilterThreshold = 13 --> Filter will not be applied
  //
  float histMinThresholdFilterFactor = -1;

  //
  // Image threshold error detection
  //

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelImageFilter, filterMode, maxObjects, histMinThresholdFilterFactor);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ChannelImageFilter::FilterMode,
                             {
                                 {ChannelImageFilter::FilterMode::OFF, "Off"},
                                 {ChannelImageFilter::FilterMode::INVALIDATE_CHANNEL, "InvalidateChannel"},
                                 {ChannelImageFilter::FilterMode::INVALIDATE_WHOLE_IMAGE, "InvalidateWholeImage"},
                             })

}    // namespace joda::settings
