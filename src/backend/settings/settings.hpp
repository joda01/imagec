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

#pragma once

#include <set>
#include <string>
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/channel/channel_reporting_settings.hpp"
#include "backend/settings/channel/channel_settings_cross.hpp"

namespace joda::settings {

///
/// \class      Settings
/// \author     Joachim Danmayr
/// \brief      Settings helper class
///
class Settings
{
public:
  static void storeSettings(const std::string &path, const joda::settings::AnalyzeSettings &settings);
  static int32_t getNrOfAllChannels(const joda::settings::AnalyzeSettings &settings);

  static std::set<const joda::settings::ChannelSettings *>
  getChannelsOfType(const joda::settings::AnalyzeSettings &settings, joda::settings::ChannelSettingsMeta::Type type);

  static std::string getChannelNameOfChannelIndex(const joda::settings::AnalyzeSettings &settings, int32_t channelIdx);

  static const joda::settings::ChannelReportingSettings &
  getReportingSettingsForChannel(const joda::settings::AnalyzeSettings &settings, int32_t channelIdx);

  static const joda::settings::CrossChannelSettings &
  getCrossChannelSettingsForChannel(const joda::settings::AnalyzeSettings &settings, int32_t channelIdx);
};
}    // namespace joda::settings
