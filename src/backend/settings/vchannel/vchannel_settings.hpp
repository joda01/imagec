

#pragma once

#include "../channel/channel_reporting_settings.hpp"
#include "../channel/channel_settings_cross.hpp"
#include "../channel/channel_settings_meta.hpp"
#include <nlohmann/json.hpp>
#include "vchannel_voronoi_settings.hpp"

namespace joda::settings {

#include "backend/helper/json_optional_parser_helper.hpp"

class VChannelSettings
{
public:
  std::optional<std::vector<VChannelVoronoi>> $voronoi;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(VChannelSettings, $voronoi)
};
}    // namespace joda::settings
