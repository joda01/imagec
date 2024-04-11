#pragma once

#include <cstdint>
#include <set>
#include <nlohmann/json.hpp>

namespace joda::settings {

class ChannelSubtraction
{
public:
  int32_t channelIdx{-1};

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelSubtraction, channelIdx);
};
}    // namespace joda::settings
