///
/// \file      results_defines.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

#pragma once

#include <cstdint>
#include <string>
#include "backend/settings/channel/channel_index.hpp"
#include "backend/settings/channel/channel_reporting_settings.hpp"

namespace joda::settings {
class AnalyzeSettings;
}    // namespace joda::settings

namespace joda::results {

///
/// \class      MeasureChannelKey
/// \author     Joachim Danmayr
/// \brief
///
class MeasureChannelKey
{
private:
  static inline const uint32_t MEASURE_CHANNEL_MASK           = 0x00FF0000;
  static inline const uint32_t MEASURE_CHANNEL_STATS_MASK     = 0x0000FF00;
  static inline const uint32_t MEASURE_CHANNEL_STATS_AND_MASK = 0x00FFFF00;
  static inline const uint32_t MEASURE_CHANNEL_INDEX_MASK     = 0x000000FF;

public:
  /////////////////////////////////////////////////////
  MeasureChannelKey(const std::string &value) : value(std::stoul(value))
  {
  }

  MeasureChannelKey(uint32_t value) : value(value)
  {
  }

  MeasureChannelKey(settings::ChannelReportingSettings::MeasureChannelsCombi value) :
      value(static_cast<uint32_t>(value))
  {
  }

  MeasureChannelKey(const MeasureChannelKey &measureChannel,
                    joda::settings::ChannelReportingSettings::MeasureChannelStat measureChannelStat) :
      value((measureChannel | measureChannelStat).value)
  {
  }

  MeasureChannelKey(joda::settings::ChannelReportingSettings::MeasureChannels measureChannel,
                    joda::settings::ChannelReportingSettings::MeasureChannelStat measureChannelStat) :
      value((static_cast<uint32_t>(measureChannel) << 16 | static_cast<uint32_t>(measureChannelStat) << 8) &
            MEASURE_CHANNEL_STATS_AND_MASK)
  {
  }

  MeasureChannelKey(joda::settings::ChannelReportingSettings::MeasureChannels measureChannel,
                    joda::settings::ChannelReportingSettings::MeasureChannelStat measureChannelStat,
                    joda::settings::ChannelIndex channelIndex) :
      value((static_cast<uint32_t>(measureChannel) << 16 | static_cast<uint32_t>(measureChannelStat) << 8 |
             (static_cast<uint32_t>(channelIndex) & MEASURE_CHANNEL_INDEX_MASK)))
  {
  }

  MeasureChannelKey(joda::settings::ChannelReportingSettings::MeasureChannels measureChannel,
                    joda::settings::ChannelIndex channelIndex) :
      value((static_cast<uint32_t>(measureChannel) << 16 |
             static_cast<uint32_t>(joda::settings::ChannelReportingSettings::MeasureChannelStat::VAL) << 8 |
             (static_cast<uint32_t>(channelIndex) & MEASURE_CHANNEL_INDEX_MASK)))
  {
  }

  bool operator<(const MeasureChannelKey &other) const
  {
    return value < other.value;
  }

  MeasureChannelKey operator|(joda::settings::ChannelReportingSettings::MeasureChannelStat stat) const
  {
    return MeasureChannelKey((value & ~MEASURE_CHANNEL_STATS_MASK) | (static_cast<uint32_t>(stat) << 8));
  }

  [[nodiscard]] joda::settings::ChannelReportingSettings::MeasureChannels getMeasureChannel() const;
  [[nodiscard]] joda::settings::ChannelReportingSettings::MeasureChannelStat getMeasureStats() const;
  [[nodiscard]] joda::settings::ChannelIndex getChannelIndex() const;
  [[nodiscard]] std::string measurementStatsToString() const;
  [[nodiscard]] std::string measurementChannelsToString() const;
  [[nodiscard]] std::string measurementChannelsToString(const joda::settings::AnalyzeSettings &settings) const;
  [[nodiscard]] auto toString() const
  {
    return std::to_string(value);
  }

private:
  /////////////////////////////////////////////////////
  uint32_t value;
};

}    // namespace joda::results
