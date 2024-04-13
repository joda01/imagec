

#pragma once

#include <nlohmann/json.hpp>
#include "channel_index.hpp"

namespace joda::settings {

class ChannelSettingsMeta
{
public:
  enum class Type
  {
    NONE,
    SPOT,
    SPOT_REFERENCE,
    NUCLEUS,
    CELL,
    BACKGROUND,
  };

  /////////////////////////////////////////////////////
  //
  // Corresponding channel
  // [0, 1, 2, ...]
  //
  ChannelIndex channelIdx = ChannelIndex::CH0;

  //
  // Series to analyze
  // [0, 1, 2, ...]
  //
  int32_t series = 0;

  //
  // User defined name of the channel
  //
  std::string name;

  //
  // What is seen in this channel
  // [NUCLEUS, EV, BACKGROUND, CELL_BRIGHTFIELD, CELL_DARKFIELD]
  //
  ChannelSettingsMeta::Type type = ChannelSettingsMeta::Type::NONE;

  //
  // Label of the channel if available
  // [CY5, CY3]
  //
  std::string label;

  //
  // Color of the channel
  // [#B91717]
  //
  std::string color = "#B91717";

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelSettingsMeta, channelIdx, series, name, type, label, color);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ChannelSettingsMeta::Type, {{ChannelSettingsMeta::Type::NONE, ""},
                                                         {ChannelSettingsMeta::Type::BACKGROUND, "Background"},
                                                         {ChannelSettingsMeta::Type::CELL, "Cell"},
                                                         {ChannelSettingsMeta::Type::NUCLEUS, "Nucleus"},
                                                         {ChannelSettingsMeta::Type::SPOT, "Spot"},
                                                         {ChannelSettingsMeta::Type::SPOT_REFERENCE, "SpotReference"}});

}    // namespace joda::settings
