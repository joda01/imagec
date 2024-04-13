

#pragma once

#include <optional>
#include "../channel/channel_reporting_settings.hpp"
#include "../channel/channel_settings_cross.hpp"
#include "../channel/channel_settings_meta.hpp"
#include "backend/settings/vchannel/vchannel_intersection.hpp"
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>
#include "vchannel_voronoi_settings.hpp"

namespace joda::settings {

#include "backend/helper/json_optional_parser_helper.hpp"

class VChannelSettings
{
public:
  std::optional<VChannelVoronoi> $voronoi           = std::nullopt;
  std::optional<VChannelIntersection> $intersection = std::nullopt;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VChannelSettings, $voronoi, $intersection)
};
}    // namespace joda::settings
