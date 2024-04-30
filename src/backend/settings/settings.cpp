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
#include <stdexcept>
#include <string>
#include "backend/settings/channel/channel_index.hpp"
#include "backend/settings/channel/channel_settings.hpp"
#include "backend/settings/channel/channel_settings_cross.hpp"
#include "backend/settings/channel/channel_settings_meta.hpp"

namespace joda::settings {

void removeNullValues(nlohmann::json &j);

void Settings::storeSettings(std::string path, const joda::settings::AnalyzeSettings &settings)
{
  if(!path.empty()) {
    nlohmann::json json = settings;
    removeNullValues(json);

    if(!path.ends_with(".json")) {
      path += ".json";
    }
    std::ofstream out(path);
    out << json.dump(2);
    out.close();
  }
}

void removeNullValues(nlohmann::json &j)
{
  if(j.is_object()) {
    for(auto it = j.begin(); it != j.end();) {
      if(it.value().is_null()) {
        it = j.erase(it);
      } else {
        removeNullValues(it.value());    // Recursively check nested objects
        ++it;                            // Move to the next element
      }
    }
  } else if(j.is_array()) {
    for(auto &element : j) {
      if(element.is_null()) {
        element = nullptr;    // Replace null values in arrays with nullptr
      } else {
        removeNullValues(element);    // Recursively check nested objects
      }
    }
  }
}

int32_t Settings::getNrOfAllChannels(const joda::settings::AnalyzeSettings &settings)
{
  return settings.channels.size() + settings.vChannels.size();
}

std::vector<const joda::settings::ChannelSettings *>
Settings::getChannelsOfType(const joda::settings::AnalyzeSettings &settings,
                            joda::settings::ChannelSettingsMeta::Type type)
{
  std::vector<const joda::settings::ChannelSettings *> ret;
  for(const auto &ch : settings.channels) {
    if(ch.meta.type == type) {
      ret.push_back(&ch);
    }
  }
  return ret;
}

std::string Settings::getChannelNameOfChannelIndex(const joda::settings::AnalyzeSettings &settings,
                                                   joda::settings::ChannelIndex channelIdx)
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

    if(channelSettings.$intersection.has_value()) {
      if(channelSettings.$intersection->meta.channelIdx == channelIdx) {
        return channelSettings.$intersection->meta.name;
      }
    }
  }
  return "Not Found";
}

const joda::settings::ChannelReportingSettings &
Settings::getReportingSettingsForChannel(const joda::settings::AnalyzeSettings &settings,
                                         joda::settings::ChannelIndex channelIdx)
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

    if(channelSettings.$intersection.has_value()) {
      if(channelSettings.$intersection->meta.channelIdx == channelIdx) {
        return channelSettings.$intersection->reporting;
      }
    }
  }
  throw std::runtime_error("No reporting settings for channel found!");
}

const joda::settings::CrossChannelSettings &
Settings::getCrossChannelSettingsForChannel(const joda::settings::AnalyzeSettings &settings,
                                            joda::settings::ChannelIndex channelIdx)
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

    if(channelSettings.$intersection.has_value()) {
      if(channelSettings.$intersection->meta.channelIdx == channelIdx) {
        return channelSettings.$intersection->crossChannel;
      }
    }
  }
  throw std::runtime_error("No reporting settings for channel found!");
}

void Settings::checkSettings(const joda::settings::AnalyzeSettings &settings)
{
  std::set<joda::settings::ChannelIndex> index;
  std::set<std::string> channelName;

  index.emplace(joda::settings::ChannelIndex::NONE);    // None is always allowed

  //
  // Check for double indexes and names
  //
  auto checkMap = [&index, &channelName](joda::settings::ChannelIndex idx, const std::string &name) {
    if(channelName.contains(name)) {
      throw std::runtime_error("Each channel must have an unique name!");
    }

    if(index.contains(idx)) {
      throw std::runtime_error("Each channel must have an unique index!");
    }
    index.emplace(idx);
    channelName.emplace(name);
  };

  auto checkCrossChannel = [&index, &channelName](const joda::settings::CrossChannelSettings &set) {
    for(const auto idx : set.crossChannelCountChannels) {
      if(!index.contains(idx)) {
        throw std::runtime_error("A cross channel count without corrsponding channel was set!");
      }
    }
    for(const auto idx : set.crossChannelIntensityChannels) {
      if(!index.contains(idx)) {
        throw std::runtime_error("A cross channel intensity without corrsponding channel was set!");
      }
    }
  };

  for(const auto &channel : settings.channels) {
    checkMap(channel.meta.channelIdx, channel.meta.name);
  }

  for(const auto &channel : settings.vChannels) {
    if(channel.$voronoi.has_value()) {
      checkMap(channel.$voronoi->meta.channelIdx, channel.$voronoi->meta.name);
    }
    if(channel.$intersection.has_value()) {
      checkMap(channel.$intersection->meta.channelIdx, channel.$intersection->meta.name);
    }
  }

  //
  // Check for not used indexes
  //

  for(const auto &channel : settings.channels) {
    checkCrossChannel(channel.crossChannel);
    if(!index.contains(channel.objectFilter.referenceSpotChannelIndex)) {
      throw std::runtime_error("A reference spot channel count without corrsponding channel was set!");
    }
    for(const auto &pipe : channel.preprocessing.pipeline)
      if(pipe.$subtractChannel.has_value()) {
        if(!index.contains(pipe.$subtractChannel->channelIdx)) {
          throw std::runtime_error("A subtract channel count without corrsponding channel was set!");
        }
      }
  }

  for(const auto &channel : settings.vChannels) {
    if(channel.$voronoi.has_value()) {
      checkCrossChannel(channel.$voronoi->crossChannel);
      if(!index.contains(channel.$voronoi->voronoi.gridPointsChannelIdx)) {
        throw std::runtime_error("A voronoi grid point channel count without corrsponding channel was set!");
      }
      if(!index.contains(channel.$voronoi->voronoi.overlayMaskChannelIdx)) {
        throw std::runtime_error("A voronoi overlay channel count without corrsponding channel was set!");
      }
    }
    if(channel.$intersection.has_value()) {
      checkCrossChannel(channel.$intersection->crossChannel);
      for(const auto idx : channel.$intersection->intersection.intersectingChannels) {
        if(!index.contains(idx)) {
          throw std::runtime_error("An intersecting channel without corrsponding channel was set!");
        }
      }
    }
  }
}

}    // namespace joda::settings
