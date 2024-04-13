///
/// \file      settings.cpp
/// \author    Joachim Danmayr
/// \date      2024-04-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "settings.hpp"
#include <exception>
#include "backend/settings/channel/channel_settings.hpp"
#include "backend/settings/channel/channel_settings_meta.hpp"

namespace joda::settings {

void Settings::storeSettings(const std::string &path, const joda::settings::AnalyzeSettings &settings)
{
}

int32_t Settings::getNrOfAllChannels(const joda::settings::AnalyzeSettings &settings)
{
  return settings.channels.size() + settings.vChannels.size();
}

std::set<const joda::settings::ChannelSettings *>
Settings::getChannelsOfType(const joda::settings::AnalyzeSettings &settings,
                            joda::settings::ChannelSettingsMeta::Type type)
{
  std::set<const joda::settings::ChannelSettings *> ret;
  for(const auto &ch : settings.channels) {
    if(ch.meta.type == type) {
      ret.emplace(&ch);
    }
  }
  return ret;
}

std::string Settings::getChannelNameOfChannelIndex(const joda::settings::AnalyzeSettings &settings, int32_t channelIdx)
{
  for(const auto &channelSettings : settings.channels) {
    if(channelSettings.meta.channelIdx == channelIdx) {
      return channelSettings.meta.name;
    }
  }

  for(const auto &channelSettings : settings.vChannels) {
    if(channelSettings.$voronoi.has_value()) {
      if(channelSettings.$voronoi->meta.channelIdx == channelIdx) {
        return channelSettings.$voronoi->meta.name;
      }
    }
  }
  return "Not Found";
}

const joda::settings::ChannelReportingSettings &
Settings::getReportingSettingsForChannel(const joda::settings::AnalyzeSettings &settings, int32_t channelIdx)
{
  for(const auto &channelSettings : settings.channels) {
    if(channelSettings.meta.channelIdx == channelIdx) {
      return channelSettings.reporting;
    }
  }

  for(const auto &channelSettings : settings.vChannels) {
    if(channelSettings.$voronoi.has_value()) {
      if(channelSettings.$voronoi->meta.channelIdx == channelIdx) {
        return channelSettings.$voronoi->reporting;
      }
    }
  }
  throw std::runtime_error("No reporting settings for channel found!");
}

const joda::settings::CrossChannelSettings &
Settings::getCrossChannelSettingsForChannel(const joda::settings::AnalyzeSettings &settings, int32_t channelIdx)
{
  for(const auto &channelSettings : settings.channels) {
    if(channelSettings.meta.channelIdx == channelIdx) {
      return channelSettings.crossChannel;
    }
  }

  for(const auto &channelSettings : settings.vChannels) {
    if(channelSettings.$voronoi.has_value()) {
      if(channelSettings.$voronoi->meta.channelIdx == channelIdx) {
        return channelSettings.$voronoi->crossChannel;
      }
    }
  }
  throw std::runtime_error("No reporting settings for channel found!");
}

}    // namespace joda::settings
