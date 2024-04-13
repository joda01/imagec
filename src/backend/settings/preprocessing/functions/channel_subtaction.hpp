#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/channel/channel_index.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

class ChannelSubtraction
{
public:
  ChannelIndex channelIdx = ChannelIndex::NONE;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelSubtraction, channelIdx);
};
}    // namespace joda::settings
