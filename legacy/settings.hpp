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
  static void storeSettings(std::string path, const joda::settings::AnalyzeSettings &settings);
  static int32_t getNrOfAllChannels(const joda::settings::AnalyzeSettings &settings);

  static std::vector<const joda::settings::ChannelSettings *>
  getChannelsOfType(const joda::settings::AnalyzeSettings &settings, joda::settings::ChannelSettingsMeta::Type type);

  static std::string getChannelNameOfChannelIndex(const joda::settings::AnalyzeSettings &settings,
                                                  joda::settings::ChannelIndex channelIdx);

  static const joda::settings::CrossChannelSettings &
  getCrossChannelSettingsForChannel(const joda::settings::AnalyzeSettings &settings,
                                    joda::settings::ChannelIndex channelIdx);

  static void checkSettings(const joda::settings::AnalyzeSettings &settings);
  static bool isEqual(const joda::settings::AnalyzeSettings &settingsOld,
                      const joda::settings::AnalyzeSettings &settingsNew);
};
}    // namespace joda::settings