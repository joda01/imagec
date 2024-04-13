

#pragma once

#include <nlohmann/json.hpp>

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
  int32_t channelIdx = 0;

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

  NLOHMANN_JSON_SERIALIZE_ENUM(Type, {{Type::NONE, ""},
                                      {Type::BACKGROUND, "Background"},
                                      {Type::CELL, "Cell"},
                                      {Type::NUCLEUS, "Nucleus"},
                                      {Type::SPOT, "Spot"},
                                      {Type::SPOT_REFERENCE, "SpotReference"}});

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelSettingsMeta, channelIdx, series, name, type, label, color);
};
}    // namespace joda::settings
