
#pragma once

#include "backend/settings/channel/channel_settings_filter.hpp"
#include <nlohmann/json.hpp>
#include "channel_reporting_settings.hpp"
#include "channel_settings_cross.hpp"
#include "channel_settings_meta.hpp"

namespace joda::settings {

class ChannelSettings
{
public:
  ChannelSettingsMeta meta;
  ChannelSettingsFilter filter;
  CrossChannelSettings crossChannel;
  ChannelReportingSettings reporting;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ChannelSettings, meta, filter, crossChannel, reporting);
};
}    // namespace joda::settings
